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

# Dependencies

include(cmake/CPM.cmake)

CPMAddPackage("gh:TheLartians/Format.cmake@1.8.2")

# Project structure

file(GLOB_RECURSE libsources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/libtfbsm/*.cpp")

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_library(tfbsm SHARED ${libsources})
target_include_directories(tfbsm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test_app src/app/test_app.cpp)
target_link_libraries(test_app PRIVATE tfbsm)

# Tests

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_something tests/test_something.cpp)
target_link_libraries(test_something PRIVATE tfbsm GTest::gtest_main)
add_test(NAME test_something COMMAND test_something)
