//===-- BuildFile.cpp -----------------------------------------------------===//
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

#include "builra/BuildSystem/BuildFile.h"
#include "builra/BuildSystem/BuildNode.h"

#include "builra/Basic/FileSystem.h"
#include "builra/Basic/toolchain.h"
#include "builra/BuildSystem/BuildDescription.h"
#include "builra/BuildSystem/Command.h"
#include "builra/BuildSystem/Tool.h"

#include "toolchain/ADT/STLExtras.h"
#include "toolchain/Support/MemoryBuffer.h"
#include "toolchain/Support/SourceMgr.h"
#include "toolchain/Support/YAMLParser.h"
#include "toolchain/Support/Path.h"

#include "builra/Basic/PlatformUtility.h"

#include <algorithm>
#include <vector>

using namespace builra;
using namespace builra::buildsystem;

void ConfigureContext::error(const Twine& message) const {
  delegate.error(filename, at, message);
}

BuildFileDelegate::~BuildFileDelegate() {}

#pragma mark - BuildFile implementation

namespace {

#ifndef NDEBUG
static void dumpNode(toolchain::yaml::Node* node, unsigned indent=0)
    TOOLCHAIN_ATTRIBUTE_USED;
static void dumpNode(toolchain::yaml::Node* node, unsigned indent) {
  switch (node->getType()) {
  default: {
    fprintf(stderr, "%*s<node: %p, unknown>\n", indent*2, "", node);
    break;
  }

  case toolchain::yaml::Node::NK_Null: {
    fprintf(stderr, "%*s(null)\n", indent*2, "");
    break;
  }

  case toolchain::yaml::Node::NK_Scalar: {
    toolchain::yaml::ScalarNode* scalar = toolchain::cast<toolchain::yaml::ScalarNode>(node);
    SmallString<256> storage;
    fprintf(stderr, "%*s(scalar: '%s')\n", indent*2, "",
            scalar->getValue(storage).str().c_str());
    break;
  }

  case toolchain::yaml::Node::NK_KeyValue: {
    assert(0 && "unexpected keyvalue node");
    break;
  }

  case toolchain::yaml::Node::NK_Mapping: {
    toolchain::yaml::MappingNode* map = toolchain::cast<toolchain::yaml::MappingNode>(node);
    fprintf(stderr, "%*smap:\n", indent*2, "");
    for (auto& it: *map) {
      fprintf(stderr, "%*skey:\n", (indent+1)*2, "");
      dumpNode(it.getKey(), indent+2);
      fprintf(stderr, "%*svalue:\n", (indent+1)*2, "");
      dumpNode(it.getValue(), indent+2);
    }
    break;
  }

  case toolchain::yaml::Node::NK_Sequence: {
    toolchain::yaml::SequenceNode* sequence =
      toolchain::cast<toolchain::yaml::SequenceNode>(node);
    fprintf(stderr, "%*ssequence:\n", indent*2, "");
    for (auto& it: *sequence) {
      dumpNode(&it, indent+1);
    }
    break;
  }

  case toolchain::yaml::Node::NK_Alias: {
    fprintf(stderr, "%*s(alias)\n", indent*2, "");
    break;
  }
  }
}
#endif

class OwnershipAnalysis {
  std::map<BuildNode*, Command*> includedPaths;
  std::map<BuildNode*, Command*> excludedPaths;
  BuildFileDelegate& fileDelegate;

public:
  std::vector<std::pair<BuildNode*, Command*>> outputNodesAndCommands;

  std::vector<std::pair<BuildNode*, Command*>> directoryInputNodesAndCommands;

  OwnershipAnalysis(const BuildDescription::command_set& commands, BuildFileDelegate& fileDelegate): fileDelegate(fileDelegate) {
    // Extract outputs and directory inputs of all commands
    for (auto it = commands.begin(); it != commands.end(); it++) {
      Command* command = (*it).getValue().get();
      for (auto output: command->getOutputs()) {
        if (!output->isVirtual()) {
          outputNodesAndCommands.push_back(std::pair<BuildNode*, Command*>(output, command));
        }
      }

      for (auto input: command->getInputs()) {
        if (input->isDirectory()) {
          directoryInputNodesAndCommands.push_back(std::pair<BuildNode*, Command*>(input, command));
        }
      }
    }

    // Sort paths according to length to ensure we assign owner to parent before assigning owner to its subpaths
    std::sort(outputNodesAndCommands.begin(),
              outputNodesAndCommands.end(),
              [](const std::pair<BuildNode*, Command*> pairA,
                 const std::pair<BuildNode*, Command*> pairB) -> bool {
      return pairA.first->getName().str().length() < pairB.first->getName().str().length();
    });
  }

