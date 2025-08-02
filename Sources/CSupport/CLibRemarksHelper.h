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

#ifndef CLibRemarksHelper_h
#define CLibRemarksHelper_h

#include <stdbool.h>

#include "CSupportDefines.h"

// Swift has no way of checking if a weak_import symbol is available.
// This function checks if TOOLCHAINRemarkVersion from libRemarks.dylib is available, which should be enough to assert the whole library is available as well.
CSUPPORT_EXPORT bool isLibRemarksAvailable(void);

#endif /* CLibRemarksHelper_h */
