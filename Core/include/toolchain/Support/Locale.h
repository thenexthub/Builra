#ifndef TOOLCHAIN_SUPPORT_LOCALE_H
#define TOOLCHAIN_SUPPORT_LOCALE_H

namespace toolchain {
class StringRef;

namespace sys {
namespace locale {

int columnWidth(StringRef s);
bool isPrint(int c);

}
}
}

#endif // TOOLCHAIN_SUPPORT_LOCALE_H
