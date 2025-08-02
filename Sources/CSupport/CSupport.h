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

// This framework defines functionality for Swift Build which is not presently written in Swift for some reason.

#ifndef SWBCSUPPORT_H
#define SWBCSUPPORT_H

#if __has_include(<TargetConditionals.h>)
#include <TargetConditionals.h>
#endif

#include "CLibclang.h"
#include "CLibRemarksHelper.h"
#include "IndexStore.h"
#include "PluginAPI.h"
#include "PluginAPI_functions.h"
#include "PluginAPI_types.h"

#endif // SWBCSUPPORT_H
