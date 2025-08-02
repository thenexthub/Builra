//===-- ShellCommand.cpp --------------------------------------------------===//
//
// Copyright (c) NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
//
//===----------------------------------------------------------------------===//

#include "builra/BuildSystem/ShellCommand.h"

#include "builra/Basic/FileSystem.h"
#include "builra/BuildSystem/BuildFile.h"
#include "builra/BuildSystem/BuildKey.h"
#include "builra/Core/DependencyInfoParser.h"
#include "builra/Core/MakefileDepsParser.h"

#include "toolchain/Support/MemoryBuffer.h"
#include "toolchain/Support/Path.h"

using namespace toolchain;
using namespace builra;
using namespace builra::basic;
using namespace builra::core;
using namespace builra::buildsystem;

void ShellCommand::start(BuildSystem& system, TaskInterface ti) {
  // Resolve the plugin state.
  handler = system.resolveShellCommandHandler(this);

  // Delegate to handler, if used.
  if (handler) {
    handlerState = handler->start(ti, this);
  }

  this->ExternalCommand::start(system, ti);
}

CommandSignature ShellCommand::getSignature() const {
  CommandSignature signature = cachedSignature;
  if (!signature.isNull())
    return signature;

  auto code = ExternalCommand::getSignature();
  if (!signatureData.empty()) {
    code = code.combine(signatureData);
  } else {
    for (const auto& arg: args) {
      code = code.combine(arg);
    }
    for (const auto& entry: env) {
      code = code.combine(entry.first);
      code = code.combine(entry.second);
    }
    for (const auto& path: depsPaths) {
      code = code.combine(path);
    }
    code = code.combine(int(depsStyle));
    code = code.combine(int(inheritEnv));
    code = code.combine(int(canSafelyInterrupt));
  }
  signature = code;
  if (signature.isNull()) {
    signature = CommandSignature(1);
  }
  cachedSignature = signature;
  return signature;
}

bool ShellCommand::processDiscoveredDependencies(BuildSystem& system,
                                                 core::TaskInterface ti,
                                                 QueueJobContext* context) {
  // It is an error if the dependencies style is not specified.
  //
  // FIXME: Diagnose this sooner.
  if (depsStyle == DepsStyle::Unused) {
    system.getDelegate().commandHadError(
        this, "missing required 'deps-style' specifier");
    return false;
  }

  for (const auto& depsPath: depsPaths) {
    // Read the dependencies file.
    std::unique_ptr<toolchain::MemoryBuffer> input;
    if (toolchain::sys::path::is_absolute(depsPath)) {
      input = system.getFileSystem().getFileContents(depsPath);
    } else {
      SmallString<PATH_MAX> absPath = StringRef(workingDirectory);
      toolchain::sys::path::append(absPath, depsPath);
      toolchain::sys::fs::make_absolute(absPath);
      input = system.getFileSystem().getFileContents(StringRef(absPath));
    }
    if (!input) {
      system.getDelegate().commandHadError(
          this, "unable to open dependencies file (" + depsPath + ")");
      return false;
    }

    switch (depsStyle) {
    case DepsStyle::Unused:
      assert(0 && "unreachable");
      break;

    case DepsStyle::Makefile:
      if (!processMakefileDiscoveredDependencies(
              system, ti, context, depsPath, input.get(), false))
        return false;
      continue;

    case DepsStyle::DependencyInfo:
      if (!processDependencyInfoDiscoveredDependencies(
              system, ti, context, depsPath, input.get()))
        return false;
      continue;

    case DepsStyle::MakefileIgnoringSubsequentOutputs:
      if (!processMakefileDiscoveredDependencies(
              system, ti, context, depsPath, input.get(), true))
        return false;
      continue;
    }
      
    toolchain::report_fatal_error("unknown dependencies style");
  }

  return true;
}

