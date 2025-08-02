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

#ifndef BuildKey_C_API_Private_h
#define BuildKey_C_API_Private_h

#include "builra/BuildSystem/BuildKey.h"
#include "builra/Core/BuildEngine.h"

namespace {
using namespace builra::buildsystem;

/// This class is used as a context pointer in the client
class CAPIBuildKey {
  
private:
  builra::core::KeyID identifier;
  bool hasIdentifier;
  BuildKey internalBuildKey;
  size_t hashValue;
public:
  
  CAPIBuildKey(const BuildKey &buildKey): hasIdentifier(false), internalBuildKey(buildKey) {
    hashValue = std::hash<builra::core::KeyType>{}(internalBuildKey.getKeyData());
  }
  
  CAPIBuildKey(const BuildKey &buildKey, builra::core::KeyID identifier): identifier(identifier), hasIdentifier(true), internalBuildKey(buildKey) {
    hashValue = std::hash<builra::core::KeyType>{}(internalBuildKey.getKeyData());
  }
  
  BuildKey &getInternalBuildKey() {
    return internalBuildKey;
  }
  size_t getHashValue() {
    return hashValue;
  }
  llb_build_key_kind_t getKind();
  
  bool operator ==(const CAPIBuildKey &b);
};
}

namespace std {
template <>
struct hash<CAPIBuildKey> {
  size_t operator()(CAPIBuildKey& key) const;
};
}

#endif /* BuildKey_C_API_Private_h */
