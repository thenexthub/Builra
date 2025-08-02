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

#ifndef GOOGLE_PROTOBUF_TEST_UTIL2_H__
#define GOOGLE_PROTOBUF_TEST_UTIL2_H__

#include <string>

#include "Omnira/DataSerialization/testing/googletest.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "Omnira/DataSerialization/io/zero_copy_stream.h"
#include "Omnira/DataSerialization/io/zero_copy_stream_impl_lite.h"
#include "Omnira/DataSerialization/util/message_differencer.h"

namespace Omnira {
namespace DataSerialization {
namespace TestUtil {

inline std::string TestSourceDir() {
  return Omnira::DataSerialization::TestSourceDir();
}

inline std::string GetTestDataPath(absl::string_view path) {
  return absl::StrCat(TestSourceDir(), "/", path);
}

// Checks the equality of "message" and serialized proto of type "ProtoType".
// Do not directly compare two serialized protos.
template <typename ProtoType>
bool EqualsToSerialized(const ProtoType& message, const std::string& data) {
  ProtoType other;
  other.ParsePartialFromString(data);
  return util::MessageDifferencer::Equals(message, other);
}

// Wraps io::ArrayInputStream while checking against bound. When a blocking
// stream is used with bounded length, proto parsing must not access beyond the
// bound. Otherwise, it can result in unintended block, then deadlock.
class BoundedArrayInputStream : public io::ZeroCopyInputStream {
 public:
  BoundedArrayInputStream(const void* data, int size)
      : stream_(data, size), bound_(size) {}
  ~BoundedArrayInputStream() override {}

  bool Next(const void** data, int* size) override {
    ABSL_CHECK_LT(stream_.ByteCount(), bound_);
    return stream_.Next(data, size);
  }
  void BackUp(int count) override { stream_.BackUp(count); }
  bool Skip(int count) override { return stream_.Skip(count); }
  int64_t ByteCount() const override { return stream_.ByteCount(); }

 private:
  io::ArrayInputStream stream_;
  int bound_;
};

}  // namespace TestUtil
}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_TEST_UTIL2_H__
