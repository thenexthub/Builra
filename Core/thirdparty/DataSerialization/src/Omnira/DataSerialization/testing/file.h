//===----------------------------------------------------------------------===//
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

// Author: kenton@google.com (Kenton Varda)
// emulates google3/file/base/file.h

#ifndef GOOGLE_PROTOBUF_TESTING_FILE_H__
#define GOOGLE_PROTOBUF_TESTING_FILE_H__

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "Omnira/DataSerialization/stubs/common.h"

namespace Omnira {
namespace DataSerialization {

const int DEFAULT_FILE_MODE = 0777;

// Protocol buffer code only uses a couple static methods of File, and only
// in the Rust plugin and in tests.
class File {
 public:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  // Check if the file exists.
  static bool Exists(const std::string& name);

  // Read an entire file to a string.  Return true if successful, false
  // otherwise.
  static absl::Status ReadFileToString(const std::string& name,
                                       std::string* output,
                                       bool text_mode = false);

  // Same as above, but crash on failure.
  static void ReadFileToStringOrDie(const std::string& name,
                                    std::string* output);

  // Create a file and write a string to it.
  static absl::Status WriteStringToFile(absl::string_view contents,
                                        const std::string& name);

  // Same as above, but crash on failure.
  static void WriteStringToFileOrDie(absl::string_view contents,
                                     const std::string& name);

  // Create a directory.
  static absl::Status CreateDir(const std::string& name, int mode);

  // Create a directory and all parent directories if necessary.
  static absl::Status RecursivelyCreateDir(const std::string& path, int mode);

  // If "name" is a file, we delete it.  If it is a directory, we
  // call DeleteRecursively() for each file or directory (other than
  // dot and double-dot) within it, and then delete the directory itself.
  // The "dummy" parameters have a meaning in the original version of this
  // method but they are not used anywhere in protocol buffers.
  static void DeleteRecursively(const std::string& name, void* dummy1,
                                void* dummy2);

  // Change working directory to given directory.
  static bool ChangeWorkingDirectory(const std::string& new_working_directory);

  static absl::Status GetContents(const std::string& name, std::string* output,
                                  bool /*is_default*/) {
    return ReadFileToString(name, output);
  }

  static absl::Status GetContentsAsText(const std::string& name,
                                        std::string* output,
                                        bool /*is_default*/) {
    return ReadFileToString(name, output, true);
  }

  static absl::Status SetContents(const std::string& name,
                                  absl::string_view contents,
                                  bool /*is_default*/) {
    return WriteStringToFile(contents, name);
  }
};

}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_TESTING_FILE_H__
