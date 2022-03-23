// #ifndef UTIL_FS_H
// #define UTIL_FS_H

// #include <cstdio>
// #include <experimental/filesystem>
// #include <ostream>
// #include <utility>

// namespace util {
// namespace fs = std::experimental::filesystem;
// class path : public fs::path {
// public:
//   path(fs::path path) : fs::path::path(std::move(path)) {}
//   path &operator=(fs::path path) {
//     fs::path::operator=(std::move(path));
//     return *this;
//   }
//   path &operator/=(fs::path path) {
//     fs::path::operator/=(std::move(path));
//     return *this;
//   }

//   path(const char *c) : fs::path(c) {}
//   path &operator=(const char *c) {
//     fs::path::operator=(c);
//     return *this;
//   }
//   path &operator/=(const char *c) {
//     fs::path::operator/=(c);
//     return *this;
//   }
//   path &append(const char *c) {
//     fs::path::append(c);
//     return *this;
//   }

//   path(std::string) = delete;
//   path &operator=(std::string) = delete;
//   path &operator/=(std::string) = delete;
//   path &append(std::string) = delete;

//   std::string string() const = delete;
//   path &make_preferred() {
//     fs::path::make_preferred();
//     return *this;
//   }
//   path filename() const { return fs::path::filename(); }
// };
// } // namespace util

// #endif // UTIL_FS_H
