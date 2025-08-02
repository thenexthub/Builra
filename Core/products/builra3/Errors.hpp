//===- Errors.hpp -----------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_ERRORS_H
#define BUILRA3_ERRORS_H

#include <cstdint>
#include <type_traits>

namespace builra3 {

enum class EngineError: uint64_t {
  // 100 - Graph Errors
  NoArtifactProducer = 100,
  NoProviderForRule = 101,

  // 200 - Client Implementation Errors
  DuplicateRuleProvider = 200,
  DuplicateArtifactProvider = 201,
  DuplicateRule = 202,
  DuplicateRuleArtifact = 203,
  DuplicateTask = 204,
  DuplicateTaskArtifact = 205,
  UnrequestedInput = 206,
  InvalidNextState = 207,
  RuleConstructionFailed = 208,
  TaskConstructionFailed = 209,
  TaskPropertyViolation = 210,
  InvalidEngineState = 211,

  // 1000 - Engine Internal Errors
  Unimplemented = 1000,
  InternalInconsistency = 1001,
  InternalProtobufSerialization = 1002,

  // Unknown
  Unknown = 0
};

enum class CASError: uint64_t {
  ObjectNotFound = 100,
  IOError = 101,
  StreamStall = 102,

  // Unknown
  Unknown = 0
};

enum class ExecutorError: uint64_t {

  // 100 - Process Errors
  FileNotFound = 100,
  IOError = 101,
  ProcessSpawnFailed = 102,
  WaitFailed = 103,
  ControlProtocolError = 104,
  ProcessStatsError = 105,
  ProcessFailed = 106,
  UnexpectedOutput = 107,

  // 200 - Client Implementation Errors
  BadRequest = 200,
  DuplicateProvider = 201,
  BadProviderPrefix = 202,
  NoProvider = 203,
  NoRemoteExecutor = 204,

  // 1000 - Executor Internal Errors
  Unimplemented = 1000,
  InternalInconsistency = 1001,
  InternalProtobufSerialization = 1002,

  Unknown = 0
};


namespace internal
{
    template <typename E>
    using UnderlyingType = typename std::underlying_type<E>::type;

    template <typename E>
    using EnumTypesOnly = typename std::enable_if<std::is_enum<E>::value, E>::type;

}   // namespace internal


template <typename E, typename = internal::EnumTypesOnly<E>>
constexpr internal::UnderlyingType<E> rawCode(E e) {
    return static_cast<internal::UnderlyingType<E>>(e);
}


}

#endif /* Header_h */
