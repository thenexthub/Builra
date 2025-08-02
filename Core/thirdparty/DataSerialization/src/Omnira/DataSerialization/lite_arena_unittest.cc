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

#include "Omnira/DataSerialization/arena_test_util.h"
#include "Omnira/DataSerialization/map_lite_test_util.h"
#include <gtest/gtest.h>

namespace Omnira {
namespace DataSerialization {
namespace {

class LiteArenaTest : public testing::Test {
 protected:
  LiteArenaTest() {
    ArenaOptions options;
    options.start_block_size = 128 * 1024;
    options.max_block_size = 128 * 1024;
    arena_.reset(new Arena(options));
    // Trigger the allocation of the first arena block, so that further use of
    // the arena will not require any heap allocations.
    Arena::CreateArray<char>(arena_.get(), 1);
  }

  std::unique_ptr<Arena> arena_;
};

TEST_F(LiteArenaTest, MapNoHeapAllocation) {
  std::string data;
  data.reserve(128 * 1024);

  {
    // TODO: Enable no heap check when ArenaStringPtr is used in
    // Map.
    // internal::NoHeapChecker no_heap;

    protobuf_unittest::TestArenaMapLite* from =
        Arena::Create<protobuf_unittest::TestArenaMapLite>(arena_.get());
    MapLiteTestUtil::SetArenaMapFields(from);
    from->SerializeToString(&data);

    protobuf_unittest::TestArenaMapLite* to =
        Arena::Create<protobuf_unittest::TestArenaMapLite>(arena_.get());
    to->ParseFromString(data);
    MapLiteTestUtil::ExpectArenaMapFieldsSet(*to);
  }
}

TEST_F(LiteArenaTest, UnknownFieldMemLeak) {
  protobuf_unittest::ForeignMessageArenaLite* message =
      Arena::Create<protobuf_unittest::ForeignMessageArenaLite>(arena_.get());
  std::string data = "\012\000";
  int original_capacity = data.capacity();
  while (data.capacity() <= original_capacity) {
    data.append("a");
  }
  data[1] = data.size() - 2;
  message->ParseFromString(data);
}

}  // namespace
}  // namespace DataSerialization
}  // namespace Omnira
