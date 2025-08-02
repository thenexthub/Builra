//===-- BuildNode.cpp -----------------------------------------------------===//
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

#include "builra/BuildSystem/BuildNode.h"

#include "builra/Basic/FileInfo.h"
#include "builra/Basic/FileSystem.h"
#include "builra/BuildSystem/BuildFile.h"
#include "builra/BuildSystem/Command.h"

#include "toolchain/ADT/ArrayRef.h"
#include "toolchain/ADT/Twine.h"

using namespace builra;
using namespace builra::basic;
using namespace builra::buildsystem;

bool BuildNode::configureAttribute(const ConfigureContext& ctx, StringRef name,
                                   StringRef value) {
  if (name == "type") {
    if (value == "plain") {
      type = NodeType::Plain;
    } else if (value == "directory") {
      type = NodeType::Plain;
    } else if (value == "directory-structure") {
      type = NodeType::DirectoryStructure;
    } else if (value == "virtual") {
      type = NodeType::Virtual;
    } else {
      ctx.error("invalid value: '" + value + "' for attribute '"
                + name + "'");
      return false;
    }
    return true;
  } else if (name == "is-directory") { // Note: Deprecated in favor of 'type'.
    if (value == "true") {
      type = NodeType::Directory;
    } else if (value == "false") {
      if (type == NodeType::Directory)
        type = NodeType::Plain;
    } else {
      ctx.error("invalid value: '" + value + "' for attribute '"
                + name + "'");
      return false;
    }
    return true;
  } else if (name == "is-directory-structure") { // Note: Deprecated in favor of 'type'.
    if (value == "true") {
      type = NodeType::DirectoryStructure;
    } else if (value == "false") {
      if (type == NodeType::DirectoryStructure)
        type = NodeType::Plain;
    } else {
      ctx.error("invalid value: '" + value + "' for attribute '"
                + name + "'");
      return false;
    }
    return true;
  } else if (name == "is-virtual") { // Note: Deprecated in favor of 'type'.
    if (value == "true") {
      type = NodeType::Virtual;
    } else if (value == "false") {
      if (type == NodeType::Virtual)
        type = NodeType::Plain;
      commandTimestamp = false;
    } else {
      ctx.error("invalid value: '" + value + "' for attribute '"
                + name + "'");
      return false;
    }
    return true;
  } else if (name == "is-command-timestamp") {
    if (value == "true") {
      commandTimestamp = true;
      type = NodeType::Virtual;
    } else if (value == "false") {
      commandTimestamp = false;
    } else {
      ctx.error("invalid value: '" + value + "' for attribute '"
                + name + "'");
      return false;
    }
    return true;
  } else if (name == "is-mutated") {
    if (value == "true") {
      mutated = true;
    } else if (value == "false") {
      mutated = false;
    } else {
      ctx.error("invalid value: '" + value + "' for attribute '"
                + name + "'");
      return false;
    }
    return true;
  } else if (name == "content-exclusion-patterns") {
    exclusionPatterns = basic::StringList(value);
    return true;
  }
    
  // We don't support any other custom attributes.
  ctx.error("unexpected attribute: '" + name + "'");
  return false;
}

bool BuildNode::configureAttribute(const ConfigureContext& ctx, StringRef name,
                                   ArrayRef<StringRef> values) {
  if (name == "content-exclusion-patterns") {
    exclusionPatterns = basic::StringList(values);
    return true;
  } else if (name == "must-scan-after-paths") {
    mustScanAfterPaths = basic::StringList(values).getValues();
    return true;
  }

  // We don't support any other custom attributes.
  ctx.error("unexpected attribute: '" + name + "'");
  return false;
}

bool BuildNode::configureAttribute(
    const ConfigureContext& ctx, StringRef name,
    ArrayRef<std::pair<StringRef, StringRef>> values) {
  // We don't support any other custom attributes.
  ctx.error("unexpected attribute: '" + name + "'");
  return false;
}

FileInfo BuildNode::getFileInfo(basic::FileSystem& fileSystem) const {
  assert(!isVirtual());

  // Drop the trailing slash
  // otherwise non-directory paths that end with "/" will be reported as missing
  StringRef path = getName();
  if (path.endswith("/") && path != "/") {
    path = path.substr(0, path.size() - 1);
  }

  return fileSystem.getFileInfo(path);
}

FileInfo BuildNode::getLinkInfo(basic::FileSystem& fileSystem) const {
  assert(!isVirtual());
  return fileSystem.getLinkInfo(getName());
}

basic::CommandSignature BuildNode::getSignature() const {
  basic::CommandSignature sig;
  sig.combine(static_cast<unsigned int>(type));
  // We include the name of all producer rules in the signature to ensure that
  // we properly pick up changes in build graph structure.  For example, a node
  // that was previously a plain input that has changed to become a produced
  // node.
  for (auto* producer : getProducers()) {
    sig.combine(producer->getName());
  }
  return sig;
}


FileInfo StatNode::getFileInfo(basic::FileSystem& fileSystem) const {
  return fileSystem.getFileInfo(name);
}

FileInfo StatNode::getLinkInfo(basic::FileSystem& fileSystem) const {
  return fileSystem.getLinkInfo(name);
}
