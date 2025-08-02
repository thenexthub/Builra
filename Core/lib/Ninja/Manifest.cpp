//===-- Manifest.cpp ------------------------------------------------------===//
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

#include "builra/Ninja/Manifest.h"

#include "builra/Basic/toolchain.h"

#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/Path.h"

using namespace builra;
using namespace builra::ninja;

bool Rule::isValidParameterName(StringRef name) {
  return name == "command" ||
    name == "description" ||
    name == "deps" ||
    name == "depfile" ||
    name == "generator" ||
    name == "pool" ||
    name == "restat" ||
    name == "rspfile" ||
    name == "rspfile_content";
}

Manifest::Manifest() {
  // Create the built-in console pool, and add it to the pool map.
  consolePool = new (getAllocator()) Pool("console");
  assert(consolePool != nullptr);
  if (consolePool)
    consolePool->setDepth(1);
  pools["console"] = consolePool;

  // Create the built-in phony rule, and add it to the rule map.
  phonyRule = new (getAllocator()) Rule("phony");
  getRootScope().getRules()["phony"] = phonyRule;
}

bool Manifest::normalize_path(StringRef workingDirectory, SmallVectorImpl<char>& tmp){
  auto separatorRef = toolchain::sys::path::get_separator();
  assert(separatorRef.size() == 1);
  char slash = *separatorRef.data();

  if (toolchain::sys::fs::make_absolute(workingDirectory, tmp) != std::error_code()) {
    return false;
  }
  if (tmp.size() == 0 || !toolchain::sys::path::is_absolute(tmp)) {
      return false;
  }

  // Ignore root "network" prefixes, such as "//net/".
  StringRef tmpRef{tmp.data(), tmp.size()};
  auto rootNameLength = toolchain::sys::path::root_name(tmpRef).size();

  auto begin = tmp.begin() + rootNameLength;
  auto src_it = begin;
  auto dst_it = begin;
  auto end = tmp.end();
  for (; src_it < end; ++src_it) {
    if (TOOLCHAIN_LIKELY(*src_it != slash)) {
        *dst_it++ = *src_it;
        continue;
    }

    if (dst_it == begin || *(dst_it-1) != slash) {
      *dst_it++ = slash;
    }

    // Current fragment "/"
    if (TOOLCHAIN_UNLIKELY(src_it + 1 >= end)) {
      break;
    }

    if (TOOLCHAIN_LIKELY(*(src_it + 1) != '.')) {
      continue;
    }

    // Current fragment "/."
    if (src_it + 2 >= end) {
      ++src_it;
      continue;
    }

    if (*(src_it + 2) != '.') {
      if (*(src_it + 2) == slash) {
        // Means "/./"
        ++src_it;
      } else {
        // "/.?"
        *dst_it++ = *++src_it;
      }
      continue;
    }

    if (src_it + 3 < end && *(src_it + 3) != slash) {
      continue;
    }

    // Move destination pointer to the previous directory.
    if (dst_it - begin <= 1) {
      // Already at the top.
    } else {
      for (dst_it -= 2; dst_it > begin; --dst_it) {
        if (TOOLCHAIN_UNLIKELY(*dst_it == slash)) {
          ++dst_it;
          break;
        }
      }
      if (dst_it == begin) {
        *dst_it++ = slash;
      }
    }

    if (src_it + 3 < end) {
      src_it += 2;
    } else {
      break;
    }
  }

  tmp.resize(dst_it - tmp.begin());

  return true;
}

Node* Manifest::findNode(StringRef workingDirectory, StringRef path0) {
  SmallString<256> absPathTmp = path0;
  if (!normalize_path(workingDirectory, absPathTmp)) {
    return nullptr;
  }

  auto it = nodes.find(absPathTmp);
  if (it == nodes.end()) {
    return nullptr;
  }
  return it->second;
}

Node* Manifest::findOrCreateNode(StringRef workingDirectory, StringRef path0) {
  SmallString<256> absPathTmp = path0;
  if (!normalize_path(workingDirectory, absPathTmp)) {
    return nullptr;
  }

  StringRef path = absPathTmp;

  auto& result = nodes[path];
  if (!result)
    result = new (getAllocator()) Node(path, path0);
  return result;
}
