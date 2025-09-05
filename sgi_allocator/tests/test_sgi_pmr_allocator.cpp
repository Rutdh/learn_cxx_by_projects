#include <gtest/gtest.h>
#include "../include/sgi_pmr_allocator.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <random>

using namespace sgi_pmr;

TEST(SGISynchronizedPoolResourceTest, BasicAllocationDeallocation) {
    synchronized_pool_resource mr;
    
    // 分配和释放小内存
    void* ptr1 = mr.allocate(16, 8);
    EXPECT_NE(ptr1, nullptr);
    mr.deallocate(ptr1, 16, 8);
    
    // 分配和释放大内存
    void* ptr2 = mr.allocate(256, 8);
    EXPECT_NE(ptr2, nullptr);
    mr.deallocate(ptr2, 256, 8);
}

TEST(SGIUnsynchronizedPoolResourceTest, BasicAllocationDeallocation) {
    unsynchronized_pool_resource mr;
    
    // 分配和释放小内存
    void* ptr1 = mr.allocate(16, 8);
    EXPECT_NE(ptr1, nullptr);
    mr.deallocate(ptr1, 16, 8);
    
    // 分配和释放大内存
    void* ptr2 = mr.allocate(256, 8);
    EXPECT_NE(ptr2, nullptr);
    mr.deallocate(ptr2, 256, 8);
}

TEST(SGISynchronizedPoolResourceTest, SmallObjectAllocation) {
    synchronized_pool_resource mr;
    
    // 测试各种小大小的分配
    std::vector<void*> pointers;
    
    for (std::size_t size = 8; size <= 128; size += 8) {
        void* ptr = mr.allocate(size, 8);
        EXPECT_NE(ptr, nullptr);
        pointers.push_back(ptr);
    }
    
    // 释放所有
    for (std::size_t i = 0; i < pointers.size(); ++i) {
        mr.deallocate(pointers[i], 8 + i * 8, 8);
    }
}

TEST(SGIUnsynchronizedPoolResourceTest, SmallObjectAllocation) {
    unsynchronized_pool_resource mr;
    
    // 测试各种小大小的分配
    std::vector<void*> pointers;
    
    for (std::size_t size = 8; size <= 128; size += 8) {
        void* ptr = mr.allocate(size, 8);
        EXPECT_NE(ptr, nullptr);
        pointers.push_back(ptr);
    }
    
    // 释放所有
    for (std::size_t i = 0; i < pointers.size(); ++i) {
        mr.deallocate(pointers[i], 8 + i * 8, 8);
    }
}

TEST(SGISynchronizedPoolResourceTest, LargeObjectAllocation) {
    synchronized_pool_resource mr;
    
    // 测试大对象的分配（应直接使用 malloc）
    void* ptr1 = mr.allocate(1024, 8);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = mr.allocate(2048, 16);
    EXPECT_NE(ptr2, nullptr);
    
    mr.deallocate(ptr1, 1024, 8);
    mr.deallocate(ptr2, 2048, 16);
}

TEST(SGIUnsynchronizedPoolResourceTest, LargeObjectAllocation) {
    unsynchronized_pool_resource mr;
    
    // 测试大对象的分配（应直接使用 malloc）
    void* ptr1 = mr.allocate(1024, 8);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = mr.allocate(2048, 16);
    EXPECT_NE(ptr2, nullptr);
    
    mr.deallocate(ptr1, 1024, 8);
    mr.deallocate(ptr2, 2048, 16);
}

TEST(SGISynchronizedPoolResourceTest, AlignmentRequirements) {
    synchronized_pool_resource mr;
    
    // 测试不同的对齐
    void* ptr1 = mr.allocate(64, 8);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0);
    
    void* ptr2 = mr.allocate(128, 16);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 16, 0);
    
    void* ptr3 = mr.allocate(256, 32);
    EXPECT_NE(ptr3, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr3) % 32, 0);
    
    mr.deallocate(ptr1, 64, 8);
    mr.deallocate(ptr2, 128, 16);
    mr.deallocate(ptr3, 256, 32);
}

