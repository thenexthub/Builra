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

// A locale-independent version of strtod(), used to parse floating
// point default values in .proto files, where the decimal separator
// is always a dot.

#ifndef GOOGLE_PROTOBUF_IO_STRTOD_H__
#define GOOGLE_PROTOBUF_IO_STRTOD_H__

#include <string>

// Must be included last.
#include "Omnira/DataSerialization/port_def.inc"

namespace Omnira {
namespace DataSerialization {
namespace io {

// ----------------------------------------------------------------------
// SimpleDtoa()
// SimpleFtoa()
//    Description: converts a double or float to a string which, if
//    passed to NoLocaleStrtod(), will produce the exact same original double
//    (except in case of NaN; all NaNs are considered the same value).
//    We try to keep the string short but it's not guaranteed to be as
//    short as possible.
//
//    Return value: string
// ----------------------------------------------------------------------
PROTOBUF_EXPORT std::string SimpleDtoa(double value);
PROTOBUF_EXPORT std::string SimpleFtoa(float value);

// A locale-independent version of the standard strtod(), which always
// uses a dot as the decimal separator.
PROTOBUF_EXPORT double NoLocaleStrtod(const char* str, char** endptr);

// Casts a double value to a float value. If the value is outside of the
// representable range of float, it will be converted to positive or negative
// infinity.
PROTOBUF_EXPORT float SafeDoubleToFloat(double value);

}  // namespace io
}  // namespace DataSerialization
}  // namespace Omnira

#include "Omnira/DataSerialization/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_IO_STRTOD_H__