  /// Establish ownerships
  bool establishOwnerships() {
    for (auto outputNodeAndCommand: outputNodesAndCommands) {
        if (outputNodeAndCommand.second->isExternalCommand() && outputNodeAndCommand.second->repairViaOwnershipAnalysis == true) {
          Command *owner = includedOwnerOf(outputNodeAndCommand.first->getName());
          if (owner == nullptr) {
            setOwner(outputNodeAndCommand.first, outputNodeAndCommand.second);
          } else if (owner == outputNodeAndCommand.second) {
            // A path and some of its subpaths are listed as output dependencies of a task.. Do nothing.
          } else {
            std::vector<Command*> conflictingProducers;
            conflictingProducers.push_back(outputNodeAndCommand.second);
            conflictingProducers.push_back(owner);
            fileDelegate.cannotLoadDueToMultipleProducers(outputNodeAndCommand.first, conflictingProducers);
            return false;
          }
        } else {
          setExcludedOwner(outputNodeAndCommand.first, outputNodeAndCommand.second);
        }
    }

    return true;
  }

  /// Check if node is unowned
  const bool isIncludedUnownedNode(const BuildNode* node) {
    return includedOwnerOf(node->getName()) == nullptr && excludedOwnerOf(node->getName()) == nullptr;
  }

  /// Set owner
  void setOwner(BuildNode* node, Command* command) {
    includedPaths[node] = command;
  }
  
  /// Set owner of a path that is produced by a command excluded from ownership analysis so we can distinguish it from an unowned path
  void setExcludedOwner(BuildNode* node, Command* command) {
    excludedPaths[node] = command;
  }

  /// Lookup included owner (a directory prefix of inputPath that is included in the analysis)
  Command* includedOwnerOf(StringRef inputPath) {
    auto it = std::find_if(includedPaths.begin(), includedPaths.end(), [inputPath](const std::pair<BuildNode*, Command*>& buildNodeAndCommand) -> bool {
      if (buildNodeAndCommand.first->getName().endswith("/")) {
        return inputPath.startswith(buildNodeAndCommand.first->getName());
      } else {
        return inputPath.startswith(buildNodeAndCommand.first->getName().str() + "/");
      }
    });

    if (it != includedPaths.end()) {
      return (*it).second;
    } else {
      return nullptr;
    }
  }

  /// Lookup owner
  Command* excludedOwnerOf(StringRef inputPath) {
    auto it = std::find_if(excludedPaths.begin(), excludedPaths.end(), [inputPath](const std::pair<BuildNode*, Command*>& buildNodeAndCommand) -> bool {
      // TODO: a good explanation of why we use "==" as opposed to "startswith"
      return inputPath == buildNodeAndCommand.first->getName();
    });

    if (it != excludedPaths.end()) {
      return (*it).second;
    } else {
      return nullptr;
    }
  }

  // Add input node to additional outputs of its owner
  //
  // [TaskB]
  //  |
  //  v
  // owned-directory/
  //      libX.fake-h
  //  ,-- libY.fake-h (ownership analysis will automatically amend this to outputs of TaskB)
  //  |   libZ.fake-h
  //  v
  // [TaskC]
  //  |
  //  v
  // libY-from-TaskC.fake-h
  //
  // This ensures TaskC will wait until TaskB is finished.
  void amendOutputOfOwnersWithConsumedSubpaths() {
    for (auto directoryInputNodeAndCommand: directoryInputNodesAndCommands) {
      Command *owner = includedOwnerOf(directoryInputNodeAndCommand.first->getName());
      if (owner != nullptr) {
        auto ownerOutputs = owner->getOutputs();
        if (std::find(ownerOutputs.begin(), ownerOutputs.end(), directoryInputNodeAndCommand.first) == ownerOutputs.end()) {
          if (owner->repairViaOwnershipAnalysis) {
            owner->addOutput(directoryInputNodeAndCommand.first);
          }
        }
      }
    }
  }

