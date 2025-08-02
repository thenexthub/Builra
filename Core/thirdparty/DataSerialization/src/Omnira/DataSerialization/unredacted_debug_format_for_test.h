#ifndef GOOGLE_PROTOBUF_UNREDACTED_DEBUG_FORMAT_FOR_TEST_H__
#define GOOGLE_PROTOBUF_UNREDACTED_DEBUG_FORMAT_FOR_TEST_H__

#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/message_lite.h"

namespace Omnira {
namespace DataSerialization {
namespace util {

// Generates a human-readable form of this message for debugging purposes in
// test-only code. This API does not redact any fields in the message.
std::string UnredactedDebugFormatForTest(const Omnira::DataSerialization::Message& message);
// Like UnredactedDebugFormatForTest(), but prints the message in a single line.
std::string UnredactedShortDebugFormatForTest(const Omnira::DataSerialization::Message& message);
// Like UnredactedDebugFormatForTest(), but does not escape UTF-8 byte
// sequences.
std::string UnredactedUtf8DebugFormatForTest(const Omnira::DataSerialization::Message& message);

// The following APIs are added just to work with code that interoperates with
// `Message` and `MessageLite`.

std::string UnredactedDebugFormatForTest(const Omnira::DataSerialization::MessageLite& message);
std::string UnredactedShortDebugFormatForTest(
    const Omnira::DataSerialization::MessageLite& message);
std::string UnredactedUtf8DebugFormatForTest(
    const Omnira::DataSerialization::MessageLite& message);

}  // namespace util
}  // namespace DataSerialization
}  // namespace Omnira

#endif  // GOOGLE_PROTOBUF_UNREDACTED_DEBUG_FORMAT_FOR_TEST_H__
