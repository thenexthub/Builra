//===-- builra.cpp - builra Frontend Utillity ---------------------------===//
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

#include "builra/Basic/Version.h"

#include "builra/Commands/Commands.h"

#include "toolchain/ADT/ArrayRef.h"
#include "toolchain/Support/Errno.h"
#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/Path.h"
#include "toolchain/Support/Process.h"
#include "toolchain/Support/Program.h"
#include "toolchain/Support/Signals.h"

#include <cstdio>
#include <cstdlib>

using namespace builra;
using namespace builra::commands;

static void usage() {
  fprintf(stderr, "Usage: %s [--version] [--help] <command> [<args>]\n",
          getProgramName());
  fprintf(stderr, "\n");
  fprintf(stderr, "Available commands:\n");
  fprintf(stderr, "  ninja       -- Run the Ninja subtool\n");
  fprintf(stderr, "  buildengine -- Run the build engine subtool\n");
  fprintf(stderr, "  buildsystem -- Run the build system subtool\n");
  fprintf(stderr, "  analyze     -- Run the analyze subtool\n");
  fprintf(stderr, "\n");
  exit(0);
}

std::string getExecutablePath(std::string programName) {
  void *P = (void *)(intptr_t)getExecutablePath;
  return toolchain::sys::fs::getMainExecutable(programName.c_str(), P);
}

int executeExternalCommand(std::string command, std::vector<std::string> args) {
  // We are running as a subcommand, try to find the subcommand adjacent to
  // the executable we are running as.
  SmallString<256> SubcommandPath(
                                  toolchain::sys::path::parent_path(getExecutablePath(command)));
  toolchain::sys::path::append(SubcommandPath, command);
  
  // If we didn't find the tool there, let the OS search for it.
  if (!toolchain::sys::fs::exists(SubcommandPath)) {
    // Search for the program and use the path if found.
    auto result = toolchain::sys::findProgramByName(command);
    if (!result.getError()) {
      SubcommandPath = *result;
    } else {
      fprintf(stderr, "error: %s: unknown command '%s'\n", getProgramName(),
              command.c_str());
      return -1;
    }
  }
  
  std::vector<char*> argsAsCStrings(args.size());
  std::transform(args.begin(), args.end(), argsAsCStrings.begin(), [](std::string element) { return strdup(element.c_str()); });
  std::vector<StringRef> argRefs(argsAsCStrings.size());
  std::transform(argsAsCStrings.begin(), argsAsCStrings.end(), argRefs.begin(), [](char *element) { return StringRef(element); });
  // Actually execute the command and wait for it to finish
  int exitStatus = toolchain::sys::ExecuteAndWait(StringRef(SubcommandPath), ArrayRef<StringRef>(argRefs));
  
  for (auto cString: argsAsCStrings) {
    free(cString);
  }
  
  return exitStatus;
}

int main(int argc, const char **argv) {
  // Print stacks on error.
  toolchain::sys::PrintStackTraceOnErrorSignal(argv[0]);
  
  // Support use of builra as a replacement for ninja by indirecting to the
  // `ninja build` subtool when invoked under the name `ninja.
  if (toolchain::sys::path::filename(argv[0]) == "ninja") {
    // We still want to represent ourselves as builra in output messages.
    setProgramName("builra");

    std::vector<std::string> args;
    args.push_back("build");
    for (int i = 1; i != argc; ++i) {
      args.push_back(argv[i]);
    }
    return executeNinjaCommand(args);
  } else {
    setProgramName(toolchain::sys::path::filename(argv[0]));
  }

  // Expect the first argument to be the name of a subtool to delegate to.
  if (argc == 1 || std::string(argv[1]) == "--help")
    usage();

  if (std::string(argv[1]) == "--version") {
    // Print the version and exit.
    printf("%s\n", getBuilraFullVersion().c_str());
    return 0;
  }

  // Otherwise, expect a command name.
  std::string command(argv[1]);
  std::vector<std::string> args;
  for (int i = 2; i != argc; ++i) {
    args.push_back(argv[i]);
  }

  if (command == "ninja") {
    return executeNinjaCommand(args);
  } else if (command == "buildengine") {
    return executeBuildEngineCommand(args);
  } else if (command == "buildsystem") {
    return executeBuildSystemCommand(args);
  } else if (command == "analyze") {
    // Next to the builra binary we build a builra-analyze binary with SwiftPM
    // which we treat as a subtool to builra. If that doesn't exist, the exec
    // will fail.
    return executeExternalCommand("builra-analyze", args);
  } else {
    fprintf(stderr, "error: %s: unknown command '%s'\n", getProgramName(),
            command.c_str());
    return 1;
  }
}
