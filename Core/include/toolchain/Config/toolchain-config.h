/*===------- toolchain/Config/toolchain-config.h - toolchain configuration -------*- C -*-===*/
/*                                                                            */
/*                     The TOOLCHAIN Compiler Infrastructure                  */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

/* This file enumerates variables from the TOOLCHAIN configuration so that they
   can be in exported headers and won't override package specific directives.
   This is a C header that can be included in the toolchain-c headers. */

#ifndef TOOLCHAIN_CONFIG_H
#define TOOLCHAIN_CONFIG_H

/* Define if TOOLCHAIN_ENABLE_DUMP is enabled */
/* #undef TOOLCHAIN_ENABLE_DUMP */

/* Define if we link Polly to the tools */
/* #undef LINK_POLLY_INTO_TOOLS */

/* Target triple TOOLCHAIN will generate code for by default */
#define TOOLCHAIN_DEFAULT_TARGET_TRIPLE ""

/* Define if threads enabled */
#define TOOLCHAIN_ENABLE_THREADS 1

/* Has gcc/MSVC atomic intrinsics */
#define TOOLCHAIN_HAS_ATOMICS 1

/* Host triple TOOLCHAIN will be executed on */
#define TOOLCHAIN_HOST_TRIPLE ""

/* TOOLCHAIN architecture name for the native architecture, if available */
#define TOOLCHAIN_NATIVE_ARCH X86

/* TOOLCHAIN name for the native AsmParser init function, if available */
#define TOOLCHAIN_NATIVE_ASMPARSER TOOLCHAINInitializeX86AsmParser

/* TOOLCHAIN name for the native AsmPrinter init function, if available */
#define TOOLCHAIN_NATIVE_ASMPRINTER TOOLCHAINInitializeX86AsmPrinter

/* TOOLCHAIN name for the native Disassembler init function, if available */
#define TOOLCHAIN_NATIVE_DISASSEMBLER TOOLCHAINInitializeX86Disassembler

/* TOOLCHAIN name for the native Target init function, if available */
#define TOOLCHAIN_NATIVE_TARGET TOOLCHAINInitializeX86Target

/* TOOLCHAIN name for the native TargetInfo init function, if available */
#define TOOLCHAIN_NATIVE_TARGETINFO TOOLCHAINInitializeX86TargetInfo

/* TOOLCHAIN name for the native target MC init function, if available */
#define TOOLCHAIN_NATIVE_TARGETMC TOOLCHAINInitializeX86TargetMC

/* Define if this is Unixish platform */
#if !defined(TOOLCHAIN_ON_WIN32)
#define TOOLCHAIN_ON_UNIX 1
#endif

/* Define if we have the Intel JIT API runtime support library */
#define TOOLCHAIN_USE_INTEL_JITEVENTS 0

/* Define if we have the oprofile JIT-support library */
#define TOOLCHAIN_USE_OPROFILE 0

/* Define if we have the perf JIT-support library */
#define TOOLCHAIN_USE_PERF 0

/* Major version of the TOOLCHAIN API */
#define TOOLCHAIN_VERSION_MAJOR 7

/* Minor version of the TOOLCHAIN API */
#define TOOLCHAIN_VERSION_MINOR 0

/* Patch version of the TOOLCHAIN API */
#define TOOLCHAIN_VERSION_PATCH 0

/* TOOLCHAIN version string */
#define TOOLCHAIN_VERSION_STRING "7.0.0svn"

/* Whether TOOLCHAIN records statistics for use with GetStatistics(),
 * PrintStatistics() or PrintStatisticsJSON()
 */
#define TOOLCHAIN_FORCE_ENABLE_STATS 0

#endif
