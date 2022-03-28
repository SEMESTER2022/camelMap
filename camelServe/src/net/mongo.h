#ifndef NET_MONGO_H
#define NET_MONGO_H

#include <function>
#include <optional>
#include <string>

#include "bsoncxx/builder/stream/array.hpp"
#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/builder/stream/helpers.hpp"
#include "bsoncxx/json.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/instance.hpp"
#include "mongocxx/stdx.hpp"
#include "mongocxx/uri.hpp"

#include "fmt/core.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

namespace net {

struct MongoProps {
  std::string host_name;
  uint16_t port{0};
  std::string user_name;
  std::string password;

  std::string ToUriStr() {
    if (this->user_name.empty() && this->password.empty()) {
      return fmt::format("mongodb://{}:{}", this->host_name, this->port);
    }

    return this->host_name.empty() || this->port.empty() ||
                   this->user_name.empty() || this->password.empty()
               ? ""
               : fmt::format("mongodb://{}:{}@{}:{}", this->user_name,
                             this->password, this->host_name, this->port);
  }
};

class Mongo {
private:
  std::string uri{""};
  mongocxx::instance m_instance{};
  mongocxx::pool m_pool;

public:
  Mongo() {}

  bool InitMongo(MongoProps &props) {
    this->uri = props.ToUriStr();
    bool ok = uri->empty() ? false : true;
    if (ok == false) {
      return false;
    }

    this->m_pool = std::move(mongocxx::uri{this->uri});
    return true;
  }

  bool Enabled() { return this->uri->empty() ? false : true; }

  template <class Func, class... Args>
  auto DoQuery(Func &&func, Args &&... args) {
    auto client = pool.acquire();
    return func(*client, args);
  }
};

} // namespace net

net::Mongo &MongoInstance();

template <class Func, class... Args>
static inline auto MongoSearchf(Func &&func, Args &&... args) {
  if (MongoInstance().Enabled()) {
    return MongoInstance().DoQuery(std::move(func), std::move(args));
  }

  return std::nullopt
}

#endif // NET_MONGO_H