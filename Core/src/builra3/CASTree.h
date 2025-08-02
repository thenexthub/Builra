//===- CASTree.h ------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_CASTREE_H
#define BUILRA3_CASTREE_H

#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <builra3/Result.hpp>

#include "builra3/CAS.h"
#include "builra3/CASTree.pb.h"
#include "builra3/Error.pb.h"


namespace builra3 {

struct NamedDirectoryEntryID {
  NamedDirectoryEntry info;
  CASID id;
};

class CASTree {
private:
  std::shared_ptr<CASDatabase> db;

  CASObject object;
  FileInformation fileInfo;
  CASID id;

public:
  CASTree(const std::vector<NamedDirectoryEntryID>& entries,
          std::shared_ptr<CASDatabase> db);

  inline const FileInformation& info() const { return fileInfo; }
  inline const CASID& treeID() const { return id; }

  void sync(std::function<void (result<CASID, Error>)>);
};


}

#endif
