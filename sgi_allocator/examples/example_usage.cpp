#include "../include/sgi_pmr_allocator.hpp"
#include <vector>
#include <iostream>
#include <string>

int main() {
    std::cout << "SGI PMR Allocator Example Usage\n";
    std::cout << "===============================\n\n";

    // Example 1: Using synchronized_pool_resource (thread-safe)
    std::cout << "1. Using synchronized_pool_resource (thread-safe):\n";
    sgi_pmr::synchronized_pool_resource sync_mr;
    void* sync_ptr = sync_mr.allocate(64, 8);
    std::cout << "   Allocated 64 bytes at: " << sync_ptr << std::endl;
    sync_mr.deallocate(sync_ptr, 64, 8);
    std::cout << "   Memory deallocated successfully.\n\n";

    // Example 2: Using unsynchronized_pool_resource (single-threaded, faster)
    std::cout << "2. Using unsynchronized_pool_resource (single-threaded):\n";
    sgi_pmr::unsynchronized_pool_resource unsync_mr;
    void* unsync_ptr = unsync_mr.allocate(64, 8);
    std::cout << "   Allocated 64 bytes at: " << unsync_ptr << std::endl;
    unsync_mr.deallocate(unsync_ptr, 64, 8);
    std::cout << "   Memory deallocated successfully.\n\n";

    // Example 3: Large allocation handling (both resources)
    std::cout << "3. Large allocation handling:\n";
    void* large_ptr = sync_mr.allocate(1024, 8);
    std::cout << "   Allocated 1024 bytes at: " << large_ptr << std::endl;
    sync_mr.deallocate(large_ptr, 1024, 8);
    std::cout << "   Large memory deallocated successfully.\n\n";

    // Example 4: Using with polymorphic allocator and vector
    std::cout << "4. Using with std::vector and polymorphic allocator:\n";
    sgi_pmr::polymorphic_allocator<int> sync_alloc(&sync_mr);
    std::vector<int, sgi_pmr::polymorphic_allocator<int>> numbers(sync_alloc);
    
    for (int i = 0; i < 10; ++i) {
        numbers.push_back(i * 10);
    }
    
    std::cout << "   Vector contents: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n   Vector size: " << numbers.size() << std::endl;
    std::cout << "   Vector capacity: " << numbers.capacity() << std::endl;
    std::cout << "   Memory resource used: synchronized_pool_resource\n\n";

    // Example 5: Using unsynchronized resource with container
    std::cout << "5. Using unsynchronized_pool_resource with container:\n";
    sgi_pmr::polymorphic_allocator<std::string> unsync_alloc(&unsync_mr);
    std::vector<std::string, sgi_pmr::polymorphic_allocator<std::string>> strings(unsync_alloc);
    
    for (int i = 0; i < 5; ++i) {
        strings.push_back("String " + std::to_string(i));
    }
    
    std::cout << "   Strings vector size: " << strings.size() << std::endl;
    for (const auto& str : strings) {
        std::cout << "   " << str << std::endl;
    }
    std::cout << "   Memory resource used: unsynchronized_pool_resource\n\n";

    // Example 6: Alignment handling
    std::cout << "6. Alignment handling:\n";
    void* aligned_ptr = sync_mr.allocate(128, 16);
    std::cout << "   Allocated 128 bytes with 16-byte alignment at: " << aligned_ptr << std::endl;
    std::cout << "   Address modulo 16: " << (reinterpret_cast<uintptr_t>(aligned_ptr) % 16) << " (should be 0)\n";
    sync_mr.deallocate(aligned_ptr, 128, 16);
    std::cout << "   Aligned memory deallocated successfully.\n\n";

    std::cout << "All examples completed successfully!\n";
    return 0;
}