find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

if (TARGET fmt::fmt-header-only)
    add_library(LibFmt ALIAS fmt::fmt-header-only)
else()
    message(FATAL_ERROR "Could not find fmt-header-only target")
endif()

if (TARGET spdlog::spdlog_header_only)
    add_library(LibSpdLog ALIAS spdlog::spdlog_header_only)
else()
    message(FATAL_ERROR "Could not find spdlog_header_only target")
endif()

if (TARGET mongo::mongocxx_static)
    add_library(LibMongoCXX ALIAS mongo::mongocxx_static)
elseif(TARGET mongo::mongocxx_shared)
    add_library(LibMongoCXX ALIAS mongo::mongocxx_shared)
else()
    message(FATAL_ERROR "Could not find mongocxx_shared")
endif()

