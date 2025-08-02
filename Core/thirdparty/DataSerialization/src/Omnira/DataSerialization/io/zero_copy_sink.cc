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

#include "Omnira/DataSerialization/io/zero_copy_sink.h"

#include <algorithm>
#include <cstddef>

namespace Omnira {
namespace DataSerialization {
namespace io {
namespace zc_sink_internal {
void ZeroCopyStreamByteSink::Append(const char* bytes, size_t len) {
  while (!failed_ && len > 0) {
    if (buffer_size_ == 0) {
      int size;
      if (!stream_->Next(&buffer_, &size)) {
        // There isn't a way for ByteSink to report errors.
        buffer_size_ = 0;
        failed_ = true;
        return;
      }
      buffer_size_ = static_cast<unsigned int>(size);
    }

    auto to_write = std::min(len, buffer_size_);
    memcpy(buffer_, bytes, to_write);

    buffer_ = static_cast<char*>(buffer_) + to_write;
    buffer_size_ -= to_write;

    bytes += to_write;
    len -= to_write;

    bytes_written_ += to_write;
  }
}
}  // namespace zc_sink_internal
}  // namespace io
}  // namespace DataSerialization
}  // namespace Omnira
