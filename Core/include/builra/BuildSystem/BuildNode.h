//===- BuildNode.h ----------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA_BUILDSYSTEM_BUILDNODE_H
#define BUILRA_BUILDSYSTEM_BUILDNODE_H

#include "BuildDescription.h"

#include "builra/Basic/toolchain.h"
#include "builra/Basic/StringList.h"
#include "builra/BuildSystem/BuildFile.h"

#include "toolchain/ADT/StringRef.h"

namespace builra {
namespace basic {
  
struct FileInfo;
class FileSystem;

}
  
namespace buildsystem {
  
// FIXME: Figure out how this is going to be organized.
class BuildNode : public Node {

  enum class NodeType : unsigned char {
    Plain = 0,
    Directory = 1,
    DirectoryStructure = 2,
    Virtual = 3,
  };

  NodeType type = NodeType::Plain;

  /// Whether this node represents a "command timestamp".
  ///
  /// Such nodes should always also be virtual.
  bool commandTimestamp = false;

  /// Whether this node is mutated by the build.
  ///
  /// This flag cannot currently be honored to provide a strongly consistent
  /// build, but it is used to detect when the file system information on a node
  /// cannot be safely used to track *output* file state.
  bool mutated = false;

  /// Exclusion filters for directory listings
  ///
  /// Items matching these filter strings are not considered as part of the
  /// signature for directory and directory structure nodes.
  basic::StringList exclusionPatterns;

  explicit BuildNode(StringRef name, NodeType type)
      : Node(name), type(type) {}

public:
  /// Check whether this is a "virtual" (non-filesystem related) node.
  bool isVirtual() const { return (type == NodeType::Virtual); }

  /// Check whether this node is intended to represent a directory's contents
  /// recursively.
  bool isDirectory() const { return (type == NodeType::Directory); }

  /// Check whether this node is intended to represent a directory's structure
  /// recursively.
  bool isDirectoryStructure() const { return (type == NodeType::DirectoryStructure); }

  bool isCommandTimestamp() const { return commandTimestamp; }

  bool isMutated() const { return mutated; }

  const basic::StringList& contentExclusionPatterns() const {
    return exclusionPatterns;
  }

  std::vector<StringRef> mustScanAfterPaths;

  const std::vector<StringRef>& getMustScanAfterPaths() const {
    return mustScanAfterPaths;
  }

  virtual bool configureAttribute(const ConfigureContext& ctx, StringRef name,
                                  StringRef value) override;
  virtual bool configureAttribute(const ConfigureContext& ctx, StringRef name,
                                  ArrayRef<StringRef> values) override;
  virtual bool configureAttribute(
      const ConfigureContext& ctx, StringRef name,
      ArrayRef<std::pair<StringRef, StringRef>> values) override;

  basic::FileInfo getFileInfo(basic::FileSystem&) const;
  basic::FileInfo getLinkInfo(basic::FileSystem&) const;

  basic::CommandSignature getSignature() const;

  static std::unique_ptr<BuildNode> makePlain(StringRef name) {
    return std::unique_ptr<BuildNode>(new BuildNode(name, NodeType::Plain));
  }

  static std::unique_ptr<BuildNode> makeDirectory(StringRef name) {
    return std::unique_ptr<BuildNode>(new BuildNode(name, NodeType::Directory));
  }

  static std::unique_ptr<BuildNode> makeVirtual(StringRef name) {
    return std::unique_ptr<BuildNode>(new BuildNode(name, NodeType::Virtual));
  }
};


class StatNode {
  std::string name;

public:
  explicit StatNode(StringRef name) : name(name) {}

  const std::string& getName() { return name; }

  basic::FileInfo getFileInfo(basic::FileSystem&) const;
  basic::FileInfo getLinkInfo(basic::FileSystem&) const;
};


}
}

#endif
