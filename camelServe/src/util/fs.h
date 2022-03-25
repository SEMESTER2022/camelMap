#ifndef UTIL_FS_H
#define UTIL_FS_H

#include "config/core.h"
#include <cstdio>
#include <ostream>
#include <utility>

// clang-format off
#if defined(CS_USE_FILESYSTEM)
#   include <filesystem>
namespace fs = std::experimental::filesystem;
#elif defined(CS_USE_EXPERIMENTAL_FILESYSTEM)
#   include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
// clang-format on

namespace util {
class path : public fs::path {
public:
  path(fs::path p) : fs::path::path(std::move(p)) {}
  path &operator=(fs::path p) {
    fs::path::operator=(std::move(p));
    return *this;
  }
  path &operator/=(fs::path p) {
    fs::path::operator/=(std::move(p));
    return *this;
  }

  path(const char *c) : fs::path(c) {}
  path &operator=(const char *c) {
    fs::path::operator=(c);
    return *this;
  }
  path &operator/=(const char *c) {
    fs::path::operator/=(c);
    return *this;
  }
  path &append(const char *c) {
    fs::path::append(c);
    return *this;
  }

  path(std::string) = delete;
  path &operator=(std::string) = delete;
  path &operator/=(std::string) = delete;
  path &append(std::string) = delete;

  std::string string() const = delete;
  path &make_preferred() {
    fs::path::make_preferred();
    return *this;
  }
  path filename() const { return fs::path::filename(); }
};
} // namespace util

#endif // UTIL_FS_H
