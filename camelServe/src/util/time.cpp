#include "util/time.h"

#include <atomic>
#include <cassert>
#include <thread>

static std::atomic<int64_t> nMockTime(0);

int64_t util::GetTime() {
  int64_t mocktime = nMockTime.load(std::memory_order_relaxed);
  if (mocktime) {
    return mocktime;
  }

  time_t now = time(nullptr);
  assert(now > 0);
  return now;
}

template <typename T> static T GetSystemTime() {
  const auto now = std::chrono::duration_cast<T>(
      std::chrono::system_clock::now().time_since_epoch());
  assert(now.count() > 0);
  return now;
}

int64_t util::GetTimeMillis() {
  return int64_t{GetSystemTime<std::chrono::milliseconds>().count()};
}

int64_t util::GetTimeMicros() {
  return int64_t{GetSystemTime<std::chrono::microseconds>().count()};
}

int64_t util::GetTimeSeconds() {
  return int64_t{GetSystemTime<std::chrono::seconds>().count()};
}

template <typename T> T util::GetTime() {
  const std::chrono::seconds mocktime{
      nMockTime.load(std::memory_order_relaxed)};

  return std::chrono::duration_cast<T>(
      mocktime.count() ? mocktime : std::chrono::microseconds{GetTimeMicros()});
}

struct timeval util::MillisToTimeval(int64_t nTimeout) {
  struct timeval timeout;
  timeout.tv_sec = nTimeout / 1000;
  timeout.tv_usec = (nTimeout % 1000) * 1000;
  return timeout;
}

struct timeval util::MillisToTimeval(std::chrono::milliseconds ms) {
  return MillisToTimeval(count_milliseconds(ms));
}
