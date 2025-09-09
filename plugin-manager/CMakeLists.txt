cmake_minimum_required(VERSION 3.12)
project(plugin_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Plugin manager library
add_library(plugin_manager STATIC
    src/plugin_loader.cpp
    src/plugin_metadata.cpp
)

target_include_directories(plugin_manager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(plugin_manager PUBLIC cxx_std_20)

# Set include directories for all targets in the project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# GTest setup
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Tests
add_executable(plugin_manager_tests
    test/test_plugin_manager.cpp
)

target_link_libraries(plugin_manager_tests
    plugin_manager
    GTest::gtest_main
)

target_include_directories(plugin_manager_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()
add_test(NAME plugin_manager_tests COMMAND plugin_manager_tests)

# Example plugin interface
add_library(example_plugin_interface INTERFACE)
target_include_directories(example_plugin_interface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/example
)

# Example plugins
add_library(string_plugin SHARED
    example/string_plugin.cpp
)

target_include_directories(string_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/example
)
target_link_libraries(string_plugin plugin_manager)

add_library(math_plugin SHARED
    example/math_plugin.cpp
)

target_include_directories(math_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/example
)
target_link_libraries(math_plugin plugin_manager)

# Example application
add_executable(example_app
    example/example_app.cpp
)

target_include_directories(example_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/example
)
target_link_libraries(example_app plugin_manager)
