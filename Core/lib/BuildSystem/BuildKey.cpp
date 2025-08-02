//===-- BuildKey.cpp ------------------------------------------------------===//
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

#include "builra/BuildSystem/BuildKey.h"

#include "builra/Basic/toolchain.h"
#include "builra/BuildSystem/BuildDescription.h"

#include "toolchain/Support/raw_ostream.h"

using namespace builra;
using namespace builra::buildsystem;

StringRef BuildKey::stringForKind(BuildKey::Kind kind) {
  switch (kind) {
#define CASE(kind) case Kind::kind: return #kind
    CASE(Command);
    CASE(CustomTask);
    CASE(DirectoryContents);
    CASE(FilteredDirectoryContents);
    CASE(DirectoryTreeSignature);
    CASE(DirectoryTreeStructureSignature);
    CASE(Node);
    CASE(Stat);
    CASE(Target);
    CASE(Unknown);
#undef CASE
  }
  return "<unknown>";
}

void BuildKey::dump(raw_ostream& os) const {
  os << "BuildKey(" << stringForKind(getKind());
  switch (getKind()) {
  case Kind::Command: {
    os << ", name='" << getCommandName() << "'";
    break;
  }
  case Kind::CustomTask: {
    os << ", name='" << getCustomTaskName() << "'";
    os << ", dataSize='" << getCustomTaskData().size() << "'";
    break;
  }
  case Kind::DirectoryContents:
  case Kind::DirectoryTreeSignature: {
    os << ", path='" << getDirectoryPath() << "'";
    break;
  }
  case Kind::DirectoryTreeStructureSignature:
  case Kind::FilteredDirectoryContents: {
    os << ", path='" << getFilteredDirectoryPath() << "'";
    // FIXME: should probably dump filters here too
    break;
  }
  case Kind::Node: {
    os << ", name='" << getNodeName() << "'";
    break;
  }
  case Kind::Stat: {
    os << ", name='" << getStatName() << "'";
    break;
  }
  case Kind::Target: {
    os << ", name='" << getTargetName() << "'";
    break;
  }
  case Kind::Unknown: {
    break;
  }
  }
  os << ")";
}
