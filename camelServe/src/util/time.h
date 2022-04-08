#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <chrono>

#include "util/compat.h"

namespace util {
int64_t GetTime();

constexpr int64_t count_seconds(std::chrono::seconds t) { return t.count(); }
constexpr int64_t count_milliseconds(std::chrono::milliseconds t) {
  return t.count();
}
constexpr int64_t count_microseconds(std::chrono::microseconds t) {
  return t.count();
}

int64_t GetTimeMillis();
int64_t GetTimeMicros();
int64_t GetTimeSeconds();

template <typename T> T GetTime();

struct timeval MillisToTimeval(int64_t nTimeout);

struct timeval MillisToTimeval(std::chrono::milliseconds ms);
} // namespace util

#endif // UTIL_TIME_H