bool ShellCommand::processMakefileDiscoveredDependencies(BuildSystem& system,
                                                         TaskInterface ti,
                                                         QueueJobContext* context,
                                                         StringRef depsPath,
                                                         toolchain::MemoryBuffer* input,
                                                         bool ignoreSubsequentOutputs) {
  // Parse the output.
  //
  // We just ignore the rule, and add any dependency that we encounter in the
  // file.
  struct DepsActions : public core::MakefileDepsParser::ParseActions {
    BuildSystem& system;
    TaskInterface ti;
    ShellCommand* command;
    StringRef depsPath;
    unsigned numErrors{0};

    DepsActions(BuildSystem& system, TaskInterface ti,
                ShellCommand* command, StringRef depsPath)
        : system(system), ti(ti), command(command), depsPath(depsPath) {}

    virtual void error(StringRef message, uint64_t position) override {
      std::string msg;
      raw_string_ostream msgStream(msg);
      msgStream << "error reading dependency file '" << depsPath << "': "
          << message << " at position " << position;
      system.getDelegate().commandHadError(command, msgStream.str());
      ++numErrors;
    }

    virtual void actOnRuleDependency(StringRef dependency,
                                     StringRef unescapedWord) override {
      if (toolchain::sys::path::is_absolute(unescapedWord)) {
        ti.discoveredDependency(BuildKey::makeNode(unescapedWord).toData());
        system.getDelegate().commandFoundDiscoveredDependency(command, unescapedWord, DiscoveredDependencyKind::Input);
        return;
      }

      // Generate absolute path
      //
      // NOTE: This is making the assumption that relative paths coming in a
      // dependency file are in relation to the explictly set working
      // directory, or the current working directory when it has not been set.
      SmallString<PATH_MAX> absPath = StringRef(command->workingDirectory);
      toolchain::sys::path::append(absPath, unescapedWord);
      toolchain::sys::fs::make_absolute(absPath);

      ti.discoveredDependency(BuildKey::makeNode(absPath).toData());
      system.getDelegate().commandFoundDiscoveredDependency(command, absPath, DiscoveredDependencyKind::Input);
    }

    virtual void actOnRuleStart(StringRef name,
                                const StringRef unescapedWord) override {}

    virtual void actOnRuleEnd() override {}
  };

  DepsActions actions(system, ti, this, depsPath);
  core::MakefileDepsParser(input->getBuffer(), actions, ignoreSubsequentOutputs).parse();
  return actions.numErrors == 0;
}

bool
ShellCommand::processDependencyInfoDiscoveredDependencies(BuildSystem& system,
                                                          TaskInterface ti,
                                                          QueueJobContext* context,
                                                          StringRef depsPath,
                                                          toolchain::MemoryBuffer* input) {
  // Parse the output.
  //
  // We just ignore the rule, and add any dependency that we encounter in the
  // file.
  struct DepsActions : public core::DependencyInfoParser::ParseActions {
    BuildSystem& system;
    TaskInterface ti;
    ShellCommand* command;
    StringRef depsPath;
    unsigned numErrors{0};

    DepsActions(BuildSystem& system, TaskInterface ti,
                ShellCommand* command, StringRef depsPath)
        : system(system), ti(ti), command(command), depsPath(depsPath) {}

    virtual void error(const char* message, uint64_t position) override {
      system.getDelegate().commandHadError(
          command, ("error reading dependency file '" + depsPath.str() +
                    "': " + std::string(message)));
      ++numErrors;
    }

    // Ignore everything but actual inputs.
    virtual void actOnVersion(StringRef) override { }
    virtual void actOnMissing(StringRef path) override {
      system.getDelegate().commandFoundDiscoveredDependency(command, path, DiscoveredDependencyKind::Missing);
    }
    virtual void actOnOutput(StringRef path) override {
      system.getDelegate().commandFoundDiscoveredDependency(command, path, DiscoveredDependencyKind::Output);
    }
    virtual void actOnInput(StringRef path) override {
      ti.discoveredDependency(BuildKey::makeNode(path).toData());
      system.getDelegate().commandFoundDiscoveredDependency(command, path, DiscoveredDependencyKind::Input);
    }
  };

  DepsActions actions(system, ti, this, depsPath);
  core::DependencyInfoParser(input->getBuffer(), actions).parse();
  return actions.numErrors == 0;
}

