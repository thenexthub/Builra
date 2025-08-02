//===- ActionCache.h --------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_ACTIONCACHE_H
#define BUILRA3_ACTIONCACHE_H

#include <builra3/Result.hpp>

#include "builra3/Error.pb.h"
#include "builra3/ActionCache.pb.h"
#include "builra3/CAS.pb.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace builra3 {

class ActionCache {
public:
  virtual ~ActionCache();

  virtual void get(const CacheKey& key, std::function<void(result<CacheValue, Error>)> resultHandler) = 0;

  virtual void update(const CacheKey& key, const CacheValue& value) = 0;
};

class InMemoryActionCache: public ActionCache {
private:
  std::unordered_map<std::string, CacheValue> cache;
  std::mutex cacheMutex;

public:
  InMemoryActionCache() { }
  ~InMemoryActionCache();

  void get(const CacheKey& key, std::function<void(result<CacheValue, Error>)> resultHandler);

  void update(const CacheKey& key, const CacheValue& value);
};

}

#endif
