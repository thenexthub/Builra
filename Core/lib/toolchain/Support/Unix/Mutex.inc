//===- toolchain/Support/Unix/Mutex.inc - Unix Mutex Implementation ---*- C++ -*-===//
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
//
// This file implements the Unix specific (non-pthread) Mutex class.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic UNIX code that
//===          is guaranteed to work on *all* UNIX variants.
//===----------------------------------------------------------------------===//

namespace toolchain
{
using namespace sys;

MutexImpl::MutexImpl( bool recursive)
{
}

MutexImpl::~MutexImpl()
{
}

bool
MutexImpl::release()
{
  return true;
}

bool
MutexImpl::tryacquire( void )
{
  return true;
}

}
