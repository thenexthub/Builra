//===- unittests/Core/DependencyInfoParserTest.cpp ------------------------===//
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

#include "builra/Core/DependencyInfoParser.h"

#include "gtest/gtest.h"

#include <string>
#include <vector>

using namespace builra;
using namespace builra::core;

namespace {

TEST(DependencyInfoParserTest, basic) {
  typedef std::pair<std::string, std::string> Event;
  typedef std::pair<std::string, uint64_t> Error;
  struct TestActions : public DependencyInfoParser::ParseActions {
    std::vector<Event> events;
    std::vector<Error> errors;

    virtual void error(const char* message, uint64_t length) override {
      errors.push_back({ message, length });
    }

    virtual void actOnVersion(StringRef name) override {
      events.push_back({ "version", std::string(name) });
    }

    virtual void actOnInput(StringRef name) override {
      events.push_back({ "input", std::string(name) });
    }

    virtual void actOnOutput(StringRef name) override {
      events.push_back({ "output", std::string(name) });
    }

    virtual void actOnMissing(StringRef name) override {
      events.push_back({ "missing", std::string(name) });
    }
  };

#define INPUT(str)                                                             \
  StringRef(str, sizeof(str) - 1);                                             \
  assert(sizeof(str) != 0);

  // Check missing terminator diagnose (on empty file).
  {
    TestActions actions;
    auto input = INPUT("xxx");
    DependencyInfoParser(input, actions).parse();
    EXPECT_EQ(std::vector<Error>{ Error("missing null terminator", 3) },
              actions.errors);
    EXPECT_EQ(std::vector<Event>{}, actions.events);
  }

  // Check invalid initial record.
  {
    TestActions actions;
    auto input = INPUT("\x01\x00");
    DependencyInfoParser(input, actions).parse();
    EXPECT_EQ(std::vector<Error>{ Error("missing version record", 0) },
              actions.errors);
    EXPECT_EQ(std::vector<Event>{}, actions.events);
  }

  // Check empty operand.
  {
    TestActions actions;
    auto input = INPUT("\x00\x00");
    DependencyInfoParser(input, actions).parse();
    EXPECT_EQ(std::vector<Error>{ Error("empty operand", 0) },
              actions.errors);
    EXPECT_EQ(std::vector<Event>{}, actions.events);
  }

  // Check duplicate version.
  {
    TestActions actions;
    auto input = INPUT("\x00VERSION\x00\x00VERSION\x00");
    DependencyInfoParser(input, actions).parse();
    EXPECT_EQ(std::vector<Error>{ Error("invalid duplicate version", 9) },
              actions.errors);
    EXPECT_EQ(std::vector<Event>{ Event("version", "VERSION") },
              actions.events);
  }

  // Check a valid file.
  {
    TestActions actions;
    auto input = INPUT("\x00VERSION\x00"
                       "\x10INPUT\x00"
                       "\x11MISSING\x00"
                       "\x40OUTPUT\x00");
    DependencyInfoParser(input, actions).parse();
    EXPECT_EQ(std::vector<Error>{}, actions.errors);
    auto expectedEvents = std::vector<Event>{
        Event("version", "VERSION"),
        Event("input", "INPUT"),
        Event("missing", "MISSING"),
        Event("output", "OUTPUT"),
    };
    EXPECT_EQ(expectedEvents, actions.events);
  }
}

}
