//===- Support/Chrono.cpp - Utilities for Timing Manipulation ---*- C++ -*-===//
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

#include "toolchain/Support/Chrono.h"
#include "toolchain/Config/toolchain-config.h"
#include "toolchain/Support/Format.h"
#include "toolchain/Support/raw_ostream.h"

namespace toolchain {

using namespace sys;

const char toolchain::detail::unit<std::ratio<3600>>::value[] = "h";
const char toolchain::detail::unit<std::ratio<60>>::value[] = "m";
const char toolchain::detail::unit<std::ratio<1>>::value[] = "s";
const char toolchain::detail::unit<std::milli>::value[] = "ms";
const char toolchain::detail::unit<std::micro>::value[] = "us";
const char toolchain::detail::unit<std::nano>::value[] = "ns";

static inline struct tm getStructTM(TimePoint<> TP) {
  struct tm Storage;
  std::time_t OurTime = toTimeT(TP);

#if defined(TOOLCHAIN_ON_UNIX)
  struct tm *LT = ::localtime_r(&OurTime, &Storage);
  assert(LT);
  (void)LT;
#endif
#if defined(_WIN32)
  int Error = ::localtime_s(&Storage, &OurTime);
  assert(!Error);
  (void)Error;
#endif

  return Storage;
}

raw_ostream &operator<<(raw_ostream &OS, TimePoint<> TP) {
  struct tm LT = getStructTM(TP);
  char Buffer[sizeof("YYYY-MM-DD HH:MM:SS")];
  strftime(Buffer, sizeof(Buffer), "%Y-%m-%d %H:%M:%S", &LT);
  return OS << Buffer << '.'
            << format("%.9lu",
                      long((TP.time_since_epoch() % std::chrono::seconds(1))
                               .count()));
}

void format_provider<TimePoint<>>::format(const TimePoint<> &T, raw_ostream &OS,
                                          StringRef Style) {
  using namespace std::chrono;
  TimePoint<seconds> Truncated = time_point_cast<seconds>(T);
  auto Fractional = T - Truncated;
  struct tm LT = getStructTM(Truncated);
  // Handle extensions first. strftime mangles unknown %x on some platforms.
  if (Style.empty()) Style = "%Y-%m-%d %H:%M:%S.%N";
  std::string Format;
  raw_string_ostream FStream(Format);
  for (unsigned I = 0; I < Style.size(); ++I) {
    if (Style[I] == '%' && Style.size() > I + 1) switch (Style[I + 1]) {
        case 'L':  // Milliseconds, from Ruby.
          FStream << toolchain::format(
              "%.3lu", (long)duration_cast<milliseconds>(Fractional).count());
          ++I;
          continue;
        case 'f':  // Microseconds, from Python.
          FStream << toolchain::format(
              "%.6lu", (long)duration_cast<microseconds>(Fractional).count());
          ++I;
          continue;
        case 'N':  // Nanoseconds, from date(1).
          FStream << toolchain::format(
              "%.6lu", (long)duration_cast<nanoseconds>(Fractional).count());
          ++I;
          continue;
        case '%':  // Consume %%, so %%f parses as (%%)f not %(%f)
          FStream << "%%";
          ++I;
          continue;
      }
    FStream << Style[I];
  }
  FStream.flush();
  char Buffer[256];  // Should be enough for anywhen.
  size_t Len = strftime(Buffer, sizeof(Buffer), Format.c_str(), &LT);
  OS << (Len ? Buffer : "BAD-DATE-FORMAT");
}

} // namespace toolchain