bool ShellCommand::configureAttribute(const ConfigureContext& ctx, StringRef name,
                                      StringRef value) {
  if (name == "args") {
    // When provided as a scalar string, we default to executing using the
    // shell.
    args.clear();
#if defined(_WIN32)
    args.push_back(ctx.getDelegate().getInternedString(
                       "C:\\windows\\system32\\cmd.exe"));
    args.push_back(ctx.getDelegate().getInternedString("/C"));
#else
    args.push_back(ctx.getDelegate().getInternedString(DefaultShellPath));
    args.push_back(ctx.getDelegate().getInternedString("-c"));
#endif
    args.push_back(ctx.getDelegate().getInternedString(value));
  } else if (name == "signature") {
    signatureData = value;
  } else if (name == "deps") {
    depsPaths.clear();
    depsPaths.emplace_back(value);
  } else if (name == "deps-style") {
    if (value == "makefile") {
      depsStyle = DepsStyle::Makefile;
    } else if (value == "dependency-info") {
      depsStyle = DepsStyle::DependencyInfo;
    } else if (value == "makefile-ignoring-subsequent-outputs") {
      depsStyle = DepsStyle::MakefileIgnoringSubsequentOutputs;
    } else {
      ctx.error("unknown 'deps-style': '" + value + "'");
      return false;
    }
    return true;
  } else if (name == "can-safely-interrupt") {
    if (value != "true" && value != "false") {
      ctx.error("invalid value: '" + value + "' for attribute '" +
                name + "'");
      return false;
    }
    canSafelyInterrupt = value == "true";
  } else if (name == "inherit-env") {
    if (value != "true" && value != "false") {
      ctx.error("invalid value: '" + value + "' for attribute '" +
                name + "'");
      return false;
    }
    inheritEnv = value == "true";
  } else if (name == "working-directory") {
    // Ensure the working directory is absolute. This will make sure any
    // relative directories are interpreted as relative to the CWD at the time
    // the rule is defined.
    SmallString<PATH_MAX> wd = value;
    toolchain::sys::fs::make_absolute(wd);
    workingDirectory = StringRef(wd);
  } else if (name == "control-enabled") {
    if (value != "true" && value != "false") {
      ctx.error("invalid value: '" + value + "' for attribute '" +
                name + "'");
      return false;
    }
    controlEnabled = value == "true";
  } else {
    return ExternalCommand::configureAttribute(ctx, name, value);
  }

  return true;
}
  
bool ShellCommand::configureAttribute(const ConfigureContext& ctx, StringRef name,
                                              ArrayRef<StringRef> values) {
  if (name == "args") {
    // Diagnose missing arguments.
    if (values.empty()) {
      ctx.error("invalid arguments for command '" + getName() + "'");
      return false;
    }

    args.clear();
    args.reserve(values.size());
    for (auto arg: values) {
      args.emplace_back(ctx.getDelegate().getInternedString(arg));
    }
  } else if (name == "deps") {
    depsPaths.clear();
    depsPaths.insert(depsPaths.begin(), values.begin(), values.end());
  } else {
    return ExternalCommand::configureAttribute(ctx, name, values);
  }

  return true;
}

bool ShellCommand::configureAttribute(
    const ConfigureContext& ctx, StringRef name,
    ArrayRef<std::pair<StringRef, StringRef>> values) {
  if (name == "env") {
    env.clear();
    env.reserve(values.size());
    for (const auto& entry: values) {
      env.emplace_back(
          std::make_pair(
              ctx.getDelegate().getInternedString(entry.first),
              ctx.getDelegate().getInternedString(entry.second)));
    }
  } else {
    return ExternalCommand::configureAttribute(ctx, name, values);
  }

  return true;
}

void ShellCommand::executeExternalCommand(
    BuildSystem& system,
    TaskInterface ti,
    QueueJobContext* context,
    toolchain::Optional<ProcessCompletionFn> completionFn) {
  auto commandCompletionFn = [this, &system, ti, completionFn](ProcessResult result) mutable {
    if (result.status != ProcessStatus::Succeeded) {
      // If the command failed, there is no need to gather dependencies.
      if (completionFn.hasValue())
        completionFn.getValue()(result);
      return;
    }

    // Collect the discovered dependencies, if used.
    if (!depsPaths.empty()) {
      ti.spawn(QueueJob{ this, [this, &system, ti, completionFn, result](QueueJobContext* context) mutable {
            if (!processDiscoveredDependencies(system, ti, context)) {
              // If we were unable to process the dependencies output, report a
              // failure.
              if (completionFn.hasValue())
                completionFn.getValue()(ProcessStatus::Failed);
              return;
            }
            if (completionFn.hasValue())
              completionFn.getValue()(result);
          }}, QueueJobPriority::High);
      return;
    }

    if (completionFn.hasValue())
      completionFn.getValue()(result);
  };
      
  // Delegate to the handler, if present.
  if (handler) {
    // FIXME: We should consider making this interface capable of feeding
    // back the dependencies directly.
    //
    // FIXME: This needs to honor certain properties of the execution queue
    // (like indicating when the work starts and stops, and communicating with
    // the execution queue delegate controlling how output is handled). It could
    // be the case that this should actually be delegating this work to run on
    // the execution queue, and the queue handles the handoff.
    handler->execute(
        handlerState.get(), this, ti, context, commandCompletionFn);
    return;
  }

  bool connectToConsole = false;

  // Execute the command.
  ti.spawn(
      context, args, env,
      {canSafelyInterrupt, connectToConsole, workingDirectory, inheritEnv, controlEnabled},
      /*completionFn=*/{commandCompletionFn});
}