  //
  // unowned-directory/
  //  |  a.txt <-- TaskA
  //  |  b.txt <-- TaskB
  //  v
  // TaskC
  //
  // We should add "a.txt" and "b.txt" to mustScanAfterPaths of "unowned-directory/".
  // This ensures TaskC will wait until TaskA and TaskB are finished.
  void deferScanningUnownedInputsUntilSubpathsAvailable() {
    auto unownedDirectoryInputNodesAndConsumingCommands = std::vector<std::pair<BuildNode*, Command*>>();
    std::copy_if(directoryInputNodesAndCommands.begin(),
                 directoryInputNodesAndCommands.end(),
                 std::back_inserter(unownedDirectoryInputNodesAndConsumingCommands),
                 [this](const std::pair<BuildNode*, Command*> directoryInputNodeAndCommand) -> bool {
      return isIncludedUnownedNode(directoryInputNodeAndCommand.first) && directoryInputNodeAndCommand.second->repairViaOwnershipAnalysis;
    });

    // For each output node and its producing command (e.g. "unowned-directory/a.txt" and "TaskA"),
    // check if there exists an unowned node (e.g. "unowned-directory/" used by "TaskC") that is a parent of the produced node.
    // Only add "a.txt" to mustScanAfterPaths of "unowned-directory/" if TaskC is marked as "repairViaOwnershipAnalysis".
    for (auto outputNodeAndCommand: outputNodesAndCommands) {
      auto repairableUnownedNode =
        std::find_if(unownedDirectoryInputNodesAndConsumingCommands.begin(),
                     unownedDirectoryInputNodesAndConsumingCommands.end(),
                     [=](std::pair<BuildNode*, Command*> unownedDirectoryAndCommand) -> bool {
          return outputNodeAndCommand.first->getName().startswith(unownedDirectoryAndCommand.first->getName()) && outputNodeAndCommand.second->repairViaOwnershipAnalysis == true;
      });
      
      if (repairableUnownedNode != unownedDirectoryInputNodesAndConsumingCommands.end()) {
        (*repairableUnownedNode).first->mustScanAfterPaths.push_back(outputNodeAndCommand.first->getName());
      }
    }
  }
};

class BuildFileImpl {
  /// The name of the main input file.
  std::string mainFilename;

  /// The build file delegate the BuildFile was configured with.
  BuildFileDelegate& delegate;
  
  /// The set of all registered tools.
  BuildDescription::tool_set tools;

  /// The set of all declared targets.
  BuildDescription::target_set targets;

  /// Default target name
  std::string defaultTarget;

  /// The set of all declared nodes.
  BuildDescription::node_set nodes;

  /// The set of all declared commands.
  BuildDescription::command_set commands;

  /// Indicates if we should perform ownership analysis after we read the build file
  bool performOwnershipAnalysis = false;

  /// The number of parsing errors.
  int numErrors = 0;
    
  // FIXME: Factor out into a parser helper class.
  std::string stringFromScalarNode(toolchain::yaml::ScalarNode* scalar) {
    SmallString<256> storage;
    return scalar->getValue(storage).str();
  }

  /// Emit an error.
  void error(StringRef filename, toolchain::SMRange at,
             StringRef message) {
    BuildFileToken atToken{at.Start.getPointer(),
        unsigned(at.End.getPointer()-at.Start.getPointer())};
    delegate.error(mainFilename, atToken, message);
    ++numErrors;
  }

  void error(StringRef message) {
    error(mainFilename, {}, message);
  }
  
  void error(toolchain::yaml::Node* node, StringRef message) {
    error(mainFilename, node->getSourceRange(), message);
  }

  ConfigureContext getContext(toolchain::SMRange at) {
    BuildFileToken atToken{at.Start.getPointer(),
        unsigned(at.End.getPointer()-at.Start.getPointer())};
    return ConfigureContext{ delegate, mainFilename, atToken };
  }

  ConfigureContext getContext(toolchain::yaml::Node *node) {
    return getContext(node->getSourceRange());
  }

  // FIXME: Factor out into a parser helper class.
  bool nodeIsScalarString(toolchain::yaml::Node* node, StringRef name) {
    if (node->getType() != toolchain::yaml::Node::NK_Scalar)
      return false;

    return stringFromScalarNode(
        static_cast<toolchain::yaml::ScalarNode*>(node)) == name;
  }

  Tool* getOrCreateTool(StringRef name, toolchain::yaml::Node* forNode) {
    // First, check the map.
    auto it = tools.find(name);
    if (it != tools.end())
      return it->second.get();
    
    // Otherwise, ask the delegate to create the tool.
    auto tool = delegate.lookupTool(name);
    if (!tool) {
      error(forNode, "invalid tool (" + name.str() +") type in 'tools' map");
      return nullptr;
    }
    auto result = tool.get();
    tools[name] = std::move(tool);

    return result;
  }

