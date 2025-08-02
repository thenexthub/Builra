//===-- BuildKey-C-API-Private.h ------------------------------------------===//
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

#ifndef BuildValue_C_API_Private_h
#define BuildValue_C_API_Private_h

#include "builra/BuildSystem/BuildValue.h"

namespace {
using namespace builra::buildsystem;

/// This class is used as a context pointer in the client
class CAPIBuildValue {

private:
  const BuildValue internalBuildValue;
public:

  CAPIBuildValue(BuildValue buildValue): internalBuildValue(std::move(buildValue)) {}

  const BuildValue &getInternalBuildValue() {
    return internalBuildValue;
  }

  llb_build_value_kind_t getKind();
};
}

namespace builra {
namespace capi {

const llb_build_value_file_info_t convertFileInfo(const builra::basic::FileInfo &fileInfo);

}
}

#endif /* BuildValue_C_API_Private_h */
