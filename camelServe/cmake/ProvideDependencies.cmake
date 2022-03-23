find_package(fmt CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

add_library(LibFmt ALIAS fmt::fmt)
add_library(LibMongoCXX ALIAS mongo::mongocxx_shared)
