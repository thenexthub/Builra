//===- Signals.cpp - Signal Handling support --------------------*- C++ -*-===//
//
// Copyright (c) NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
//
//===----------------------------------------------------------------------===//
//
// This file defines some helpful functions for dealing with the possibility of
// Unix signals occurring while your program is running.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Signals.h"
#include "toolchain/ADT/STLExtras.h"
#include "toolchain/ADT/StringRef.h"
#include "toolchain/Config/toolchain-config.h"
#include "toolchain/Support/ErrorOr.h"
#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/FileUtilities.h"
#include "toolchain/Support/Format.h"
#include "toolchain/Support/ManagedStatic.h"
#include "toolchain/Support/MemoryBuffer.h"
#include "toolchain/Support/Mutex.h"
#include "toolchain/Support/Program.h"
#include "toolchain/Support/StringSaver.h"
#include "toolchain/Support/raw_ostream.h"
#include "toolchain/Support/Options.h"
#include <vector>

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

using namespace toolchain;

// Use explicit storage to avoid accessing cl::opt in a signal handler.
static bool DisableSymbolicationFlag = false;

#if defined(TOOLCHAIN_CODE_DISABLED_FOR_BUILRA)
static cl::opt<bool, true>
    DisableSymbolication("disable-symbolication",
                         cl::desc("Disable symbolizing crash backtraces."),
                         cl::location(DisableSymbolicationFlag), cl::Hidden);
#endif

// Callbacks to run in signal handler must be lock-free because a signal handler
// could be running as we add new callbacks. We don't add unbounded numbers of
// callbacks, an array is therefore sufficient.
struct CallbackAndCookie {
  sys::SignalHandlerCallback Callback;
  void *Cookie;
  enum class Status { Empty, Initializing, Initialized, Executing };
  std::atomic<Status> Flag;
};
static constexpr size_t MaxSignalHandlerCallbacks = 8;
static CallbackAndCookie CallBacksToRun[MaxSignalHandlerCallbacks];

// Signal-safe.
void sys::RunSignalHandlers() {
  for (size_t I = 0; I < MaxSignalHandlerCallbacks; ++I) {
    auto &RunMe = CallBacksToRun[I];
    auto Expected = CallbackAndCookie::Status::Initialized;
    auto Desired = CallbackAndCookie::Status::Executing;
    if (!RunMe.Flag.compare_exchange_strong(Expected, Desired))
      continue;
    (*RunMe.Callback)(RunMe.Cookie);
    RunMe.Callback = nullptr;
    RunMe.Cookie = nullptr;
    RunMe.Flag.store(CallbackAndCookie::Status::Empty);
  }
}

// Signal-safe.
static void insertSignalHandler(sys::SignalHandlerCallback FnPtr,
                                void *Cookie) {
  for (size_t I = 0; I < MaxSignalHandlerCallbacks; ++I) {
    auto &SetMe = CallBacksToRun[I];
    auto Expected = CallbackAndCookie::Status::Empty;
    auto Desired = CallbackAndCookie::Status::Initializing;
    if (!SetMe.Flag.compare_exchange_strong(Expected, Desired))
      continue;
    SetMe.Callback = FnPtr;
    SetMe.Cookie = Cookie;
    SetMe.Flag.store(CallbackAndCookie::Status::Initialized);
    return;
  }
  report_fatal_error("too many signal callbacks already registered");
}

static bool findModulesAndOffsets(void **StackTrace, int Depth,
                                  const char **Modules, intptr_t *Offsets,
                                  const char *MainExecutableName,
                                  StringSaver &StrPool);

/// Format a pointer value as hexadecimal. Zero pad it out so its always the
/// same width.
static FormattedNumber format_ptr(void *PC) {
  // Each byte is two hex digits plus 2 for the 0x prefix.
  unsigned PtrWidth = 2 + 2 * sizeof(void *);
  return format_hex((uint64_t)PC, PtrWidth);
}

