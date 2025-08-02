//===- MockBuildSystemDelegate.cpp ----------------------------------------===//
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
//===----------------------------------------------------------------------===//

#include "MockBuildSystemDelegate.h"

using namespace toolchain;
using namespace builra;
using namespace builra::buildsystem;
using namespace builra::unittests;

MockExecutionQueueDelegate::MockExecutionQueueDelegate() {}

MockBuildSystemDelegate::MockBuildSystemDelegate(bool trackAllMessages)
    : BuildSystemDelegate("mock", 0), trackAllMessages(trackAllMessages)
{
}

    
