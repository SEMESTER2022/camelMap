cmake_minimum_required(VERSION 3.22)

include(CMakeParseArguments)
function(join result_var)
    set(result "")
    foreach(arg ${ARGN})
        set(result ${result}${arg})
    endforeach()
    set(${result_var} "${result}" PARENT_SCOPE)
endfunction()

function(set_verbose)
    list(GET ARGN 0 var)
    list(REMOVE_AT ARGN 0)
    list(GET ARGN 0 val)
    list(REMOVE_AT ARGN 0)
    list(REMOVE_AT ARGN 0)
    list(GET ARGN 0 type)
    list(REMOVE_AT ARGN 0)
    join(doc ${ARGN})
    message(STATUS ${doc})
    set(${var} ${val} CACHE ${type} ${doc})
endfunction()

if (NOT CMAKE_BUILD_TYPE)
    set_verbose(CMAKE_BUILD_TYPE "Debug" CACHE STRING
                "Choose build type, all options available include"
                "``Debug``, ``Release``, ``RelWithDebInfo`` and ``MinSizeRel``")
endif()

set(CS_DEBUG_MODE OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CS_DEBUG_MODE ON)
endif()

option(CS_PEDANTIC "Enable extra warning using standard language flags" OFF)
option(CS_WERROR "Halt compiler warning into errors" OFF)
option(CS_TEST "Generate test target module when debug" ${CS_DEBUG_MODE})
option(CS_USE_EXTERNAL_VCPKG "Using built in vcpkg" OFF)

if (NOT CS_USE_EXTERNAL_VCPKG)
    set_verbose(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING
                "vcpkg tool chain file - crossplatform c++ package manager "
                "https://github.com/microsoft/vcpkg")
else()
    if (NOT CMAKE_TOOLCHAIN_FILE)
        message(FATAL_ERROR "Not found tool chain file given")
    endif()
endif()

project(CS CXX)
set(CS_VERSION_MAJOR 1)
set(CS_VERSION_MINOR 0)
set(CS_VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CheckCXXCompilerFlag)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CS_PEDANTIC_FLAGS -pedantic -Wall -pedantic-errors -Werror
        -Wold-style-cast -Wundef
        -Wredundant-decls -Wwrite-strings -Wpointer-arith
        -Wcast-qual -Wformat=2 -Wmissing-include-dirs
        -Wcast-align
        -Wctor-dtor-privacy -Wdisabled-optimization
        -Winvalid-pch -Woverloaded-virtual
        -Wconversion -Wundef
        -Wno-ctor-dtor-privacy -Wno-format-nonliteral)
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6)
        set(CS_PEDANTIC_FLAGS ${CS_PEDANTIC_FLAGS}
            -Wno-dangling-else -Wno-unused-local-typedefs)
    endif()
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
        set(CS_PEDANTIC_FLAGS ${CS_PEDANTIC_FLAGS} -Wdouble-promotion
            -Wtrampolines -Wzero-as-null-pointer-constant -Wuseless-cast
            -Wvector-operation-performance -Wsized-deallocation -Wshadow)
    endif()
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
        set(CS_PEDANTIC_FLAGS ${CS_PEDANTIC_FLAGS} -Wshift-overflow=2
            -Wnull-dereference -Wduplicated-cond)
        check_cxx_compiler_flag(-Wzero-as-null-pointer-constant HAS_NULLPTR_WARNING)
        if (HAS_NULLPTR_WARNING)
            set(CS_PEDANTIC_FLAGS ${CS_PEDANTIC_FLAGS}
                -Wzero-as-null-pointer-constant)
        endif()
    endif()
    set(CS_WERROR_FLAGS -Werror)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CS_PEDANTIC_FLAGS -Wall -Wextra -pedantic -Wconversion -Wundef
        -Wdeprecated -Wweak-vtables -Wshadow
        -Wno-gnu-zero-variadic-macro-arguments)
    set(CS_WERROR_FLAGS -Werror)
endif()

if (MSVC)
    set(CS_PEDANTIC_FLAGS /W3)
    set(CS_WERROR_FLAGS /WX)
endif()

include(GNUInstallDirs)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

include(ProvideDependencies)

file(GLOB CS_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.cpp)
list(SORT CS_SOURCES_CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (CS_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/share")
else()
    set (CS_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
endif()

add_library(cs ${CS_SOURCES_CXX})
target_include_directories(cs PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(cs PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>")

if (CS_PEDANTIC)
    target_compile_options(cs PUBLIC ${CS_PEDANTIC_FLAGS})
endif()
if (CS_WERROR)
    target_compile_options(cs PUBLIC ${CS_WERROR_FLAGS})
endif()
if (BUILD_SHARED_LIBS)
    target_compile_definitions(cs PRIVATE CS_EXPORT INTERFACE CS_SHARED)
endif()

# linking all dependencies library
target_link_libraries(cs PUBLIC LibFmt)
target_link_libraries(cs PUBLIC LibSpdLog)
target_link_libraries(cs PUBLIC LibMongoCXX)

if (WIN32)
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
    target_link_libraries(cs PUBLIC wsock32 ws2_32)
endif()

# main program
add_executable(camelServe src/main.cpp)
target_link_libraries(camelServe PRIVATE cs)

# import secret configurations
include(ImportConfig)
configure_file(config.h.in config.h)

install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PATTERN "*.cpp" EXCLUDE)

install(DIRECTORY share/ DESTINATION ${CMAKE_INSTALL_DATADIR})

install(TARGETS cs
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(TARGETS camelServe
        DESTINATION ${CMAKE_INSTALL_BINDIR})

if (CS_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    include(InstallRequiredSystemLibraries)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_VERSION_MAJOR "${CS_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${CS_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${CS_VERSION_PATCH}")
    include(CPack)
endif()
