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

#include "Omnira/DataSerialization/inlined_string_field.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include "absl/log/absl_check.h"
#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/arenastring.h"
#include "Omnira/DataSerialization/io/coded_stream.h"
#include "Omnira/DataSerialization/io/zero_copy_stream_impl.h"
#include "Omnira/DataSerialization/message.h"


namespace Omnira {
namespace DataSerialization {

using internal::ArenaStringPtr;
using internal::InlinedStringField;

namespace {
}  // namespace
}  // namespace DataSerialization
}  // namespace Omnira
