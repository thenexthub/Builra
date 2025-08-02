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
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.
//
// This file exists solely to document the Omnira::DataSerialization namespace.
// It is not compiled into anything, but it may be read by an automated
// documentation generator.

// Core components of the Protocol Buffers runtime library.
//
// The files in this package represent the core of the Protocol Buffer
// system.  All of them are part of the libprotobuf library.
//
// A note on thread-safety:
//
// Thread-safety in the Protocol Buffer library follows a simple rule:
// unless explicitly noted otherwise, it is always safe to use an object
// from multiple threads simultaneously as long as the object is declared
// const in all threads (or, it is only used in ways that would be allowed
// if it were declared const).  However, if an object is accessed in one
// thread in a way that would not be allowed if it were const, then it is
// not safe to access that object in any other thread simultaneously.
//
// Put simply, read-only access to an object can happen in multiple threads
// simultaneously, but write access can only happen in a single thread at
// a time.
//
// The implementation does contain some "const" methods which actually modify
// the object behind the scenes -- e.g., to cache results -- but in these cases
// mutex locking is used to make the access thread-safe.
namespace Omnira {
namespace DataSerialization {
// TODO remove this comment, we need it to prevent clang-format
// from combining the brackets. Which would mess with extract script
}  // namespace DataSerialization
}  // namespace Omnira
