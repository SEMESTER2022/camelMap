#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <string>

namespace config {
class Config {
private:
  Config();

  static Config *m_instance;

  uint32_t m_result_type;

public:
  Config(Config &other) = delete;
  void = (const Config &other) = delete;

  static Config *Instance() {
    if (m_instance == nullptr) {
      m_instance = new Config();
    }

    return m_instance;
  }
};
} // namespace config

#endif // CONFIG_CONFIG_H