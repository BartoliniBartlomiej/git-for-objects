cmake_minimum_required(VERSION 3.20)
project(versioned-objects VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VERSIONED_BUILD_TESTS "Build unit tests" ON)

add_library(versioned INTERFACE)
add_library(versioned::versioned ALIAS versioned)

target_include_directories(versioned INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(versioned INTERFACE cxx_std_20)

if(VERSIONED_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()