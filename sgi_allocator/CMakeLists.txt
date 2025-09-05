cmake_minimum_required(VERSION 3.14)
project(sgi_pmr_allocator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the include directory
include_directories(include)

# Create a library for the allocator
add_library(sgi_pmr_allocator src/sgi_pmr_allocator.cpp)

# Enable testing
include(CTest)
enable_testing()

# Try to find GoogleTest first
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GoogleTest not found, downloading via FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "GoogleTest found: ${GTest_DIR}")
endif()

# Try to find Google Benchmark first
find_package(benchmark QUIET)
if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark not found, downloading via FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
    )
    FetchContent_MakeAvailable(googlebenchmark)
else()
    message(STATUS "Google Benchmark found: ${benchmark_DIR}")
endif()

# Add unit tests
add_subdirectory(tests)

# Add performance tests if requested
option(BUILD_BENCHMARKS "Build performance benchmarks" ON)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Add examples
add_subdirectory(examples)
