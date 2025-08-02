//===- toolchain/Support/Options.cpp - Debug options support ---------*- C++ -*-===//
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
//
// This file implements the helper objects for defining debug options using the
// new API built on cl::opt, but not requiring the use of static globals.
//
//===----------------------------------------------------------------------===//

#include "toolchain/Support/Options.h"
#include "toolchain/Support/ManagedStatic.h"

using namespace toolchain;

OptionRegistry::~OptionRegistry() {
  for (auto IT = Options.begin(); IT != Options.end(); ++IT)
    delete IT->second;
}

void OptionRegistry::addOption(void *Key, cl::Option *O) {
  assert(Options.find(Key) == Options.end() &&
         "Argument with this key already registerd");
  Options.insert(std::make_pair(Key, O));
}

static ManagedStatic<OptionRegistry> OR;

OptionRegistry &OptionRegistry::instance() { return *OR; }
