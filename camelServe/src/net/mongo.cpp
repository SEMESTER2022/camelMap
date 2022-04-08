#include "net/mongo.h"

net::Mongo &MongoInstance() {
  static net::Mongo *g_mongo{new net::Mongo()};
  return *g_mongo;
}
