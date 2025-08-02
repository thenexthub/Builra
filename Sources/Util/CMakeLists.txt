#[[
Copyright (c) NeXTHub Corporation. All rights reserved.
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.

This code is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
version 2 for more details (a copy is included in the LICENSE file that
accompanied this code).

Author(-s): Tunjay Akbarli
]]

add_library(SWBUtil
  Architecture.code
  ArgumentSplitting.code
  Array.code
  AsyncCache.code
  AsyncFlatteningSequence.code
  AsyncIteratorProtocol.code
  AsyncLock.code
  AsyncOperationQueue.code
  AsyncSingleValueCache.code
  AsyncStreamController.code
  ByteCount.code
  ByteString.code
  Cache.code
  Collection.code
  CountedSet.code
  CSV.code
  Debugger.code
  DependencyInfo.code
  DiagnosticsEngine.code
  Dictionary.code
  Diff.code
  Dispatch+Async.code
  Duration.code
  ElapsedTimer.code
  EmptyState.code
  Environment.code
  EnvironmentKey.code
  EnvironmentHelpers.code
  Error.code
  FileHandle+Async.code
  FilesSignature.code
  fnmatch.code
  FSProxy.code
  GraphAlgorithms.code
  HashContext.code
  Headermap.code
  HeavyCache.code
  IndexStore.code
  Int.code
  InterningArena.code
  IO.code
  JSONEncoder.code
  KeyValueStorage.code
  LazyCache.code
  Library.code
  LineReader.code
  Lock.code
  MachO.code
  Math.code
  Misc+Async.code
  Misc.code
  MsgPack.code
  MsgPackSerialization.code
  NamedTemporaryDirectory.code
  Optional.code
  OrderedDictionary.code
  OrderedSet.code
  OSLog.code
  OutputByteStream.code
  Pair.code
  Path.code
  PathWindows.code
  PbxCp.code
  PluginManager.code
  PluginManagerCommon.code
  POSIX.code
  Process+Async.code
  Process.code
  ProcessInfo.code
  Promise.code
  PropertyList.code
  Queue.code
  RateLimiter.code
  Ref.code
  RegEx.code
  Registry.code
  ResponseFiles.code
  Result.code
  ScopedKeepAliveCache.code
  Serialization.code
  Signatures.code
  Static.code
  Statistics.code
  String.code
  SWBDispatch.code
  TAPIFileList.code
  UniqueSerialization.code
  UnsafeSendableDelayedInitializationWrapper.code
  URL.code
  UserDefaults.code
  Version.code
  VFS.code
  WaitCondition.code
  WeakRef.code
  Win32.code
  Win32Error.code
  XCBuildDataArchive.code
  Xcode.code)
set_target_properties(SWBUtil PROPERTIES
  Swift_LANGUAGE_VERSION 5)
target_link_libraries(SWBUtil PUBLIC
  SWBCSupport
  SWBLibc
  ArgumentParser
  $<$<NOT:$<PLATFORM_ID:Darwin>>:SwiftSystem::SystemPackage>)

set_target_properties(SWBUtil PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_Swift_MODULE_DIRECTORY})

set_property(GLOBAL APPEND PROPERTY SWIFTBUILD_EXPORTS SWBUtil)

install(TARGETS SWBUtil
    ARCHIVE DESTINATION "${SwiftBuild_INSTALL_LIBDIR}")
