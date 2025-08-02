//===----- lib/Support/Error.cpp - Error and associated utilities ---------===//
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

#include "toolchain/Support/Error.h"
#include "toolchain/ADT/Twine.h"
#include "toolchain/Support/ErrorHandling.h"
#include "toolchain/Support/ManagedStatic.h"
#include <system_error>

using namespace toolchain;

namespace {

  enum class ErrorErrorCode : int {
    MultipleErrors = 1,
    InconvertibleError
  };

  // FIXME: This class is only here to support the transition to toolchain::Error. It
  // will be removed once this transition is complete. Clients should prefer to
  // deal with the Error value directly, rather than converting to error_code.
  class ErrorErrorCategory : public std::error_category {
  public:
    const char *name() const noexcept override { return "Error"; }

    std::string message(int condition) const override {
      switch (static_cast<ErrorErrorCode>(condition)) {
      case ErrorErrorCode::MultipleErrors:
        return "Multiple errors";
      case ErrorErrorCode::InconvertibleError:
        return "Inconvertible error value. An error has occurred that could "
               "not be converted to a known std::error_code. Please file a "
               "bug.";
      }
      llvm_unreachable("Unhandled error code");
    }
  };

}

static ManagedStatic<ErrorErrorCategory> ErrorErrorCat;

namespace toolchain {

void ErrorInfoBase::anchor() {}
char ErrorInfoBase::ID = 0;
char ErrorList::ID = 0;
char ECError::ID = 0;
char StringError::ID = 0;

void logAllUnhandledErrors(Error E, raw_ostream &OS, Twine ErrorBanner) {
  if (!E)
    return;
  OS << ErrorBanner;
  handleAllErrors(std::move(E), [&](const ErrorInfoBase &EI) {
    EI.log(OS);
    OS << "\n";
  });
}


std::error_code ErrorList::convertToErrorCode() const {
  return std::error_code(static_cast<int>(ErrorErrorCode::MultipleErrors),
                         *ErrorErrorCat);
}

std::error_code inconvertibleErrorCode() {
  return std::error_code(static_cast<int>(ErrorErrorCode::InconvertibleError),
                         *ErrorErrorCat);
}

Error errorCodeToError(std::error_code EC) {
  if (!EC)
    return Error::success();
  return Error(toolchain::make_unique<ECError>(ECError(EC)));
}

std::error_code errorToErrorCode(Error Err) {
  std::error_code EC;
  handleAllErrors(std::move(Err), [&](const ErrorInfoBase &EI) {
    EC = EI.convertToErrorCode();
  });
  if (EC == inconvertibleErrorCode())
    report_fatal_error(EC.message());
  return EC;
}

#if TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS
void Error::fatalUncheckedError() const {
  dbgs() << "Program aborted due to an unhandled Error:\n";
  if (getPtr())
    getPtr()->log(dbgs());
  else
    dbgs() << "Error value was Success. (Note: Success values must still be "
              "checked prior to being destroyed).\n";
  abort();
}
#endif

StringError::StringError(const Twine &S, std::error_code EC)
    : Msg(S.str()), EC(EC) {}

void StringError::log(raw_ostream &OS) const { OS << Msg; }

std::error_code StringError::convertToErrorCode() const {
  return EC;
}

Error createStringError(std::error_code EC, char const *Msg) {
  return make_error<StringError>(Msg, EC);
}

void report_fatal_error(Error Err, bool GenCrashDiag) {
  assert(Err && "report_fatal_error called with success value");
  std::string ErrMsg;
  {
    raw_string_ostream ErrStream(ErrMsg);
    logAllUnhandledErrors(std::move(Err), ErrStream, "");
  }
  report_fatal_error(ErrMsg);
}

}

#ifndef _MSC_VER
namespace toolchain {

// One of these two variables will be referenced by a symbol defined in
// toolchain-config.h. We provide a link-time (or load time for DSO) failure when
// there is a mismatch in the build configuration of the API client and TOOLCHAIN.
#if TOOLCHAIN_ENABLE_ABI_BREAKING_CHECKS
int EnableABIBreakingChecks;
#else
int DisableABIBreakingChecks;
#endif

} // end namespace toolchain
#endif
