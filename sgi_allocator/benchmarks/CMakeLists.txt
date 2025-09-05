cmake_minimum_required(VERSION 3.12)

# Create benchmark executable
add_executable(sgi_pmr_allocator_benchmarks
    benchmark_sgi_pmr_allocator.cpp
)

# Link with Google Benchmark and our library
target_link_libraries(sgi_pmr_allocator_benchmarks
    benchmark::benchmark
    sgi_pmr_allocator
)