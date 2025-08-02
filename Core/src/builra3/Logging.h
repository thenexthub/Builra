//===- Logging.h ------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_LOGGING_H
#define BUILRA3_LOGGING_H

#include <memory>
#include <optional>
#include <string>

#include "builra3/Common.h"
#include "builra3/Common.pb.h"
#include "builra3/Error.pb.h"


namespace builra3 {

class ClientContext {
private:
  void* ctx;
  std::function<void (void*)> releaseFn;

public:
  ClientContext(void* ctx, std::function<void (void*)> releaseFn)
    : ctx(ctx), releaseFn(releaseFn) { }
  ~ClientContext() {
    if (ctx && releaseFn) releaseFn(ctx);
  }

  void* get() const { return ctx; }
};

struct LoggingContext {
  std::optional<EngineID> engine;
  std::shared_ptr<ClientContext> clientContext;
};

class Logger {
public:
  virtual ~Logger() = 0;

  virtual void error(LoggingContext, Error) = 0;
  virtual void event(LoggingContext, const std::vector<Stat>&) = 0;
};

class NullLogger: public Logger {
public:
  NullLogger() { }
  ~NullLogger() { }

  void error(LoggingContext, Error) override;
  void event(LoggingContext, const std::vector<Stat>&) override;
};

}

#endif
