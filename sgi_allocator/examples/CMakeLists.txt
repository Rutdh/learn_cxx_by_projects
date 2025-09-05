cmake_minimum_required(VERSION 3.12)

# Create example executable
add_executable(sgi_pmr_example
    example_usage.cpp
)

# Link with our library
target_link_libraries(sgi_pmr_example
    sgi_pmr_allocator
)