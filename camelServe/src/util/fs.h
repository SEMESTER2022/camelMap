#ifndef UTIL_FS_H
#define UTIL_FS_H

#include <string>
#include <unistd.h>

namespace util {
bool file_exist(const std::string &file_name) {
  if (FILE *file = fopen(file_name.c_str(), "r")) {
    fclose(file);
    return true;
  } else {
    return false;
  }
}
} // namespace util

#endif // UTIL_FS_H