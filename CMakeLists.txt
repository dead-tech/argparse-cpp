cmake_minimum_required(VERSION 3.18.4)
project(argparse-cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(COMPILER_WARNINGS
        -std=c++20
        -Wall
        -Wextra
        -Werror
        -Wsign-conversion
        -Wpedantic
        -Wold-style-cast
        -Wunused
        -Wnull-dereference
        -fsanitize=undefined
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS TARGETS)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "include/")
target_link_options(${PROJECT_NAME} INTERFACE ${COMPILER_WARNINGS})

# Testing
option(ENABLE_TESTING "Enable testing" ON)
if (ENABLE_TESTING)
    file(GLOB_RECURSE TESTS_SOURCES "tests/*.cpp")
    list(REMOVE_ITEM TESTS_SOURCES "${CMAKE_SOURCE_DIR}/tests/main.cpp")

    add_library(catch_main OBJECT "tests/main.cpp")
    target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)
    target_link_options(catch_main PRIVATE ${COMPILER_WARNINGS})

    add_executable(argparse-cpp_tests ${TESTS_SOURCES})
    target_link_libraries(argparse-cpp_tests PRIVATE catch_main ${PROJECT_NAME})
    target_link_options(argparse-cpp_tests PRIVATE -std=c++20)
endif ()
