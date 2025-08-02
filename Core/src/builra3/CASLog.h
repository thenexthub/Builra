//===- CASLog.h -------------------------------------------------*- C++ -*-===//
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

#ifndef BUILRA3_CASLOG_H
#define BUILRA3_CASLOG_H

#include <cstdint>
#include <deque>
#include <functional>
#include <optional>
#include <memory>
#include <mutex>
#include <vector>

#include <builra3/Errors.hpp>

#include "builra3/CAS.h"
#include "builra3/CASTree.h"
#include "builra3/Error.pb.h"


namespace builra3 {

class CASLogWriter: public std::enable_shared_from_this<CASLogWriter> {
public:
  typedef std::function<void(result<CASID, Error>, bool terminal)> Handler;
private:
  struct Private{ explicit Private() = default; };

private:
  std::shared_ptr<CASDatabase> db;
  uint64_t maxEntries;

  std::mutex mutex;
  bool isOK{true};

  uint64_t writeOpCount{0};
  uint64_t lastWriteOp{0};
  std::optional<CASID> currentID;
  std::optional<CASID> prevID;
  std::optional<CASID> rootID;
  uint64_t prevSize{0};
  uint64_t rootSize{0};

  uint64_t currentSize{0};
  std::vector<NamedDirectoryEntryID> directoryEntries;

  struct PendingWriteOp {
    uint64_t opIdx;
    Handler handler;
  };
  std::deque<PendingWriteOp> pendingOps;
  bool flushPending{false};


public:
  CASLogWriter(Private, std::shared_ptr<CASDatabase> db, uint64_t max = 256) : db(db), maxEntries(max) {
    if (maxEntries < 3) {
      maxEntries = 3;
    }
  }

  static std::shared_ptr<CASLogWriter> create(std::shared_ptr<CASDatabase> db, uint64_t max = 256) {
    return std::make_shared<CASLogWriter>(Private(), std::move(db), max);
  }

  std::optional<CASID> getLatestID() {
    std::lock_guard<std::mutex> lock(mutex);
    if (currentID.has_value()) {
      return currentID;
    }

    return prevID;
  }

  void append(std::string_view data, uint8_t channel, Handler handler);

  inline void append(std::string_view data, Handler handler) {
    append(data, 0, handler);
  }

private:
  void postChunk(uint8_t channel, Handler handler, uint64_t size,
                 result<CASID, Error> res);
  void postSync(uint64_t op, bool isFlush, result<CASID, Error> res);
};

/// Stream writer that buffers data before ingesting it into the CAS Database.
/// NOTE: NOT THREAD SAFE. Although the underlying writer is, the stream buffer
/// is not, and should be protected by the client if necessary.
class BufferedStreamCASLogWriter {
public:
  typedef std::function<void(result<CASID, Error>)> Handler;
private:
  uint64_t bufferSize;
  std::shared_ptr<CASLogWriter> writer;
  std::string currentBuffer;
  uint8_t currentChannel{0};

public:
  BufferedStreamCASLogWriter(std::shared_ptr<CASDatabase> db, uint64_t bufferSize = 1 << 19)
    : bufferSize(bufferSize), writer(CASLogWriter::create(db))
  {
    currentBuffer.reserve(bufferSize);
  }

  std::optional<CASID> getLatestID() {
    return writer->getLatestID();
  }

  void write(std::string_view data, uint8_t channel, Handler handler) {
    if (channel != currentChannel || data.size() > (bufferSize - currentBuffer.size())) {
      flush({});
    }

    if (data.size() >= bufferSize) {
      writer->append(data, channel, [handler](result<CASID, Error> res, bool terminal) {
        if (!terminal) {
          return;
        }

        if (handler) handler(res);
      });
      return;
    }

    currentChannel = channel;
    currentBuffer += data;

    if (currentBuffer.size() == bufferSize) {
      flush(handler);
      return;
    }

    if (handler) {
      if (auto latest = writer->getLatestID(); latest.has_value()) {
        handler(*latest);
      } else {
        handler({});
      }
    }
  }

  void flush(Handler handler) {
    if (currentBuffer.size() == 0) {
      // empty buffer, nothing to do
      return;
    }

    writer->append(currentBuffer, currentChannel, [handler](result<CASID, Error> res, bool terminal){
      if (!terminal) {
        return;
      }

      if (handler) handler(res);
    });
  }
};



}

#endif
