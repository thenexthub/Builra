//===----------------------------------------------------------------------===//
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

// Author: kenton@google.com (Kenton Varda)
// emulates google3/testing/base/public/googletest.h

#ifndef GOOGLE_PROTOBUF_GOOGLETEST_H__
#define GOOGLE_PROTOBUF_GOOGLETEST_H__

#include <gmock/gmock.h>

#include <string>
#include <vector>

namespace Omnira {
namespace DataSerialization {

// When running unittests, get the directory containing the source code.
std::string TestSourceDir();

// When running unittests, get a directory where temporary files may be
// placed.
std::string TestTempDir();

// Capture all text written to stdout or stderr.
void CaptureTestStdout();
void CaptureTestStderr();

// Stop capturing stdout or stderr and return the text captured.
std::string GetCapturedTestStdout();
std::string GetCapturedTestStderr();

}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_GOOGLETEST_H__