  Node* getOrCreateNode(StringRef name, bool isImplicit) {
    // First, check the map.
    auto it = nodes.find(name);
    if (it != nodes.end())
      return it->second.get();
    
    // Otherwise, ask the delegate to create the node.
    auto node = delegate.createNode(name, isImplicit);
    assert(node);
    auto result = node.get();
    nodes[name] = std::move(node);

    return result;
  }
  
  bool parseRootNode(toolchain::yaml::Node* node) {
    // The root must always be a mapping.
    if (node->getType() != toolchain::yaml::Node::NK_Mapping) {
      error(node, "unexpected top-level node");
      return false;
    }
    auto mapping = static_cast<toolchain::yaml::MappingNode*>(node);

    // Iterate over each of the sections in the mapping.
    auto it = mapping->begin();
    if (!nodeIsScalarString(it->getKey(), "client")) {
      error(it->getKey(), "expected initial mapping key 'client'");
      return false;
    }
    if (it->getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
      error(it->getValue(), "unexpected 'client' value (expected map)");
      return false;
    }

    // Parse the client mapping.
    if (!parseClientMapping(
            static_cast<toolchain::yaml::MappingNode*>(it->getValue()))) {
      return false;
    }
    ++it;

    // Parse the tools mapping, if present.
    if (it != mapping->end() && nodeIsScalarString(it->getKey(), "tools")) {
      if (it->getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(it->getValue(), "unexpected 'tools' value (expected map)");
        return false;
      }

      if (!parseToolsMapping(
              static_cast<toolchain::yaml::MappingNode*>(it->getValue()))) {
        return false;
      }
      ++it;
    }

    // Parse the targets mapping, if present.
    if (it != mapping->end() && nodeIsScalarString(it->getKey(), "targets")) {
      if (it->getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(it->getValue(), "unexpected 'targets' value (expected map)");
        return false;
      }

      if (!parseTargetsMapping(
              static_cast<toolchain::yaml::MappingNode*>(it->getValue()))) {
        return false;
      }
      ++it;
    }

    // Parse the default target, if present.
    if (it != mapping->end() && nodeIsScalarString(it->getKey(), "default")) {
      if (it->getValue()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(it->getValue(), "unexpected 'default' target value (expected scalar)");
        return false;
      }

      if (!parseDefaultTarget(
              static_cast<toolchain::yaml::ScalarNode*>(it->getValue()))) {
        return false;
      }
      ++it;
    }

    // Parse the nodes mapping, if present.
    if (it != mapping->end() && nodeIsScalarString(it->getKey(), "nodes")) {
      if (it->getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(it->getValue(), "unexpected 'nodes' value (expected map)");
        return false;
      }

      if (!parseNodesMapping(
              static_cast<toolchain::yaml::MappingNode*>(it->getValue()))) {
        return false;
      }
      ++it;
    }

    // Parse the commands mapping, if present.
    if (it != mapping->end() && nodeIsScalarString(it->getKey(), "commands")) {
      if (it->getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(it->getValue(), "unexpected 'commands' value (expected map)");
        return false;
      }

      if (!parseCommandsMapping(
              static_cast<toolchain::yaml::MappingNode*>(it->getValue()))) {
        return false;
      }
      ++it;
    }

    // There shouldn't be any trailing sections.
    if (it != mapping->end()) {
      error(&*it, "unexpected trailing top-level section");
      return false;
    }

    return true;
  }

  bool parseClientMapping(toolchain::yaml::MappingNode* map) {
    // Collect all of the keys.
    std::string name;
    uint32_t version = 0;
    property_list_type properties;

    for (auto& entry: *map) {
      // All keys and values must be scalar.
      if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(entry.getKey(), "invalid key type in 'client' map");
        return false;
      }
      if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(entry.getValue(), "invalid value type in 'client' map");
        return false;
      }

      std::string key = stringFromScalarNode(
          static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
      std::string value = stringFromScalarNode(
          static_cast<toolchain::yaml::ScalarNode*>(entry.getValue()));
      if (key == "name") {
        name = value;
      } else if (key == "version") {
        if (StringRef(value).getAsInteger(10, version)) {
          error(entry.getValue(), "invalid version number in 'client' map");
        }
      } if (key == "perform-ownership-analysis") {
        if (value == "yes") {
          performOwnershipAnalysis = true;
        }
      } else {
        properties.push_back({ key, value });
      }
    }

    // Pass to the delegate.
    if (!delegate.configureClient(getContext(map), name, version, properties)) {
      error(map, "unable to configure client");
      return false;
    }

