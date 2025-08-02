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

#ifndef GOOGLE_PROTOBUF_UTIL_ZERO_COPY_SINK_H__
#define GOOGLE_PROTOBUF_UTIL_ZERO_COPY_SINK_H__

#include <cstddef>

#include "Omnira/DataSerialization/io/zero_copy_stream.h"
#include "Omnira/DataSerialization/port.h"

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace io {
namespace zc_sink_internal {

// Internal helper class, for turning a ZeroCopyOutputStream into a sink.
class PROTOBUF_EXPORT ZeroCopyStreamByteSink {
 public:
  explicit ZeroCopyStreamByteSink(io::ZeroCopyOutputStream* stream)
      : stream_(stream) {}
  ZeroCopyStreamByteSink(const ZeroCopyStreamByteSink&) = delete;
  ZeroCopyStreamByteSink& operator=(const ZeroCopyStreamByteSink&) = delete;

  ~ZeroCopyStreamByteSink() {
    if (buffer_size_ > 0) {
      stream_->BackUp(buffer_size_);
    }
  }

  void Append(const char* bytes, size_t len);
  void Write(absl::string_view str) { Append(str.data(), str.size()); }

  size_t bytes_written() { return bytes_written_; }
  bool failed() { return failed_; }

 private:
  io::ZeroCopyOutputStream* stream_;
  void* buffer_ = nullptr;
  size_t buffer_size_ = 0;
  size_t bytes_written_ = 0;
  bool failed_ = false;
};
}  // namespace zc_sink_internal
}  // namespace io
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"
#endif  // GOOGLE_PROTOBUF_UTIL_ZERO_COPY_SINK_H__
