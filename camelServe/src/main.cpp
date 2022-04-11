
#include "spdlog/spdlog.h"

#include "cs/core.h"

int main(int argc, char *argv[]) {
  bool ok = cs::init();
  if (ok == true) {
    spdlog::info("Start application sucessfully!");
  }

  cs::mainloop();

  cs::shutdown();

  return EXIT_SUCCESS;
}
