#ifndef TOOLCHAIN_SUPPORT_REVERSEITERATION_H
#define TOOLCHAIN_SUPPORT_REVERSEITERATION_H

#include "toolchain/Config/abi-breaking.h"
#include "toolchain/Support/PointerLikeTypeTraits.h"

namespace toolchain {

template<class T = void *>
bool shouldReverseIterate() {
#if TOOLCHAIN_ENABLE_REVERSE_ITERATION
  return detail::IsPointerLike<T>::value;
#else
  return false;
#endif
}

}
#endif