/// Helper that launches toolchain-symbolizer and symbolizes a backtrace.
TOOLCHAIN_ATTRIBUTE_USED
static bool printSymbolizedStackTrace(StringRef Argv0, void **StackTrace,
                                      int Depth, toolchain::raw_ostream &OS) {
  if (DisableSymbolicationFlag)
    return false;

  // Don't recursively invoke the toolchain-symbolizer binary.
  if (Argv0.find("toolchain-symbolizer") != std::string::npos)
    return false;

  // FIXME: Subtract necessary number from StackTrace entries to turn return addresses
  // into actual instruction addresses.
  // Use toolchain-symbolizer tool to symbolize the stack traces. First look for it
  // alongside our binary, then in $PATH.
  ErrorOr<std::string> TOOLCHAINSymbolizerPathOrErr = std::error_code();
  if (!Argv0.empty()) {
    StringRef Parent = toolchain::sys::path::parent_path(Argv0);
    if (!Parent.empty())
      TOOLCHAINSymbolizerPathOrErr = sys::findProgramByName("toolchain-symbolizer", Parent);
  }
  if (!TOOLCHAINSymbolizerPathOrErr)
    TOOLCHAINSymbolizerPathOrErr = sys::findProgramByName("toolchain-symbolizer");
  if (!TOOLCHAINSymbolizerPathOrErr)
    return false;
  const std::string &TOOLCHAINSymbolizerPath = *TOOLCHAINSymbolizerPathOrErr;

  // If we don't know argv0 or the address of main() at this point, try
  // to guess it anyway (it's possible on some platforms).
  std::string MainExecutableName =
      Argv0.empty() ? sys::fs::getMainExecutable(nullptr, nullptr)
                    : (std::string)Argv0;
  BumpPtrAllocator Allocator;
  StringSaver StrPool(Allocator);
  std::vector<const char *> Modules(Depth, nullptr);
  std::vector<intptr_t> Offsets(Depth, 0);
  if (!findModulesAndOffsets(StackTrace, Depth, Modules.data(), Offsets.data(),
                             MainExecutableName.c_str(), StrPool))
    return false;
  int InputFD;
  SmallString<32> InputFile, OutputFile;
  sys::fs::createTemporaryFile("symbolizer-input", "", InputFD, InputFile);
  sys::fs::createTemporaryFile("symbolizer-output", "", OutputFile);
  FileRemover InputRemover(InputFile.c_str());
  FileRemover OutputRemover(OutputFile.c_str());

  {
    raw_fd_ostream Input(InputFD, true);
    for (int i = 0; i < Depth; i++) {
      if (Modules[i])
        Input << Modules[i] << " " << (void*)Offsets[i] << "\n";
    }
  }

  Optional<StringRef> Redirects[] = {StringRef(InputFile),
                                     StringRef(OutputFile), toolchain::None};
  StringRef Args[] = {"toolchain-symbolizer", "--functions=linkage", "--inlining",
#ifdef _WIN32
                      // Pass --relative-address on Windows so that we don't
                      // have to add ImageBase from PE file.
                      // FIXME: Make this the default for toolchain-symbolizer.
                      "--relative-address",
#endif
                      "--demangle"};
  int RunResult =
      sys::ExecuteAndWait(TOOLCHAINSymbolizerPath, Args, None, Redirects);
  if (RunResult != 0)
    return false;

  // This report format is based on the sanitizer stack trace printer.  See
  // sanitizer_stacktrace_printer.cc in compiler-rt.
  auto OutputBuf = MemoryBuffer::getFile(OutputFile.c_str());
  if (!OutputBuf)
    return false;
  StringRef Output = OutputBuf.get()->getBuffer();
  SmallVector<StringRef, 32> Lines;
  Output.split(Lines, "\n");
  auto CurLine = Lines.begin();
  int frame_no = 0;
  for (int i = 0; i < Depth; i++) {
    if (!Modules[i]) {
      OS << '#' << frame_no++ << ' ' << format_ptr(StackTrace[i]) << '\n';
      continue;
    }
    // Read pairs of lines (function name and file/line info) until we
    // encounter empty line.
    for (;;) {
      if (CurLine == Lines.end())
        return false;
      StringRef FunctionName = *CurLine++;
      if (FunctionName.empty())
        break;
      OS << '#' << frame_no++ << ' ' << format_ptr(StackTrace[i]) << ' ';
      if (!FunctionName.startswith("??"))
        OS << FunctionName << ' ';
      if (CurLine == Lines.end())
        return false;
      StringRef FileLineInfo = *CurLine++;
      if (!FileLineInfo.startswith("??"))
        OS << FileLineInfo;
      else
        OS << "(" << Modules[i] << '+' << format_hex(Offsets[i], 0) << ")";
      OS << "\n";
    }
  }
  return true;
}

// Include the platform-specific parts of this class.
#ifdef TOOLCHAIN_ON_UNIX
#include "Unix/Signals.inc"
#endif
#ifdef _WIN32
#include "Windows/Signals.inc"
#endif
