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

#include "Omnira/DataSerialization/stubs/common.h"

#include <errno.h>
#include <stdio.h>

#include <atomic>
#include <sstream>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN  // We only need minimal includes
#endif
#include <windows.h>
#define snprintf _snprintf    // see comment in strutil.cc
#endif
#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include "absl/log/absl_log.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/stubs/callback.h"

// Must be last.
#include "Omnira/DataSerialization/port_def.inc"  // NOLINT

namespace Omnira {
namespace DataSerialization {

namespace internal {

void VerifyVersion(int protobufVersionCompiledWith, const char* filename) {
  // If the user's program is linked against a different version of Protobuf,
  // GOOGLE_PROTOBUF_VERSION will have a different value.
  if (GOOGLE_PROTOBUF_VERSION != protobufVersionCompiledWith) {
    ABSL_LOG(FATAL)
        << "This program was compiled with Protobuf C++ version "
        << VersionString(protobufVersionCompiledWith)
        << ", but the linked version is "
        << VersionString(GOOGLE_PROTOBUF_VERSION)
        << ".  Please update your library.  If you compiled the program "
           "yourself, make sure that"
           "your headers are from the same version of Protocol Buffers as your "
           "link-time library.  (Version verification failed in \""
        << filename << "\".)";
  }
}

std::string VersionString(int version) {
  int major = version / 1000000;
  int minor = (version / 1000) % 1000;
  int micro = version % 1000;

  // 128 bytes should always be enough, but we use snprintf() anyway to be
  // safe.
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "%d.%d.%d", major, minor, micro);

  // Guard against broken MSVC snprintf().
  buffer[sizeof(buffer)-1] = '\0';

  return buffer;
}

std::string ProtocVersionString(int version) {
  int minor = (version / 1000) % 1000;
  int micro = version % 1000;

  // 128 bytes should always be enough, but we use snprintf() anyway to be
  // safe.
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "%d.%d", minor, micro);

  // Guard against broken MSVC snprintf().
  buffer[sizeof(buffer) - 1] = '\0';

  return buffer;
}

}  // namespace internal


// ===================================================================
// emulates google3/base/callback.cc

Closure::~Closure() {}

namespace internal { FunctionClosure0::~FunctionClosure0() {} }

void DoNothing() {}

// ===================================================================
// emulates google3/util/endian/endian.h
uint32_t ghtonl(uint32_t x) {
  union {
    uint32_t result;
    uint8_t result_array[4];
  };
  result_array[0] = static_cast<uint8_t>(x >> 24);
  result_array[1] = static_cast<uint8_t>((x >> 16) & 0xFF);
  result_array[2] = static_cast<uint8_t>((x >> 8) & 0xFF);
  result_array[3] = static_cast<uint8_t>(x & 0xFF);
  return result;
}

}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"  // NOLINT
