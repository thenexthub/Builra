//===-- ActionCache.cpp ---------------------------------------------------===//
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

#include "builra3/ActionCache.h"

using namespace builra3;

ActionCache::~ActionCache() { }

InMemoryActionCache::~InMemoryActionCache() { }

void InMemoryActionCache::get(const CacheKey& key, std::function<void(result<CacheValue, Error>)> resultHandler) {
  CacheValue value;

  {
    std::lock_guard<std::mutex> lock(cacheMutex);
    if (auto entry = cache.find(key.data().bytes()); entry != cache.end()) {
      value = entry->second;
    }
  }

  resultHandler(value);
}

void InMemoryActionCache::update(const CacheKey& key, const CacheValue& value) {
  std::lock_guard<std::mutex> lock(cacheMutex);
  cache.insert_or_assign(key.data().bytes(), value);
}

