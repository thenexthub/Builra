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

#include "CLibRemarksHelper.h"

#include <stddef.h>
#include <stdint.h>

#if __has_include(<toolchain-c/Remarks.h>) && !defined(SKIP_TOOLCHAIN_REMARKS)
#define HAVE_REMARKS
#endif

// This is hacky: we can't include toolchain-c/Remarks.h because the symbols are not weak_import there, so the compiler can remove the check and return 1.
// Note that the other symbols (which are actually used in OptimizationRemarks.code) are not weak_import, but the library is weak linked, so it won't require them.
// And another (possible) issue with this approach is the fact that we're weak linking libRemarks to SWBCSupport.framework and SWBCore.framework, but the code in SWBCore is not checking the weak_imports itself (it's Swift, so it can't), but instead calling into SWBCSupport (here) to do that.
#ifdef HAVE_REMARKS
extern uint32_t TOOLCHAINRemarkVersion(void) __attribute__((weak_import));
#endif

bool isLibRemarksAvailable(void) {
#ifdef HAVE_REMARKS
    return &TOOLCHAINRemarkVersion != NULL;
#else
    return false;
#endif
}
