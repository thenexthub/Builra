//===- Common.h -------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_COMMON_H
#define BUILRA3_COMMON_H

#include <string>

#define UUID_SYSTEM_GENERATOR 1
#include "uuid.h"

#include "builra3/Common.pb.h"


namespace builra3 {

typedef uuids::uuid EngineID;

inline EngineID generateEngineID() { return uuids::uuid_system_generator()(); }

inline Stat makeStat(std::string_view name, int64_t value) {
  Stat s;
  s.set_name(name);
  s.set_int_value(value);
  return s;
}

inline Stat makeStat(std::string_view name, uint64_t value) {
  Stat s;
  s.set_name(name);
  s.set_uint_value(value);
  return s;
}

inline Stat makeStat(std::string_view name, std::string_view value) {
  Stat s;
  s.set_name(name);
  s.set_string_value(value);
  return s;
}

inline Stat makeStatB(std::string_view name, bool value) {
  Stat s;
  s.set_name(name);
  s.set_bool_value(value);
  return s;
}

inline Stat makeStat(std::string_view name, double value) {
  Stat s;
  s.set_name(name);
  s.set_double_value(value);
  return s;
}

inline Stat makeStat(std::string_view name, const CASID& value) {
  Stat s;
  s.set_name(name);
  *s.mutable_cas_object() = value;
  return s;
}

inline Stat makeStat(std::string_view name, const Error& value) {
  Stat s;
  s.set_name(name);
  *s.mutable_error_value() = value;
  return s;
}
}

#endif
