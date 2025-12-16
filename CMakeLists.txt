cmake_minimum_required(VERSION 3.20)

project(
    "Time-Fractional Black-Scholes model option pricing"
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CTEST_OUTPUT_ON_FAILURE 1)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
        FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif()

include(FetchContent)

# Dependencies

include(cmake/CPM.cmake)

# Format
CPMAddPackage("gh:TheLartians/Format.cmake@1.8.2")

# ZeroMQ find_package(ZeroMQ REQUIRED) # This means, that 0mq must be installed with apt install
# libzmq5 or # brew install zeromq

set(ENABLE_CLANG OFF) # CMakeLists.txt in libzmq, line 1899
set(LIBZMQ_BUILD_TESTS OFF)
set(CPPZMQ_BUILD_TESTS OFF)

# Fetch and build libzmq
FetchContent_Declare(
    libzmq
    GIT_REPOSITORY https://github.com/zeromq/libzmq
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libzmq
    GIT_TAG 7a7bfa10e6b0e99210ed9397369b59f9e69cef8e # Master
)
FetchContent_MakeAvailable(libzmq)

FetchContent_Declare(
    cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cppzmq
    GIT_TAG 8bace31b3db612d7c5ad7fd01b46f4c2c8a3a7e3 # Master
)

FetchContent_GetProperties(cppzmq)
if(NOT cppzmq_POPULATED)
    FetchContent_MakeAvailable(cppzmq)
    # add_subdirectory(${cppzmq_SOURCE_DIR} ${cppzmq_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# JSON
FetchContent_Declare(
    json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Project structure
file(GLOB_RECURSE libsources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/libtfbsm/*.cpp")

add_compile_options(-Wall -Wpedantic)
add_link_options(-Wall -Wpedantic)

option(ENABLE_ASAN "Enable address santitizer" ON)
if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

add_library(tfbsm STATIC ${libsources})
target_include_directories(
    tfbsm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/cppzmq
)
target_link_libraries(tfbsm PUBLIC cppzmq nlohmann_json::nlohmann_json)

add_executable(test_app src/app/test_app.cpp)
target_link_libraries(test_app PRIVATE tfbsm)

add_executable(test_feeder src/app/test_feeder.cpp)
target_link_libraries(test_feeder PRIVATE tfbsm)

# Tests

include(CTest)
enable_testing()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_something tests/test_something.cpp)
target_link_libraries(test_something PRIVATE tfbsm GTest::gtest_main)
add_test(NAME test_something COMMAND test_something)
