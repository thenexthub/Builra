//===- unittests/Core/SQLiteBuildDBTest.cpp -----------------------------===//
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

#include "builra/Core/BuildDB.h"

#include "toolchain/ADT/SmallString.h"
#include "toolchain/Support/FileSystem.h"

#include "gtest/gtest.h"

#include <sqlite3.h>

using namespace builra;
using namespace builra::core;

TEST(SQLiteBuildDBTest, ErrorHandling) {
    // Create a temporary file.
    toolchain::SmallString<256> dbPath;
    auto ec = toolchain::sys::fs::createTemporaryFile("build", "db", dbPath);
    EXPECT_EQ(bool(ec), false);
    const char* path = dbPath.c_str();
    fprintf(stderr, "using db: %s\n", path);

    std::string error;
    std::unique_ptr<BuildDB> buildDB = createSQLiteBuildDB(dbPath, 1, /* recreateUnmatchedVersion = */ true, &error);
    EXPECT_TRUE(buildDB != nullptr);
    EXPECT_EQ(error, "");

    sqlite3 *db = nullptr;
    sqlite3_open(path, &db);
    sqlite3_exec(db, "PRAGMA locking_mode = EXCLUSIVE; BEGIN EXCLUSIVE;", nullptr, nullptr, nullptr);

    buildDB = createSQLiteBuildDB(dbPath, 1, /* recreateUnmatchedVersion = */ true, &error);
    EXPECT_FALSE(buildDB == nullptr);

    // The database is opened lazily, thus run an operation that will cause it
    // to be opened and verify that it fails as expected.
    bool result = true;
    buildDB->getCurrentEpoch(&result, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error.find("database is locked") != std::string::npos);

    // Clean up database connections before unlinking
    sqlite3_exec(db, "END;", nullptr, nullptr, nullptr);
    sqlite3_close(db);
    buildDB = nullptr;

    ec = toolchain::sys::fs::remove(dbPath.str());
    EXPECT_EQ(bool(ec), false);
}

TEST(SQLiteBuildDBTest, LockedWhileBuilding) {
  // Create a temporary file.
  toolchain::SmallString<256> dbPath;
  auto ec = toolchain::sys::fs::createTemporaryFile("build", "db", dbPath);
  EXPECT_EQ(bool(ec), false);
  const char* path = dbPath.c_str();
  fprintf(stderr, "using db: %s\n", path);

  std::string error;
  std::unique_ptr<BuildDB> buildDB = createSQLiteBuildDB(dbPath, 1, /* recreateUnmatchedVersion = */ true, &error);
  EXPECT_TRUE(buildDB != nullptr);
  EXPECT_EQ(error, "");

  std::unique_ptr<BuildDB> secondBuildDB = createSQLiteBuildDB(dbPath, 1, /* recreateUnmatchedVersion = */ true, &error);
  EXPECT_TRUE(buildDB != nullptr);
  EXPECT_EQ(error, "");

  bool result = buildDB->buildStarted(&error);
  EXPECT_TRUE(result);
  EXPECT_EQ(error, "");

  // Tests that we cannot start a second build with an existing connection
  result = secondBuildDB->buildStarted(&error);
  EXPECT_FALSE(result);
  EXPECT_TRUE(error.find("database is locked") != std::string::npos);

  // Tests that we cannot create new connections while a build is running
  std::unique_ptr<BuildDB> otherBuildDB = createSQLiteBuildDB(dbPath, 1, /* recreateUnmatchedVersion = */ true, &error);
  EXPECT_FALSE(otherBuildDB == nullptr);

  // The database is opened lazily, thus run an operation that will cause it
  // to be opened and verify that it fails as expected.
  bool success = true;
  otherBuildDB->getCurrentEpoch(&success, &error);
  EXPECT_FALSE(success);
  EXPECT_TRUE(error.find("database is locked") != std::string::npos);

  // Clean up database connections before unlinking
  buildDB->buildComplete();
  buildDB = nullptr;
  secondBuildDB = nullptr;
  otherBuildDB = nullptr;

  ec = toolchain::sys::fs::remove(dbPath.str());
  EXPECT_EQ(bool(ec), false);
}

TEST(SQLiteBuildDBTest, CloseDBConnectionAfterCloseCall) {
  // Create a temporary file.
  toolchain::SmallString<256> dbPath;
  auto ec = toolchain::sys::fs::createTemporaryFile("build", "db", dbPath);
  EXPECT_EQ(bool(ec), false);
  const char* path = dbPath.c_str();
  fprintf(stderr, "using db: %s\n", path);
  
  std::string error;
  std::unique_ptr<BuildDB> buildDB = createSQLiteBuildDB(dbPath, 1, /* recreateUnmatchedVersion = */ true, &error);
  EXPECT_TRUE(buildDB != nullptr);
  EXPECT_EQ(error, "");
  
  buildDB->buildStarted(&error);
  EXPECT_EQ(error, "");
  
  buildDB->buildComplete();
}
