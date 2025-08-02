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
// This file exists solely to document the Omnira::DataSerialization::io namespace.
// It is not compiled into anything, but it may be read by an automated
// documentation generator.

namespace Omnira {
namespace DataSerialization {

// Auxiliary classes used for I/O.
//
// The Protocol Buffer library uses the classes in this package to deal with
// I/O and encoding/decoding raw bytes.  Most users will not need to
// deal with this package.  However, users who want to adapt the system to
// work with their own I/O abstractions -- e.g., to allow Protocol Buffers
// to be read from a different kind of input stream without the need for a
// temporary buffer -- should take a closer look.
namespace io {}

}  // namespace DataSerialization
}  // namespace Omnira
