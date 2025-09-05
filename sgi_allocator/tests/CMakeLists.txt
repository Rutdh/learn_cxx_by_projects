cmake_minimum_required(VERSION 3.12)

# Create test executable
add_executable(sgi_pmr_allocator_tests
    test_sgi_pmr_allocator.cpp
)

# Link with GoogleTest and our library
target_link_libraries(sgi_pmr_allocator_tests
    gtest_main
    sgi_pmr_allocator
)

# Add test
add_test(NAME sgi_pmr_allocator_tests
    COMMAND sgi_pmr_allocator_tests
)