TEST(SGIUnsynchronizedPoolResourceTest, AlignmentRequirements) {
    unsynchronized_pool_resource mr;
    
    // 测试不同的对齐
    void* ptr1 = mr.allocate(64, 8);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 8, 0);
    
    void* ptr2 = mr.allocate(128, 16);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 16, 0);
    
    void* ptr3 = mr.allocate(256, 32);
    EXPECT_NE(ptr3, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr3) % 32, 0);
    
    mr.deallocate(ptr1, 64, 8);
    mr.deallocate(ptr2, 128, 16);
    mr.deallocate(ptr3, 256, 32);
}

TEST(SGISynchronizedPoolResourceTest, IsEqualComparison) {
    synchronized_pool_resource mr1;
    synchronized_pool_resource mr2;
    
    // 相同资源应相等
    EXPECT_TRUE(mr1.is_equal(mr1));
    
    // 不同资源不应相等
    EXPECT_FALSE(mr1.is_equal(mr2));
}

TEST(SGIUnsynchronizedPoolResourceTest, IsEqualComparison) {
    unsynchronized_pool_resource mr1;
    unsynchronized_pool_resource mr2;
    
    // 相同资源应相等
    EXPECT_TRUE(mr1.is_equal(mr1));
    
    // 不同资源不应相等
    EXPECT_FALSE(mr1.is_equal(mr2));
}

TEST(SGISynchronizedPoolResourceTest, PolymorphicAllocatorUsage) {
    synchronized_pool_resource mr;
    polymorphic_allocator<int> alloc(&mr);
    
    // 测试通过多态分配器分配
    int* ptr = alloc.allocate(10);
    EXPECT_NE(ptr, nullptr);
    
    // 测试释放
    alloc.deallocate(ptr, 10);
}

TEST(SGIUnsynchronizedPoolResourceTest, PolymorphicAllocatorUsage) {
    unsynchronized_pool_resource mr;
    polymorphic_allocator<int> alloc(&mr);
    
    // 测试通过多态分配器分配
    int* ptr = alloc.allocate(10);
    EXPECT_NE(ptr, nullptr);
    
    // 测试释放
    alloc.deallocate(ptr, 10);
}

TEST(SGISynchronizedPoolResourceTest, VectorWithPolymorphicAllocator) {
    synchronized_pool_resource mr;
    
    // 使用我们的自定义分配器创建向量
    std::pmr::vector<int> vec({1, 2, 3, 4, 5}, &mr);
    
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 5);
    
    // 添加更多元素以触发重新分配
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_EQ(vec.size(), 105);
}

TEST(SGIUnsynchronizedPoolResourceTest, VectorWithPolymorphicAllocator) {
    unsynchronized_pool_resource mr;
    
    // 使用我们的自定义分配器创建向量
    std::pmr::vector<int> vec({1, 2, 3, 4, 5}, &mr);
    
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 5);
    
    // 添加更多元素以触发重新分配
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    
    EXPECT_EQ(vec.size(), 105);
}

TEST(SGISynchronizedPoolResourceTest, MemoryReuse) {
    synchronized_pool_resource mr;
    
    // 多次分配和释放以测试内存重用
    std::vector<void*> pointers;
    
    for (int i = 0; i < 100; ++i) {
        void* ptr = mr.allocate(16, 8);
        EXPECT_NE(ptr, nullptr);
        pointers.push_back(ptr);
    }
    
    for (void* ptr : pointers) {
        mr.deallocate(ptr, 16, 8);
    }
    
    // 再次分配 - 应重用空闲链表中的内存
    for (int i = 0; i < 100; ++i) {
        void* ptr = mr.allocate(16, 8);
        EXPECT_NE(ptr, nullptr);
        mr.deallocate(ptr, 16, 8);
    }
}