    return true;
  }

  bool parseToolsMapping(toolchain::yaml::MappingNode* map) {
    for (auto& entry: *map) {
      // Every key must be scalar.
      if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(entry.getKey(), "invalid key type in 'tools' map");
        continue;
      }
      // Every value must be a mapping.
      if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(entry.getValue(), "invalid value type in 'tools' map");
        continue;
      }

      std::string name = stringFromScalarNode(
          static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
      toolchain::yaml::MappingNode* attrs = static_cast<toolchain::yaml::MappingNode*>(
          entry.getValue());

      // Get the tool.
      auto tool = getOrCreateTool(name, entry.getKey());
      if (!tool) {
        return false;
      }

      // Configure all of the tool attributes.
      for (auto& valueEntry: *attrs) {
        auto key = valueEntry.getKey();
        auto value = valueEntry.getValue();
        
        // All keys must be scalar.
        if (key->getType() != toolchain::yaml::Node::NK_Scalar) {
          error(key, "invalid key type for tool in 'tools' map");
          continue;
        }


        auto attribute = stringFromScalarNode(
            static_cast<toolchain::yaml::ScalarNode*>(key));

        if (value->getType() == toolchain::yaml::Node::NK_Mapping) {
          std::vector<std::pair<std::string, std::string>> values;
          for (auto& entry: *static_cast<toolchain::yaml::MappingNode*>(value)) {
            // Every key must be scalar.
            if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
              error(entry.getKey(), ("invalid key type for '" + attribute +
                                     "' in 'tools' map"));
              continue;
            }
            // Every value must be scalar.
            if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Scalar) {
              error(entry.getKey(), ("invalid value type for '" + attribute +
                                     "' in 'tools' map"));
              continue;
            }

            std::string key = stringFromScalarNode(
                static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
            std::string value = stringFromScalarNode(
                static_cast<toolchain::yaml::ScalarNode*>(entry.getValue()));
            values.push_back(std::make_pair(key, value));
          }

          if (!tool->configureAttribute(
                  getContext(key), attribute,
                  std::vector<std::pair<StringRef, StringRef>>(
                      values.begin(), values.end()))) {
            return false;
          }
        } else if (value->getType() == toolchain::yaml::Node::NK_Sequence) {
          std::vector<std::string> values;
          for (auto& node: *static_cast<toolchain::yaml::SequenceNode*>(value)) {
            if (node.getType() != toolchain::yaml::Node::NK_Scalar) {
              error(&node, "invalid value type for tool in 'tools' map");
              continue;
            }
            values.push_back(
                stringFromScalarNode(
                    static_cast<toolchain::yaml::ScalarNode*>(&node)));
          }

          if (!tool->configureAttribute(
                  getContext(key), attribute,
                  std::vector<StringRef>(values.begin(), values.end()))) {
            return false;
          }
        } else {
          if (value->getType() != toolchain::yaml::Node::NK_Scalar) {
            error(value, "invalid value type for tool in 'tools' map");
            continue;
          }

          if (!tool->configureAttribute(
                  getContext(key), attribute,
                  stringFromScalarNode(
                      static_cast<toolchain::yaml::ScalarNode*>(value)))) {
            return false;
          }
        }
      }
    }

    return true;
  }
  
  bool parseTargetsMapping(toolchain::yaml::MappingNode* map) {
    for (auto& entry: *map) {
      // Every key must be scalar.
      if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(entry.getKey(), "invalid key type in 'targets' map");
        continue;
      }
      // Every value must be a sequence.
      if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Sequence) {
        error(entry.getValue(), "invalid value type in 'targets' map");
        continue;
      }

      std::string name = stringFromScalarNode(
          static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
      toolchain::yaml::SequenceNode* nodes = static_cast<toolchain::yaml::SequenceNode*>(
          entry.getValue());

      // Create the target.
      auto target = toolchain::make_unique<Target>(name);

      // Add all of the nodes.
      for (auto& node: *nodes) {
        // All items must be scalar.
        if (node.getType() != toolchain::yaml::Node::NK_Scalar) {
          error(&node, "invalid node type in 'targets' map");
          continue;
        }

        target->getNodes().push_back(
            getOrCreateNode(
                stringFromScalarNode(
                    static_cast<toolchain::yaml::ScalarNode*>(&node)),
                /*isImplicit=*/true));
      }

      // Let the delegate know we loaded a target.
      delegate.loadedTarget(name, *target);

      // Add the target to the targets map.
      targets[name] = std::move(target);
    }

    return true;
  }

  bool parseDefaultTarget(toolchain::yaml::ScalarNode* entry) {
    std::string target = stringFromScalarNode(entry);

    if (targets.find(target) == targets.end()) {
      error(entry, "invalid default target, a default target should be in targets");
      return false;
    }

    defaultTarget = target;
    delegate.loadedDefaultTarget(defaultTarget);

    return true;
  }

  bool parseNodesMapping(toolchain::yaml::MappingNode* map) {
    for (auto& entry: *map) {
      // Every key must be scalar.
      if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(entry.getKey(), "invalid key type in 'nodes' map");
        continue;
      }
      // Every value must be a mapping.
      if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(entry.getValue(), "invalid value type in 'nodes' map");
        continue;
      }

      std::string name = stringFromScalarNode(
          static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
      toolchain::yaml::MappingNode* attrs = static_cast<toolchain::yaml::MappingNode*>(
          entry.getValue());

      // Get the node.
      //
      // FIXME: One downside of doing the lookup here is that the client cannot
      // ever make a context dependent node that can have configured properties.
      auto node = getOrCreateNode(name, /*isImplicit=*/false);

      // Configure all of the tool attributes.
      for (auto& valueEntry: *attrs) {
        auto key = valueEntry.getKey();
        auto value = valueEntry.getValue();
        
        // All keys must be scalar.
        if (key->getType() != toolchain::yaml::Node::NK_Scalar) {
          error(key, "invalid key type for node in 'nodes' map");
          continue;
        }

        auto attribute = stringFromScalarNode(
            static_cast<toolchain::yaml::ScalarNode*>(key));

        if (value->getType() == toolchain::yaml::Node::NK_Mapping) {
          std::vector<std::pair<std::string, std::string>> values;
          for (auto& entry: *static_cast<toolchain::yaml::MappingNode*>(value)) {
            // Every key must be scalar.
            if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
              error(entry.getKey(), ("invalid key type for '" + attribute +
                                     "' in 'nodes' map"));
              continue;
            }
            // Every value must be scalar.
            if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Scalar) {
              error(entry.getKey(), ("invalid value type for '" + attribute +
                                     "' in 'nodes' map"));
              continue;
            }

            std::string key = stringFromScalarNode(
                static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
            std::string value = stringFromScalarNode(
                static_cast<toolchain::yaml::ScalarNode*>(entry.getValue()));
            values.push_back(std::make_pair(key, value));
          }

          if (!node->configureAttribute(
                  getContext(key), attribute,
                  std::vector<std::pair<StringRef, StringRef>>(
                      values.begin(), values.end()))) {
            return false;
          }
        } else if (value->getType() == toolchain::yaml::Node::NK_Sequence) {
          std::vector<std::string> values;
          for (auto& node: *static_cast<toolchain::yaml::SequenceNode*>(value)) {
            if (node.getType() != toolchain::yaml::Node::NK_Scalar) {
              error(&node, "invalid value type for node in 'nodes' map");
              continue;
            }
            values.push_back(
                stringFromScalarNode(
                    static_cast<toolchain::yaml::ScalarNode*>(&node)));
          }

          if (!node->configureAttribute(
                  getContext(key), attribute,
                  std::vector<StringRef>(values.begin(), values.end()))) {
            return false;
          }
        } else {
          if (value->getType() != toolchain::yaml::Node::NK_Scalar) {
            error(value, "invalid value type for node in 'nodes' map");
            continue;
          }
        
          if (!node->configureAttribute(
                  getContext(key), attribute,
                  stringFromScalarNode(
                      static_cast<toolchain::yaml::ScalarNode*>(value)))) {
            return false;
          }
        }
      }
    }

    return true;
  }

  bool parseCommandsMapping(toolchain::yaml::MappingNode* map) {
    for (auto& entry: *map) {
      // Every key must be scalar.
      if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(entry.getKey(), "invalid key type in 'commands' map");
        continue;
      }
      // Every value must be a mapping.
      if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Mapping) {
        error(entry.getValue(), "invalid value type in 'commands' map");
        continue;
      }

      std::string name = stringFromScalarNode(
          static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
      toolchain::yaml::MappingNode* attrs = static_cast<toolchain::yaml::MappingNode*>(
          entry.getValue());

      // Check that the command is not a duplicate.
      if (commands.count(name) != 0) {
        error(entry.getKey(), "duplicate command in 'commands' map");
        continue;
      }
      
      // Get the initial attribute, which must be the tool name.
      auto it = attrs->begin();
      if (it == attrs->end()) {
        error(entry.getKey(),
              "missing 'tool' key for command in 'command' map");
        continue;
      }
      if (!nodeIsScalarString(it->getKey(), "tool")) {
        error(it->getKey(),
              "expected 'tool' initial key for command in 'commands' map");
        // Skip to the end.
        while (it != attrs->end()) ++it;
        continue;
      }
      if (it->getValue()->getType() != toolchain::yaml::Node::NK_Scalar) {
        error(it->getValue(),
              "invalid 'tool' value type for command in 'commands' map");
        // Skip to the end.
        while (it != attrs->end()) ++it;
        continue;
      }
      
      // Lookup the tool for this command.
      auto tool = getOrCreateTool(
          stringFromScalarNode(
              static_cast<toolchain::yaml::ScalarNode*>(
                  it->getValue())),
          it->getValue());
      if (!tool) {
        return false;
      }
        
      // Create the command.
      auto command = tool->createCommand(name);
      if (!command) {
        error(it->getValue(), "tool failed to create a command");
        return false;
      }

      // Parse the remaining command attributes.
      ++it;
      for (; it != attrs->end(); ++it) {
        auto key = it->getKey();
        auto value = it->getValue();
        
        // If this is a known key, parse it.
        if (nodeIsScalarString(key, "inputs")) {
          if (value->getType() != toolchain::yaml::Node::NK_Sequence) {
            error(value, "invalid value type for 'inputs' command key");
            continue;
          }

          toolchain::yaml::SequenceNode* nodeNames =
            static_cast<toolchain::yaml::SequenceNode*>(value);

          std::vector<Node*> nodes;
          for (auto& nodeName: *nodeNames) {
            if (nodeName.getType() != toolchain::yaml::Node::NK_Scalar) {
              error(&nodeName, "invalid node type in 'inputs' command key");
              continue;
            }

            nodes.push_back(
                getOrCreateNode(
                    stringFromScalarNode(
                        static_cast<toolchain::yaml::ScalarNode*>(&nodeName)),
                    /*isImplicit=*/true));
          }

          command->configureInputs(getContext(key), nodes);
        } else if (nodeIsScalarString(key, "outputs")) {
          if (value->getType() != toolchain::yaml::Node::NK_Sequence) {
            error(value, "invalid value type for 'outputs' command key");
            continue;
          }

          toolchain::yaml::SequenceNode* nodeNames =
            static_cast<toolchain::yaml::SequenceNode*>(value);

          std::vector<Node*> nodes;
          for (auto& nodeName: *nodeNames) {
            if (nodeName.getType() != toolchain::yaml::Node::NK_Scalar) {
              error(&nodeName, "invalid node type in 'outputs' command key");
              continue;
            }

            auto node = getOrCreateNode(
                    stringFromScalarNode(
                        static_cast<toolchain::yaml::ScalarNode*>(&nodeName)),
                    /*isImplicit=*/true);
            nodes.push_back(node);

            // Add this command to the node producer list.
            node->getProducers().push_back(command.get());
          }

          command->configureOutputs(getContext(key), nodes);
        } else if (nodeIsScalarString(key, "description")) {
          if (value->getType() != toolchain::yaml::Node::NK_Scalar) {
            error(value, "invalid value type for 'description' command key");
            continue;
          }

          command->configureDescription(
              getContext(key), stringFromScalarNode(
                  static_cast<toolchain::yaml::ScalarNode*>(value)));
        } else {
          // Otherwise, it should be an attribute assignment.
          
          // All keys must be scalar.
          if (key->getType() != toolchain::yaml::Node::NK_Scalar) {
            error(key, "invalid key type in 'commands' map");
            continue;
          }

          auto attribute = stringFromScalarNode(
              static_cast<toolchain::yaml::ScalarNode*>(key));

          if (value->getType() == toolchain::yaml::Node::NK_Mapping) {
            std::vector<std::pair<std::string, std::string>> values;
            for (auto& entry: *static_cast<toolchain::yaml::MappingNode*>(value)) {
              // Every key must be scalar.
              if (entry.getKey()->getType() != toolchain::yaml::Node::NK_Scalar) {
                error(entry.getKey(), ("invalid key type for '" + attribute +
                                       "' in 'commands' map"));
                continue;
              }
              // Every value must be scalar.
              if (entry.getValue()->getType() != toolchain::yaml::Node::NK_Scalar) {
                error(entry.getKey(), ("invalid value type for '" + attribute +
                                       "' in 'commands' map"));
                continue;
              }

              std::string key = stringFromScalarNode(
                  static_cast<toolchain::yaml::ScalarNode*>(entry.getKey()));
              std::string value = stringFromScalarNode(
                  static_cast<toolchain::yaml::ScalarNode*>(entry.getValue()));
              values.push_back(std::make_pair(key, value));
            }

            if (!command->configureAttribute(
                    getContext(key), attribute,
                    std::vector<std::pair<StringRef, StringRef>>(
                        values.begin(), values.end()))) {
              return false;
            }
          } else if (value->getType() == toolchain::yaml::Node::NK_Sequence) {
            std::vector<std::string> values;
            for (auto& node: *static_cast<toolchain::yaml::SequenceNode*>(value)) {
              if (node.getType() != toolchain::yaml::Node::NK_Scalar) {
                error(&node, "invalid value type for command in 'commands' map");
                continue;
              }
              values.push_back(
                  stringFromScalarNode(
                      static_cast<toolchain::yaml::ScalarNode*>(&node)));
            }

            if (!command->configureAttribute(
                    getContext(key), attribute,
                    std::vector<StringRef>(values.begin(), values.end()))) {
              return false;
            }
          } else {
            if (value->getType() != toolchain::yaml::Node::NK_Scalar) {
              error(value, "invalid value type for command in 'commands' map");
              continue;
            }
            
            if (!command->configureAttribute(
                    getContext(key), attribute,
                    stringFromScalarNode(
                        static_cast<toolchain::yaml::ScalarNode*>(value)))) {
              return false;
            }
          }
        }
      }

      // Let the delegate know we loaded a command.
      delegate.loadedCommand(name, *command);

      // Add the command to the commands map.
      commands[name] = std::move(command);
    }

    return true;
  }

