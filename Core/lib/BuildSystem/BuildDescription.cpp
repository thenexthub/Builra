//===-- BuildDescription.cpp ----------------------------------------------===//
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

#include "builra/BuildSystem/BuildDescription.h"

#include "builra/BuildSystem/Command.h"
#include "builra/BuildSystem/Tool.h"

using namespace builra;
using namespace builra::buildsystem;

Node::~Node() {}

Command::~Command() {}

basic::CommandSignature Command::getSignature() const {
  return basic::CommandSignature().combine(name);
}


Tool::~Tool() {}

std::unique_ptr<Command> Tool::createCustomCommand(const BuildKey& key) {
  return nullptr;
}

Node& BuildDescription::addNode(std::unique_ptr<Node> value) {
  auto& result = *value.get();
  getNodes()[value->getName()] = std::move(value);
  return result;
}

Target& BuildDescription::addTarget(std::unique_ptr<Target> value) {
  auto& result = *value.get();
  getTargets()[value->getName()] = std::move(value);
  return result;
}

Command& BuildDescription::addCommand(std::unique_ptr<Command> value) {
  auto& result = *value.get();
  getCommands()[value->getName()] = std::move(value);
  return result;
}

Tool& BuildDescription::addTool(std::unique_ptr<Tool> value) {
  auto& result = *value.get();
  getTools()[value->getName()] = std::move(value);
  return result;
}
