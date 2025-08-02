//===-- BuildValue.cpp ----------------------------------------------------===//
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

#include "builra/BuildSystem/BuildValue.h"

#include "builra/Basic/toolchain.h"

#include "toolchain/ADT/Hashing.h"

#include "toolchain/Support/raw_ostream.h"

using namespace builra;
using namespace builra::buildsystem;

StringRef BuildValue::stringForKind(BuildValue::Kind kind) {
  switch (kind) {
#define CASE(kind) case Kind::kind: return #kind
    CASE(Invalid);
    CASE(VirtualInput);
    CASE(ExistingInput);
    CASE(MissingInput);
    CASE(DirectoryContents);
    CASE(FilteredDirectoryContents);
    CASE(DirectoryTreeSignature);
    CASE(DirectoryTreeStructureSignature);
    CASE(MissingOutput);
    CASE(FailedInput);
    CASE(SuccessfulCommand);
    CASE(FailedCommand);
    CASE(PropagatedFailureCommand);
    CASE(CancelledCommand);
    CASE(SkippedCommand);
    CASE(Target);
    CASE(StaleFileRemoval);
    CASE(SuccessfulCommandWithOutputSignature);
#undef CASE
  }
  return "<unknown>";
}
  
void BuildValue::dump(raw_ostream& os) const {
  os << "BuildValue(" << stringForKind(kind);
  if (kindHasSignature()) {
    os << ", signature=" << signature.value;
  }
  if (kindHasOutputInfo()) {
    os << ", outputInfos=[";
    for (unsigned i = 0; i != getNumOutputs(); ++i) {
      auto& info = getNthOutputInfo(i);
      if (i != 0) os << ", ";
      if (info.isMissing()) {
        os << "FileInfo{/*missing*/}";
      } else {
        os << "FileInfo{"
           << "dev=" << info.device
           << ", inode=" << info.inode
           << ", mode=" << info.mode
           << ", size=" << info.size
           << ", modTime=(" << info.modTime.seconds
           << ":" << info.modTime.nanoseconds << "}";
      }
    }
    os << "]";
  }
  if (kindHasStringList()) {
    std::vector<StringRef> values = getStringListValues();
    os << ", values=[";
    for (unsigned i = 0; i != values.size(); ++i) {
      if (i != 0) os << ", ";
      os << '"';
      os.write_escaped(values[i]);
      os << '"';
    }
    os << "]";
  }
  os << ")";
}