public:
  BuildFileImpl(class BuildFile& buildFile,
                StringRef mainFilename,
                BuildFileDelegate& delegate)
    : mainFilename(mainFilename), delegate(delegate) {}

  BuildFileDelegate* getDelegate() {
    return &delegate;
  }

  /// @name Parse Actions
  /// @{

  std::unique_ptr<BuildDescription> load() {
    // Create a memory buffer for the input.
    //
    // FIXME: Lift the file access into the delegate, like we do for Ninja.
    toolchain::SourceMgr sourceMgr;
    auto input = delegate.getFileSystem().getFileContents(mainFilename);
    if (!input) {
      error("unable to open '" + mainFilename + "'");
      return nullptr;
    }

    delegate.setFileContentsBeingParsed(input->getBuffer());

    // Create a YAML parser.
    toolchain::yaml::Stream stream(input->getMemBufferRef(), sourceMgr);

    // Read the stream, we only expect a single document.
    auto it = stream.begin();
    if (it == stream.end()) {
      error("missing document in stream");
      return nullptr;
    }

    auto& document = *it;
    auto root = document.getRoot();
    if (!root) {
      error("missing document in stream");
      return nullptr;
    }

    if (!parseRootNode(root)) {
      return nullptr;
    }

    if (++it != stream.end()) {
      error(it->getRoot(), "unexpected additional document in stream");
      return nullptr;
    }

    if (performOwnershipAnalysis) {
      OwnershipAnalysis ownershipAnalysis = OwnershipAnalysis(commands, delegate);
      if (ownershipAnalysis.establishOwnerships()) {
        ownershipAnalysis.amendOutputOfOwnersWithConsumedSubpaths();
        ownershipAnalysis.deferScanningUnownedInputsUntilSubpathsAvailable();
      } else {
        return nullptr;
      }
    }

    // Create the actual description from our constructed elements.
    //
    // FIXME: This is historical, We should tidy up this class to reflect that
    // it is now just a builder.
    auto description = toolchain::make_unique<BuildDescription>();
    std::swap(description->getNodes(), nodes);
    std::swap(description->getTargets(), targets);
    std::swap(description->getDefaultTarget(), defaultTarget);
    std::swap(description->getCommands(), commands);
    std::swap(description->getTools(), tools);
    return description;
  }
};

}

#pragma mark - BuildFile

BuildFile::BuildFile(StringRef mainFilename,
                     BuildFileDelegate& delegate)
  : impl(new BuildFileImpl(*this, mainFilename, delegate))
{
}

BuildFile::~BuildFile() {
  delete static_cast<BuildFileImpl*>(impl);
}

std::unique_ptr<BuildDescription> BuildFile::load() {
  // Create the build description.
  return static_cast<BuildFileImpl*>(impl)->load();
}
