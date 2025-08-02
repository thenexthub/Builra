#include "toolchain/Support/Locale.h"
#include "toolchain/ADT/StringRef.h"
#include "toolchain/Support/Unicode.h"

namespace toolchain {
namespace sys {
namespace locale {

int columnWidth(StringRef Text) {
#if _WIN32
  return Text.size();
#else
  return toolchain::sys::unicode::columnWidthUTF8(Text);
#endif
}

bool isPrint(int UCS) {
#if _WIN32
  // Restrict characters that we'll try to print to the lower part of ASCII
  // except for the control characters (0x20 - 0x7E). In general one can not
  // reliably output code points U+0080 and higher using narrow character C/C++
  // output functions in Windows, because the meaning of the upper 128 codes is
  // determined by the active code page in the console.
  return ' ' <= UCS && UCS <= '~';
#else
  return toolchain::sys::unicode::isPrintable(UCS);
#endif
}

} // namespace locale
} // namespace sys
} // namespace toolchain
