file(READ ${CMAKE_SOURCE_DIR}/cs.env cs_env)
if (NOT cs_env MATCHES "CS_MONGO_HOST_NAME=([0-9.]+)")
    message(FATAL_ERROR "Cannot get CS_MONGO_HOST_NAME from cs.env")
endif()
set(CS_MONGO_HOST_NAME ${CMAKE_MATCH_1})

if (NOT cs_env MATCHES "CS_MONGO_PORT=([0-9]+)")
    message(FATAL_ERROR "Cannot get CS_MONGO_PORT from cs.env")
endif()
set(CS_MONGO_PORT ${CMAKE_MATCH_1})

if (NOT cs_env MATCHES "CS_MONGO_USER_NAME=([a-zA-Z0-9]+)")
    message(FATAL_ERROR "Cannot get CS_MONGO_USER_NAME from cs.env")
endif()
set(CS_MONGO_USER_NAME ${CMAKE_MATCH_1})

if (NOT cs_env MATCHES "CS_MONGO_PASSWORD=([a-zA-Z0-9]+)")
    message(FATAL_ERROR "Cannot get CS_MONGO_PASSWORD from cs.env")
endif()

if (NOT cs_env MATCHES "CS_SERVICE_THREAD_POOL_SIZE=([0-9]+)")
    message(FATAL_ERROR "Cannot get CS_SERVICE_THREAD_POOL_SIZE from cs.env")
endif()

set(CS_MONGO_PASSWORD ${CMAKE_MATCH_1})
