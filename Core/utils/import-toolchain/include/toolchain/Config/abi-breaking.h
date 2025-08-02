/*===------- toolchain/Config/abi-breaking.h - toolchain configuration -------*- C -*-===*/
/*                                                                            */
/*                     The TOOLCHAIN Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

/* This file controls the C++ ABI break introduced in TOOLCHAIN public header. */

#ifndef TOOLCHAIN_ABI_BREAKING_CHECKS_H
#define TOOLCHAIN_ABI_BREAKING_CHECKS_H

/* Define to enable checks that alter the TOOLCHAIN C++ ABI */
#define TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS 1

/* Define to enable reverse iteration of unordered toolchain containers */
#define TOOLCHAIN_ENABLE_REVERSE_ITERATION 0

/* Allow selectively disabling link-time mismatch checking so that header-only
   ADT content from TOOLCHAIN can be used without linking libSupport. */
#if !TOOLCHAIN_DISABLE_ABI_BREAKING_CHECKS_ENFORCING

// ABI_BREAKING_CHECKS protection: provides link-time failure when clients build
// mismatch with TOOLCHAIN
#if defined(_MSC_VER)
// Use pragma with MSVC
#define TOOLCHAIN_XSTR(s) TOOLCHAIN_STR(s)
#define TOOLCHAIN_STR(s) #s
#pragma detect_mismatch("TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS", TOOLCHAIN_XSTR(TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS))
#undef TOOLCHAIN_XSTR
#undef TOOLCHAIN_STR
#elif defined(_WIN32) || defined(__CYGWIN__) // Win32 w/o #pragma detect_mismatch
// FIXME: Implement checks without weak.
#elif defined(__cplusplus)
namespace toolchain {
#if TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS
extern int EnableABIBreakingChecks;
__attribute__((weak, visibility ("hidden"))) int *VerifyEnableABIBreakingChecks = &EnableABIBreakingChecks;
#else
extern int DisableABIBreakingChecks;
__attribute__((weak, visibility ("hidden"))) int *VerifyDisableABIBreakingChecks = &DisableABIBreakingChecks;
#endif
}
#endif // _MSC_VER

#endif // TOOLCHAIN_DISABLE_ABI_BREAKING_CHECKS_ENFORCING

#endif
