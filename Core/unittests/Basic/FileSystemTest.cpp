//===- unittests/Basic/FileSystemTest.cpp ---------------------------------===//
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

#include "../BuildSystem/TempDir.h"

#include "builra/Basic/FileSystem.h"
#include "builra/Basic/toolchain.h"
#include "builra/Basic/PlatformUtility.h"
#include "builra/Basic/Stat.h"

#include "toolchain/Support/FileSystem.h"
#include "toolchain/Support/MemoryBuffer.h"
#include "toolchain/Support/Path.h"
#include "toolchain/Support/raw_ostream.h"

#include "gtest/gtest.h"

using namespace builra;
using namespace builra::basic;

namespace {

TEST(FileSystemTest, basic) {
  // Check basic sanity of the local filesystem object.
  auto fs = createLocalFileSystem();

  // Write a temp file.
  SmallString<256> tempPath;
  toolchain::sys::fs::createTemporaryFile("FileSystemTests", "txt", tempPath);
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(tempPath.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  auto missingFileInfo = fs->getFileInfo("/does/not/exists");
  EXPECT_TRUE(missingFileInfo.isMissing());
  
  auto ourFileInfo = fs->getFileInfo(tempPath.str());
  EXPECT_FALSE(ourFileInfo.isMissing());

  auto missingFileContents = fs->getFileContents("/does/not/exist");
  EXPECT_EQ(missingFileContents.get(), nullptr);

  auto ourFileContents = fs->getFileContents(tempPath.str());
  EXPECT_EQ(ourFileContents->getBuffer().str(), "Hello, world!");

  // Remote the temporary file.
  auto ec = toolchain::sys::fs::remove(tempPath.str());
  EXPECT_FALSE(ec);
}

TEST(FileSystemTest, testRecursiveRemoval) {
  TmpDir rootTempDir(__func__);

  SmallString<256> tempDir { rootTempDir.str() };
  toolchain::sys::path::append(tempDir, "root");
  sys::mkdir(tempDir.c_str());

  SmallString<256> file{ tempDir.str() };
  toolchain::sys::path::append(file, "test.txt");
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(file.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  SmallString<256> dir{ tempDir.str() };
  toolchain::sys::path::append(dir, "subdir");
  sys::mkdir(dir.c_str());

  toolchain::sys::path::append(dir, "file_in_subdir.txt");
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(dir.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  auto fs = createLocalFileSystem();
  bool result = fs->remove(tempDir.c_str());
  EXPECT_TRUE(result);

  sys::StatStruct statbuf;
  EXPECT_EQ(-1, sys::stat(tempDir.c_str(), &statbuf));
  EXPECT_EQ(ENOENT, errno);
}

TEST(FileSystemTest, testRecursiveRemovalDoesNotFollowSymlinks) {
  TmpDir rootTempDir(__func__);

  SmallString<256> file{ rootTempDir.str() };
  toolchain::sys::path::append(file, "test.txt");
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(file.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  SmallString<256> otherDir{ rootTempDir.str() };
  toolchain::sys::path::append(otherDir, "other_dir");
  sys::mkdir(otherDir.c_str());

  SmallString<256> otherFile{ otherDir.str() };
  toolchain::sys::path::append(otherFile, "test.txt");
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(otherFile.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  SmallString<256> tempDir { rootTempDir.str() };
  toolchain::sys::path::append(tempDir, "root");
  sys::mkdir(tempDir.c_str());

  SmallString<256> linkPath { tempDir.str() };
  toolchain::sys::path::append(linkPath, "link.txt");
  int res = sys::symlink(file.c_str(), linkPath.c_str());
  EXPECT_EQ(res, 0);

  SmallString<256> directoryLinkPath { tempDir.str() };
  toolchain::sys::path::append(directoryLinkPath, "link_to_other_dir");
  res = sys::symlink(otherDir.c_str(), directoryLinkPath.c_str());
  EXPECT_EQ(res, 0);

  auto fs = createLocalFileSystem();
  bool result = fs->remove(tempDir.c_str());
  EXPECT_TRUE(result);

  sys::StatStruct  statbuf;
  EXPECT_EQ(-1, sys::stat(tempDir.c_str(), &statbuf));
  EXPECT_EQ(ENOENT, errno);
  // Verify that the symlink target still exists.
  EXPECT_EQ(0, sys::stat(file.c_str(), &statbuf));
  // Verify that we did not delete the symlinked directories contents.
  EXPECT_EQ(0, sys::stat(otherFile.c_str(), &statbuf));
}

TEST(DeviceAgnosticFileSystemTest, basic) {
  // Check basic sanity of the local filesystem object.
  auto fs = DeviceAgnosticFileSystem::from(createLocalFileSystem());

  // Write a temp file.
  SmallString<256> tempPath;
  toolchain::sys::fs::createTemporaryFile("FileSystemTests", "txt", tempPath);
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(tempPath.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  auto missingFileInfo = fs->getFileInfo("/does/not/exists");
  EXPECT_TRUE(missingFileInfo.isMissing());

  auto ourFileInfo = fs->getFileInfo(tempPath.str());
  EXPECT_FALSE(ourFileInfo.isMissing());

  EXPECT_EQ(ourFileInfo.device, 0ull);
  EXPECT_EQ(ourFileInfo.inode, 0ull);

  auto missingFileContents = fs->getFileContents("/does/not/exist");
  EXPECT_EQ(missingFileContents.get(), nullptr);

  auto ourFileContents = fs->getFileContents(tempPath.str());
  EXPECT_EQ(ourFileContents->getBuffer().str(), "Hello, world!");
  // Remove the temporary file.
  auto ec = toolchain::sys::fs::remove(tempPath.str());
  EXPECT_FALSE(ec);
}

TEST(ChecksumOnlyFileSystem, basic) {
  // Check basic sanity of the local filesystem object.
  auto fs = ChecksumOnlyFileSystem::from(createLocalFileSystem());

  // Write a temp file.
  SmallString<256> tempPath;
  toolchain::sys::fs::createTemporaryFile("FileSystemTests", "txt", tempPath);
  {
    std::error_code ec;
    toolchain::raw_fd_ostream os(tempPath.str(), ec, toolchain::sys::fs::F_Text);
    EXPECT_FALSE(ec);
    os << "Hello, world!";
    os.close();
  }

  auto missingFileInfo = fs->getFileInfo("/does/not/exists");
  EXPECT_TRUE(missingFileInfo.isMissing());

  auto ourFileInfo = fs->getFileInfo(tempPath.str());
  EXPECT_FALSE(ourFileInfo.isMissing());

  EXPECT_EQ(ourFileInfo.device, 0ull);
  EXPECT_EQ(ourFileInfo.inode, 0ull);

  auto missingFileContents = fs->getFileContents("/does/not/exist");
  EXPECT_EQ(missingFileContents.get(), nullptr);

  auto ourFileContents = fs->getFileContents(tempPath.str());
  EXPECT_EQ(ourFileContents->getBuffer().str(), "Hello, world!");
  // Remove the temporary file.
  auto ec = toolchain::sys::fs::remove(tempPath.str());
  EXPECT_FALSE(ec);
}

}
