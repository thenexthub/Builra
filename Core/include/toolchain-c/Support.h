/*===-- toolchain-c/Support.h - Support C Interface --------------------*- C -*-===*\
|*                                                                            *|
|*                     The TOOLCHAIN Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file defines the C interface to the TOOLCHAIN support library.             *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef TOOLCHAIN_C_SUPPORT_H
#define TOOLCHAIN_C_SUPPORT_H

#include "toolchain-c/DataTypes.h"
#include "toolchain-c/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function permanently loads the dynamic library at the given path.
 * It is safe to call this function multiple times for the same library.
 *
 * @see sys::DynamicLibrary::LoadLibraryPermanently()
  */
TOOLCHAINBool TOOLCHAINLoadLibraryPermanently(const char* Filename);

/**
 * This function parses the given arguments using the TOOLCHAIN command line parser.
 * Note that the only stable thing about this function is its signature; you
 * cannot rely on any particular set of command line arguments being interpreted
 * the same way across TOOLCHAIN versions.
 *
 * @see toolchain::cl::ParseCommandLineOptions()
 */
void TOOLCHAINParseCommandLineOptions(int argc, const char *const *argv,
                                 const char *Overview);

/**
 * This function will search through all previously loaded dynamic
 * libraries for the symbol \p symbolName. If it is found, the address of
 * that symbol is returned. If not, null is returned.
 *
 * @see sys::DynamicLibrary::SearchForAddressOfSymbol()
 */
void *TOOLCHAINSearchForAddressOfSymbol(const char *symbolName);

/**
 * This functions permanently adds the symbol \p symbolName with the
 * value \p symbolValue.  These symbols are searched before any
 * libraries.
 *
 * @see sys::DynamicLibrary::AddSymbol()
 */
void TOOLCHAINAddSymbol(const char *symbolName, void *symbolValue);

#ifdef __cplusplus
}
#endif

#endif
