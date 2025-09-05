#include <benchmark/benchmark.h>
#include "../include/sgi_pmr_allocator.hpp"
#include <memory_resource>
#include <vector>
#include <random>

using namespace sgi_pmr;

// synchronized_pool_resource 的基准测试
static void BM_SynchronizedPoolResource_SmallAllocations(benchmark::State& state) {
    synchronized_pool_resource mr;
    
    for (auto _ : state) {
        std::vector<void*> pointers;
        pointers.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            void* ptr = mr.allocate(16, 8);
            benchmark::DoNotOptimize(ptr);
            pointers.push_back(ptr);
        }
        
        for (void* ptr : pointers) {
            mr.deallocate(ptr, 16, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_SynchronizedPoolResource_SmallAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// unsynchronized_pool_resource 的基准测试
static void BM_UnsynchronizedPoolResource_SmallAllocations(benchmark::State& state) {
    unsynchronized_pool_resource mr;
    
    for (auto _ : state) {
        std::vector<void*> pointers;
        pointers.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            void* ptr = mr.allocate(16, 8);
            benchmark::DoNotOptimize(ptr);
            pointers.push_back(ptr);
        }
        
        for (void* ptr : pointers) {
            mr.deallocate(ptr, 16, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_UnsynchronizedPoolResource_SmallAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// std::pmr::synchronized_pool_resource 的基准测试
static void BM_StdSynchronizedPoolResource_SmallAllocations(benchmark::State& state) {
    std::pmr::synchronized_pool_resource mr;
    
    for (auto _ : state) {
        std::vector<void*> pointers;
        pointers.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            void* ptr = mr.allocate(16, 8);
            benchmark::DoNotOptimize(ptr);
            pointers.push_back(ptr);
        }
        
        for (void* ptr : pointers) {
            mr.deallocate(ptr, 16, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdSynchronizedPoolResource_SmallAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// std::pmr::unsynchronized_pool_resource 的基准测试
static void BM_StdUnsynchronizedPoolResource_SmallAllocations(benchmark::State& state) {
    std::pmr::unsynchronized_pool_resource mr;
    
    for (auto _ : state) {
        std::vector<void*> pointers;
        pointers.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            void* ptr = mr.allocate(16, 8);
            benchmark::DoNotOptimize(ptr);
            pointers.push_back(ptr);
        }
        
        for (void* ptr : pointers) {
            mr.deallocate(ptr, 16, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_StdUnsynchronizedPoolResource_SmallAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// 默认内存资源的基准测试
static void BM_DefaultMemoryResource_SmallAllocations(benchmark::State& state) {
    std::pmr::memory_resource* mr = std::pmr::get_default_resource();
    
    for (auto _ : state) {
        std::vector<void*> pointers;
        pointers.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            void* ptr = mr->allocate(16, 8);
            benchmark::DoNotOptimize(ptr);
            pointers.push_back(ptr);
        }
        
        for (void* ptr : pointers) {
            mr->deallocate(ptr, 16, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_DefaultMemoryResource_SmallAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// synchronized_pool_resource 的混合分配基准测试
static void BM_SynchronizedPoolResource_MixedAllocations(benchmark::State& state) {
    synchronized_pool_resource mr;
    std::mt19937 rng(42);
    std::uniform_int_distribution<std::size_t> size_dist(8, 256);
    std::uniform_int_distribution<std::size_t> align_dist(1, 3);
    
    for (auto _ : state) {
        std::vector<std::pair<void*, std::size_t>> allocations;
        allocations.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            std::size_t size = size_dist(rng);
            std::size_t alignment = 1 << align_dist(rng); // 2, 4, or 8
            
            void* ptr = mr.allocate(size, alignment);
            benchmark::DoNotOptimize(ptr);
            allocations.emplace_back(ptr, size);
        }
        
        for (const auto& alloc : allocations) {
            mr.deallocate(alloc.first, alloc.second, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_SynchronizedPoolResource_MixedAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// unsynchronized_pool_resource 的混合分配基准测试
static void BM_UnsynchronizedPoolResource_MixedAllocations(benchmark::State& state) {
    unsynchronized_pool_resource mr;
    std::mt19937 rng(42);
    std::uniform_int_distribution<std::size_t> size_dist(8, 256);
    std::uniform_int_distribution<std::size_t> align_dist(1, 3);
    
    for (auto _ : state) {
        std::vector<std::pair<void*, std::size_t>> allocations;
        allocations.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            std::size_t size = size_dist(rng);
            std::size_t alignment = 1 << align_dist(rng); // 2, 4, or 8
            
            void* ptr = mr.allocate(size, alignment);
            benchmark::DoNotOptimize(ptr);
            allocations.emplace_back(ptr, size);
        }
        
        for (const auto& alloc : allocations) {
            mr.deallocate(alloc.first, alloc.second, 8);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_UnsynchronizedPoolResource_MixedAllocations)->Arg(100)->Arg(1000)->Arg(10000);

// 使用 synchronized_pool_resource 的多态分配器向量基准测试
static void BM_PolymorphicAllocatorVector_Synchronized(benchmark::State& state) {
    synchronized_pool_resource mr;
    polymorphic_allocator<int> alloc(&mr);
    
    for (auto _ : state) {
        std::vector<int, polymorphic_allocator<int>> vec(alloc);
        for (int i = 0; i < state.range(0); ++i) {
            vec.push_back(i);
            benchmark::DoNotOptimize(vec.data());
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_PolymorphicAllocatorVector_Synchronized)->Arg(100)->Arg(1000)->Arg(10000);

// 使用 unsynchronized_pool_resource 的多态分配器向量基准测试
static void BM_PolymorphicAllocatorVector_Unsynchronized(benchmark::State& state) {
    unsynchronized_pool_resource mr;
    polymorphic_allocator<int> alloc(&mr);
    
    for (auto _ : state) {
        std::vector<int, polymorphic_allocator<int>> vec(alloc);
        for (int i = 0; i < state.range(0); ++i) {
            vec.push_back(i);
            benchmark::DoNotOptimize(vec.data());
        }
    }
    
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_PolymorphicAllocatorVector_Unsynchronized)->Arg(100)->Arg(1000)->Arg(10000);

BENCHMARK_MAIN();