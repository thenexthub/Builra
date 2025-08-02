/*===-- toolchain-c/ErrorHandling.h - Error Handling C Interface -------*- C -*-===*\
|*                                                                            *|
|*                     The TOOLCHAIN Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file defines the C interface to TOOLCHAIN's error handling mechanism.      *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef TOOLCHAIN_C_ERROR_HANDLING_H
#define TOOLCHAIN_C_ERROR_HANDLING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TOOLCHAINFatalErrorHandler)(const char *Reason);

/**
 * Install a fatal error handler. By default, if TOOLCHAIN detects a fatal error, it
 * will call exit(1). This may not be appropriate in many contexts. For example,
 * doing exit(1) will bypass many crash reporting/tracing system tools. This
 * function allows you to install a callback that will be invoked prior to the
 * call to exit(1).
 */
void TOOLCHAINInstallFatalErrorHandler(TOOLCHAINFatalErrorHandler Handler);

/**
 * Reset the fatal error handler. This resets TOOLCHAIN's fatal error handling
 * behavior to the default.
 */
void TOOLCHAINResetFatalErrorHandler(void);

/**
 * Enable TOOLCHAIN's built-in stack trace code. This intercepts the OS's crash
 * signals and prints which component of TOOLCHAIN you were in at the time if the
 * crash.
 */
void TOOLCHAINEnablePrettyStackTrace(void);

#ifdef __cplusplus
}
#endif

#endif
