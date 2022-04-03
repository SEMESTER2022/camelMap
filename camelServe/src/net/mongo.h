#ifndef NET_MONGO_H
#define NET_MONGO_H

#include <memory>
#include <string>

#include "bsoncxx/builder/basic/array.hpp"
#include "bsoncxx/builder/basic/document.hpp"
#include "bsoncxx/builder/basic/kvp.hpp"
#include "bsoncxx/json.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/instance.hpp"
#include "mongocxx/pool.hpp"
#include "mongocxx/uri.hpp"

#include "fmt/core.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

namespace net {

struct MongoProps {
  std::string host_name;
  std::string port;
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
  std::unique_ptr<mongocxx::instance> m_instance{};
  std::unique_ptr<mongocxx::pool> m_pool;

public:
  Mongo() {}

  bool InitMongo(MongoProps &props) {
    this->uri = props.ToUriStr();
    bool ok = uri.empty() ? false : true;
    if (ok == false) {
      return false;
    }

    this->m_pool =
        std::make_unique<mongocxx::pool>(std::move(mongocxx::uri{this->uri}));
    return true;
  }

  bool Enabled() { return this->uri.empty() ? false : true; }

  template <class Func> bool DoQuery(Func &&func) {
    auto client = m_pool->acquire();
    return func(*client);
  }
};

} // namespace net

net::Mongo &MongoInstance();

template <class Func> static inline bool MongoQueryf(Func &&func) {
  return MongoInstance().DoQuery(func);
}

#endif // NET_MONGO_H