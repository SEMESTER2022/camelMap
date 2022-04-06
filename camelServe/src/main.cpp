
#include "spdlog/spdlog.h"

#include "cs/core.h"

int main(int argc, char *argv[]) {
  bool ok = cs::AppInit();
  if (ok == true) {
    spdlog::info("OK");
  }

  return EXIT_SUCCESS;
}
