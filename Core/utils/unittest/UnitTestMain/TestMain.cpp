//===--- utils/unittest/UnitTestMain/TestMain.cpp - unittest driver -------===//
//
// Part of the TOOLCHAIN Project, under the Apache License v2.0 with TOOLCHAIN Exceptions.
// See https://toolchain.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH TOOLCHAIN-exception
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/CommandLine.h"
#include "toolchain/Support/Signals.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <stdlib.h>

#if defined(_WIN32)
# include <windows.h>
# if defined(_MSC_VER)
#   include <crtdbg.h>
# endif
#endif

const char *TestMainArgv0;

int main(int argc, char **argv) {
  // Skip setting up signal handlers for tests that need to test things without
  // them configured.
  if (!getenv("TOOLCHAIN_PROGRAM_TEST_NO_STACKTRACE_HANDLER")) {
    toolchain::sys::PrintStackTraceOnErrorSignal(argv[0],
                                            true /* Disable crash reporting */);
  }

  // Initialize both gmock and gtest.
  testing::InitGoogleMock(&argc, argv);

  toolchain::cl::ParseCommandLineOptions(argc, argv);

  // Make it easy for a test to re-execute itself by saving argv[0].
  TestMainArgv0 = argv[0];

# if defined(_WIN32)
  // Disable all of the possible ways Windows conspires to make automated
  // testing impossible.
  ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#   if defined(_MSC_VER)
    ::_set_error_mode(_OUT_TO_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#   endif
# endif

  return RUN_ALL_TESTS();
}
