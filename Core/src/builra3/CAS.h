//===- CAS.h ----------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_CAS_H
#define BUILRA3_CAS_H

#include <builra3/Result.hpp>

#include "builra3/Error.pb.h"
#include "builra3/CAS.pb.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace builra3 {

class CASDatabase {
public:
  virtual ~CASDatabase();

  virtual void contains(const CASID& casid, std::function<void(result<bool, Error>)> resultHandler) = 0;

  virtual void get(const CASID& casid, std::function<void(result<CASObject, Error>)> resultHandler) = 0;

  virtual void put(const CASObject& object, std::function<void(result<CASID, Error>)> resultHandler) = 0;

  virtual CASID identify(const CASObject& object) = 0;

  virtual void* __raw_context();
};

class InMemoryCASDatabase: public CASDatabase {
private:
  std::unordered_map<std::string, CASObject> db;
  std::mutex dbMutex;

public:
  InMemoryCASDatabase() { }
  ~InMemoryCASDatabase();

  void contains(const CASID& casid, std::function<void(result<bool, Error>)> resultHandler);

  void get(const CASID& casid, std::function<void(result<CASObject, Error>)> resultHandler);

  void put(const CASObject& object, std::function<void(result<CASID, Error>)> resultHandler);

  CASID identify(const CASObject& object);
};

std::string CASIDAsCanonicalString(const CASID& objID);
void ParseCanonicalCASIDString(CASID& objID, const std::string& str);

}

#endif
