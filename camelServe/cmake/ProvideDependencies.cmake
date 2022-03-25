find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

add_library(LibFmt ALIAS fmt::fmt-header-only)
add_library(LibSpdLog ALIAS spdlog::spdlog_header_only)
add_library(LibMongoCXX ALIAS mongo::mongocxx_static)