TEST(SGIUnsynchronizedPoolResourceTest, MemoryReuse) {
    unsynchronized_pool_resource mr;
    
    // 多次分配和释放以测试内存重用
    std::vector<void*> pointers;
    
    for (int i = 0; i < 100; ++i) {
        void* ptr = mr.allocate(16, 8);
        EXPECT_NE(ptr, nullptr);
        pointers.push_back(ptr);
    }
    
    for (void* ptr : pointers) {
        mr.deallocate(ptr, 16, 8);
    }
    
    // 再次分配 - 应重用空闲链表中的内存
    for (int i = 0; i < 100; ++i) {
        void* ptr = mr.allocate(16, 8);
        EXPECT_NE(ptr, nullptr);
        mr.deallocate(ptr, 16, 8);
    }
}

TEST(SGISynchronizedPoolResourceTest, ThreadSafety) {
    synchronized_pool_resource mr;
    constexpr int num_threads = 4;
    constexpr int allocations_per_thread = 1000;
    
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&mr, &success_count]() {
            std::vector<void*> pointers;
            pointers.reserve(allocations_per_thread);
            
            for (int j = 0; j < allocations_per_thread; ++j) {
                void* ptr = mr.allocate(16, 8);
                if (ptr != nullptr) {
                    pointers.push_back(ptr);
                }
            }
            
            success_count += pointers.size();
            
            for (void* ptr : pointers) {
                mr.deallocate(ptr, 16, 8);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(success_count, num_threads * allocations_per_thread);
}

// 非同步池资源不应在多线程环境中使用
// 但我们可以在单线程上下文中测试它
TEST(SGIUnsynchronizedPoolResourceTest, SingleThreadPerformance) {
    unsynchronized_pool_resource mr;
    constexpr int allocations_count = 4000;
    
    std::vector<void*> pointers;
    pointers.reserve(allocations_count);
    
    for (int j = 0; j < allocations_count; ++j) {
        void* ptr = mr.allocate(16, 8);
        if (ptr != nullptr) {
            pointers.push_back(ptr);
        }
    }
    
    EXPECT_EQ(pointers.size(), allocations_count);
    
    for (void* ptr : pointers) {
        mr.deallocate(ptr, 16, 8);
    }
}

TEST(SGISynchronizedPoolResourceTest, RandomAllocationPattern) {
    synchronized_pool_resource mr;
    std::mt19937 rng(42);
    std::uniform_int_distribution<std::size_t> size_dist(8, 256);
    std::uniform_int_distribution<std::size_t> align_dist(1, 3);
    
    std::vector<std::pair<void*, std::size_t>> allocations;
    
    // 执行随机分配
    for (int i = 0; i < 1000; ++i) {
        std::size_t size = size_dist(rng);
        std::size_t alignment = 1 << align_dist(rng); // 2, 4, 或 8
        
        void* ptr = mr.allocate(size, alignment);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignment, 0);
        
        allocations.emplace_back(ptr, size);
    }
    
    // 以随机顺序释放
    std::shuffle(allocations.begin(), allocations.end(), rng);
    
    for (const auto& alloc : allocations) {
        mr.deallocate(alloc.first, alloc.second, 8);
    }
}

TEST(SGIUnsynchronizedPoolResourceTest, RandomAllocationPattern) {
    unsynchronized_pool_resource mr;
    std::mt19937 rng(42);
    std::uniform_int_distribution<std::size_t> size_dist(8, 256);
    std::uniform_int_distribution<std::size_t> align_dist(1, 3);
    
    std::vector<std::pair<void*, std::size_t>> allocations;
    
    // 执行随机分配
    for (int i = 0; i < 1000; ++i) {
        std::size_t size = size_dist(rng);
        std::size_t alignment = 1 << align_dist(rng); // 2, 4, 或 8
        
        void* ptr = mr.allocate(size, alignment);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignment, 0);
        
        allocations.emplace_back(ptr, size);
    }
    
    // 以随机顺序释放
    std::shuffle(allocations.begin(), allocations.end(), rng);
    
    for (const auto& alloc : allocations) {
        mr.deallocate(alloc.first, alloc.second, 8);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}