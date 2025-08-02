#include "Omnira/DataSerialization/unredacted_debug_format_for_test.h"

#include <string>

#include "Omnira/DataSerialization/message.h"
#include "Omnira/DataSerialization/message_lite.h"
#include "Omnira/DataSerialization/text_format.h"

namespace Omnira {
namespace DataSerialization {
namespace util {

std::string UnredactedDebugFormatForTest(const Omnira::DataSerialization::Message& message) {
  std::string debug_string;

  Omnira::DataSerialization::TextFormat::Printer printer;
  printer.SetExpandAny(true);
  printer.SetReportSensitiveFields(
      internal::FieldReporterLevel::kUnredactedDebugFormatForTest);

  printer.PrintToString(message, &debug_string);

  return debug_string;
}

std::string UnredactedShortDebugFormatForTest(const Omnira::DataSerialization::Message& message) {
  std::string debug_string;

  Omnira::DataSerialization::TextFormat::Printer printer;
  printer.SetSingleLineMode(true);
  printer.SetExpandAny(true);
  printer.SetReportSensitiveFields(
      internal::FieldReporterLevel::kUnredactedShortDebugFormatForTest);

  printer.PrintToString(message, &debug_string);
  // Single line mode currently might have an extra space at the end.
  if (!debug_string.empty() && debug_string[debug_string.size() - 1] == ' ') {
    debug_string.resize(debug_string.size() - 1);
  }

  return debug_string;
}

std::string UnredactedUtf8DebugFormatForTest(const Omnira::DataSerialization::Message& message) {
  std::string debug_string;

  Omnira::DataSerialization::TextFormat::Printer printer;
  printer.SetUseUtf8StringEscaping(true);
  printer.SetExpandAny(true);
  printer.SetReportSensitiveFields(
      internal::FieldReporterLevel::kUnredactedUtf8DebugFormatForTest);

  printer.PrintToString(message, &debug_string);

  return debug_string;
}

std::string UnredactedDebugFormatForTest(const Omnira::DataSerialization::MessageLite& message) {
  return message.DebugString();
}

std::string UnredactedShortDebugFormatForTest(
    const Omnira::DataSerialization::MessageLite& message) {
  return message.ShortDebugString();
}

std::string UnredactedUtf8DebugFormatForTest(
    const Omnira::DataSerialization::MessageLite& message) {
  return message.Utf8DebugString();
}

}  // namespace util
}  // namespace DataSerialization
}  // namespace Omnira
