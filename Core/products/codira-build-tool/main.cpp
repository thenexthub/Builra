//===-- swift-build-tool.cpp - Swift Build Tool ---------------------------===//
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

#include "builra/BuildSystem/BuildSystemFrontend.h"

#include "builra/Basic/FileSystem.h"
#include "builra/Basic/Version.h"
#include "builra/BuildSystem/BuildDescription.h"
#include "builra/BuildSystem/BuildFile.h"
#include "builra/BuildSystem/Command.h"
#include "builra/BuildSystem/Tool.h"

#include "toolchain/Support/SourceMgr.h"
#include "toolchain/Support/Signals.h"
#include "toolchain/Support/raw_ostream.h"

using namespace builra;
using namespace builra::buildsystem;

class BasicBuildSystemFrontendDelegate : public BuildSystemFrontendDelegate {
public:
  BasicBuildSystemFrontendDelegate(toolchain::SourceMgr& sourceMgr,
                                   const BuildSystemInvocation& invocation)
    : BuildSystemFrontendDelegate(sourceMgr,
                                  "swift-build", /*version=*/0) {}

  virtual void hadCommandFailure() override {
    // Call the base implementation.
    BuildSystemFrontendDelegate::hadCommandFailure();

    // Cancel the build, by default.
    cancel();
  }
  
  virtual std::unique_ptr<Tool> lookupTool(StringRef name) override {
    return nullptr;
  }

  virtual void cycleDetected(const std::vector<core::Rule*>& items) override {
    auto message = BuildSystemInvocation::formatDetectedCycle(items);
    error(message);
  }
};

static void usage(int exitCode) {
  int optionWidth = 25;
  fprintf(stderr, "Usage: swift-build-tool [options] [<target>]\n");
  fprintf(stderr, "\nOptions:\n");
  BuildSystemInvocation::getUsage(optionWidth, toolchain::errs());
  ::exit(exitCode);
}

static int execute(ArrayRef<std::string> args) {
  // The source manager to use for diagnostics.
  toolchain::SourceMgr sourceMgr;

  // Create the invocation.
  BuildSystemInvocation invocation{};

  // Initialize defaults.
  invocation.dbPath = "build.db";
  invocation.buildFilePath = "build.code-build";
  invocation.parse(args, sourceMgr);

  // Handle invocation actions.
  if (invocation.showUsage) {
    usage(0);
  } else if (invocation.showVersion) {
    // Print the version and exit.
    printf("%s\n", getBuilraFullVersion("swift-build-tool").c_str());
    return 0;
  } else if (invocation.hadErrors) {
    usage(1);
  }
  
  if (invocation.positionalArgs.size() > 1) {
    fprintf(stderr, "swift-build-tool: error: invalid number of arguments\n");
    usage(1);
  }

  // Select the target to build.
  std::string targetToBuild =
    invocation.positionalArgs.empty() ? "" : invocation.positionalArgs[0];

  // Create the frontend object.
  BasicBuildSystemFrontendDelegate delegate(sourceMgr, invocation);
  BuildSystemFrontend frontend(delegate, invocation, basic::createLocalFileSystem());
  if (!frontend.build(targetToBuild)) {
    return 1;
  }

  return 0;
}

int main(int argc, const char **argv) {
  // Print stacks on error.
  toolchain::sys::PrintStackTraceOnErrorSignal(argv[0]);

  std::vector<std::string> args;
  for (int i = 1; i != argc; ++i) {
    args.push_back(argv[i]);
  }
  return execute(args);
}
