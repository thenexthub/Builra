//===- toolchain/Support/Win32/Mutex.inc - Win32 Mutex Implementation -*- C++ -*-===//
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
// This file implements the Win32 specific (non-pthread) Mutex class.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic Win32 code that
//===          is guaranteed to work on *all* Win32 variants.
//===----------------------------------------------------------------------===//

#include "WindowsSupport.h"
#include "toolchain/Support/Mutex.h"

namespace toolchain {

sys::MutexImpl::MutexImpl(bool /*recursive*/)
{
  data_ = new CRITICAL_SECTION;
  InitializeCriticalSection((LPCRITICAL_SECTION)data_);
}

sys::MutexImpl::~MutexImpl()
{
  DeleteCriticalSection((LPCRITICAL_SECTION)data_);
  delete (LPCRITICAL_SECTION)data_;
  data_ = 0;
}

bool
sys::MutexImpl::acquire()
{
  EnterCriticalSection((LPCRITICAL_SECTION)data_);
  return true;
}

bool
sys::MutexImpl::release()
{
  LeaveCriticalSection((LPCRITICAL_SECTION)data_);
  return true;
}

bool
sys::MutexImpl::tryacquire()
{
  return TryEnterCriticalSection((LPCRITICAL_SECTION)data_);
}

}
