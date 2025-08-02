//===-- BuildSystemExtensionManager.cpp -----------------------------------===//
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
//===----------------------------------------------------------------------===//

#include "builra/BuildSystem/BuildSystemExtensions.h"

#include "builra/Basic/Subprocess.h"
#include "builra/Basic/PlatformUtility.h"

#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/raw_ostream.h"

using namespace builra;
using namespace builra::basic;
using namespace builra::buildsystem;

HandlerState::~HandlerState() {}
ShellCommandHandler::~ShellCommandHandler() {}
BuildSystemExtension::~BuildSystemExtension() {}

#pragma mark - BuildSystemExtensionManager implementation

BuildSystemExtension*
BuildSystemExtensionManager::lookupByCommandPath(StringRef path) {
  std::lock_guard<std::mutex> guard(extensionsLock);

  // Check the cache.
  auto it = extensions.find(path);
  if (it != extensions.end()) return it->second.get();

  // Register negative hit, unless we succeed.
  extensions[path] = nullptr;

  // If missing, look for an extension for this path.
  //
  // Currently, extensions are discovered by expecting that a command has an
  // adjacent "...-for-builra" binary which can be queried for info.
  SmallString<256> infoPath{ path };
  infoPath += "-for-builra";
  if (!toolchain::sys::fs::exists(infoPath)) {
    return {};
  }

  // If the path exists, then query it to find the actual extension library.
  struct CapturingProcessDelegate: ProcessDelegate {
    SmallString<1024> output;
    bool success;
    
    virtual void processStarted(ProcessContext* ctx, ProcessHandle handle,
                                builra_pid_t pid) {}

    virtual void processHadError(ProcessContext* ctx, ProcessHandle handle,
                                 const Twine& message) {};

    virtual void processHadOutput(ProcessContext* ctx, ProcessHandle handle,
                                  StringRef data) {
      output += data;
    };

    virtual void processFinished(ProcessContext* ctx, ProcessHandle handle,
                                 const ProcessResult& result) {
      success = (result.status == ProcessStatus::Succeeded &&
                 result.exitCode == 0);
    }
  };
  CapturingProcessDelegate delegate;
  {
    // FIXME: Add a utility for capturing a subprocess infocation.
    ProcessAttributes attr{/*canSafelyInterrupt=*/true};
    ProcessGroup pgrp;
    ProcessHandle handle{0};
    std::vector<StringRef> cmd{infoPath, "--builra-extension-version", "0",
        "--extension-path" };
    ProcessReleaseFn releaseFn = [](std::function<void()>&& pwait){ pwait(); };
    ProcessCompletionFn completionFn = [](ProcessResult){};
    spawnProcess(delegate, nullptr, pgrp, handle, cmd, POSIXEnvironment(), attr,
                 std::move(releaseFn), std::move(completionFn));
  }

  // The output is expected to be the exact path to the extension (no extra
  // whitespace, etc.).
  auto extensionPath = delegate.output;
  if (!delegate.success || !toolchain::sys::fs::exists(infoPath)) {
    return {};
  }

  // Load the plugin.
  auto handle = sys::OpenLibrary(extensionPath.c_str());
  if (handle == nullptr)
    return {};

  BuildSystemExtension *(*registrationFn)(void) =
      reinterpret_cast<decltype(registrationFn)>(sys::GetSymbolByname(handle,
                                                                      "initialize_builra_buildsystem_extension_v0"));
  if (!registrationFn) {
    sys::CloseLibrary(handle);
    return {};
  }

  // For now, we expect the registration to simply allocate and return us a (C)
  // extension instance.
  //
  // FIXME: This needs to be reworked to go through a C API.
  auto *extension = registrationFn();
  if (!extension) {
    sys::CloseLibrary(handle);
    return {};
  }

  extensions[path] = std::unique_ptr<BuildSystemExtension>(extension);
  return extension;
}

