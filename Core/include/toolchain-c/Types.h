/*===-- toolchain-c/Support.h - C Interface Types declarations ---------*- C -*-===*\
|*                                                                            *|
|*                     The TOOLCHAIN Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file defines types used by the C interface to TOOLCHAIN.                   *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef TOOLCHAIN_C_TYPES_H
#define TOOLCHAIN_C_TYPES_H

#include "toolchain-c/DataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup TOOLCHAINCSupportTypes Types and Enumerations
 *
 * @{
 */

typedef int TOOLCHAINBool;

/* Opaque types. */

/**
 * TOOLCHAIN uses a polymorphic type hierarchy which C cannot represent, therefore
 * parameters must be passed as base types. Despite the declared types, most
 * of the functions provided operate only on branches of the type hierarchy.
 * The declared parameter names are descriptive and specify which type is
 * required. Additionally, each type hierarchy is documented along with the
 * functions that operate upon it. For more detail, refer to TOOLCHAIN's C++ code.
 * If in doubt, refer to Core.cpp, which performs parameter downcasts in the
 * form unwrap<RequiredType>(Param).
 */

/**
 * Used to pass regions of memory through TOOLCHAIN interfaces.
 *
 * @see toolchain::MemoryBuffer
 */
typedef struct TOOLCHAINOpaqueMemoryBuffer *TOOLCHAINMemoryBufferRef;

/**
 * The top-level container for all TOOLCHAIN global data. See the TOOLCHAINContext class.
 */
typedef struct TOOLCHAINOpaqueContext *TOOLCHAINContextRef;

/**
 * The top-level container for all other TOOLCHAIN Intermediate Representation (IR)
 * objects.
 *
 * @see toolchain::Module
 */
typedef struct TOOLCHAINOpaqueModule *TOOLCHAINModuleRef;

/**
 * Each value in the TOOLCHAIN IR has a type, an TOOLCHAINTypeRef.
 *
 * @see toolchain::Type
 */
typedef struct TOOLCHAINOpaqueType *TOOLCHAINTypeRef;

/**
 * Represents an individual value in TOOLCHAIN IR.
 *
 * This models toolchain::Value.
 */
typedef struct TOOLCHAINOpaqueValue *TOOLCHAINValueRef;

/**
 * Represents a basic block of instructions in TOOLCHAIN IR.
 *
 * This models toolchain::BasicBlock.
 */
typedef struct TOOLCHAINOpaqueBasicBlock *TOOLCHAINBasicBlockRef;

/**
 * Represents an TOOLCHAIN Metadata.
 *
 * This models toolchain::Metadata.
 */
typedef struct TOOLCHAINOpaqueMetadata *TOOLCHAINMetadataRef;

/**
 * Represents an TOOLCHAIN basic block builder.
 *
 * This models toolchain::IRBuilder.
 */
typedef struct TOOLCHAINOpaqueBuilder *TOOLCHAINBuilderRef;

/**
 * Represents an TOOLCHAIN debug info builder.
 *
 * This models toolchain::DIBuilder.
 */
typedef struct TOOLCHAINOpaqueDIBuilder *TOOLCHAINDIBuilderRef;

/**
 * Interface used to provide a module to JIT or interpreter.
 * This is now just a synonym for toolchain::Module, but we have to keep using the
 * different type to keep binary compatibility.
 */
typedef struct TOOLCHAINOpaqueModuleProvider *TOOLCHAINModuleProviderRef;

/** @see toolchain::PassManagerBase */
typedef struct TOOLCHAINOpaquePassManager *TOOLCHAINPassManagerRef;

/** @see toolchain::PassRegistry */
typedef struct TOOLCHAINOpaquePassRegistry *TOOLCHAINPassRegistryRef;

/**
 * Used to get the users and usees of a Value.
 *
 * @see toolchain::Use */
typedef struct TOOLCHAINOpaqueUse *TOOLCHAINUseRef;

/**
 * Used to represent an attributes.
 *
 * @see toolchain::Attribute
 */
typedef struct TOOLCHAINOpaqueAttributeRef *TOOLCHAINAttributeRef;

/**
 * @see toolchain::DiagnosticInfo
 */
typedef struct TOOLCHAINOpaqueDiagnosticInfo *TOOLCHAINDiagnosticInfoRef;

/**
 * @see toolchain::Comdat
 */
typedef struct TOOLCHAINComdat *TOOLCHAINComdatRef;

/**
 * @see toolchain::Module::ModuleFlagEntry
 */
typedef struct TOOLCHAINOpaqueModuleFlagEntry TOOLCHAINModuleFlagEntry;

/**
 * @see toolchain::JITEventListener
 */
typedef struct TOOLCHAINOpaqueJITEventListener *TOOLCHAINJITEventListenerRef;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
