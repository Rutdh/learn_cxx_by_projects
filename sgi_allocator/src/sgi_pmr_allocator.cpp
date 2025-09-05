#include "../include/sgi_pmr_allocator.hpp"
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <memory_resource>

namespace sgi_pmr {

// sgi_pool_resource_base 实现
sgi_pool_resource_base::sgi_pool_resource_base() {
    // 初始化所有空闲链表为 nullptr
    for (std::size_t i = 0; i < NFREELISTS; ++i) {
        free_lists[i] = nullptr;
    }
}

sgi_pool_resource_base::~sgi_pool_resource_base() {
    // 释放所有内存块
    for (char* chunk : memory_chunks) {
        std::free(chunk);
    }
    memory_chunks.clear();
}

void* sgi_pool_resource_base::allocate_impl(std::size_t bytes, std::size_t alignment) {
    // 对于大分配，直接使用系统 malloc
    if (bytes > MAX_BYTES || alignment > ALIGN) {
        void* ptr = std::malloc(bytes);
        if (!ptr) {
            throw std::bad_alloc();
        }
        return ptr;
    }

    // 向上取整到最近的 ALIGN 边界
    std::size_t rounded_bytes = round_up(bytes);
    std::size_t index = free_list_index(rounded_bytes);
    
    obj* result = free_lists[index];
    
    if (result) {
        // 从空闲链表中移除
        free_lists[index] = result->free_list_link;
        return result;
    }
    
    // 空闲链表为空，重新填充
    return refill(rounded_bytes);
}

void sgi_pool_resource_base::deallocate_impl(void* p, std::size_t bytes, std::size_t alignment) {
    if (!p) return;
    
    // 对于大分配，直接使用系统 free
    if (bytes > MAX_BYTES || alignment > ALIGN) {
        std::free(p);
        return;
    }
    
    std::size_t rounded_bytes = round_up(bytes);
    std::size_t index = free_list_index(rounded_bytes);
    
    obj* q = static_cast<obj*>(p);
    q->free_list_link = free_lists[index];
    free_lists[index] = q;
}

char* sgi_pool_resource_base::chunk_alloc(std::size_t size, int& nobjs) {
    char* result;
    std::size_t total_bytes = size * nobjs;
    std::size_t bytes_left = 0;
    
    // 尝试从内存池分配
    // 为简单起见，我们总是分配新块
    // 在更复杂的实现中，我们会维护一个内存池
    
    result = static_cast<char*>(std::malloc(total_bytes));
    if (!result) {
        // 如果 malloc 失败，尝试减少对象数量
        if (nobjs > 1) {
            nobjs /= 2;
            return chunk_alloc(size, nobjs);
        }
        throw std::bad_alloc();
    }
    
    // 存储块以供后续释放
    memory_chunks.push_back(result);
    return result;
}

void* sgi_pool_resource_base::refill(std::size_t size) {
    int nobjs = 20; // 要分配的对象数量
    
    char* chunk = chunk_alloc(size, nobjs);
    if (nobjs == 1) {
        return chunk;
    }
    
    std::size_t index = free_list_index(size);
    
    // 第一个对象将被返回
    obj* result = reinterpret_cast<obj*>(chunk);
    
    // 空闲链表应从第二个对象开始
    obj* current = reinterpret_cast<obj*>(chunk + size);
    free_lists[index] = current;
    
    // 链接第二个到最后一个对象
    for (int i = 2; i < nobjs; ++i) {
        obj* next = reinterpret_cast<obj*>(reinterpret_cast<char*>(current) + size);
        current->free_list_link = next;
        current = next;
    }
    current->free_list_link = nullptr;
    
    return result;
}

// synchronized_pool_resource 实现
synchronized_pool_resource::synchronized_pool_resource() = default;

void* synchronized_pool_resource::do_allocate(std::size_t bytes, std::size_t alignment) {
    std::lock_guard<std::mutex> lock(mutex_);
    return base_.allocate_impl(bytes, alignment);
}

void synchronized_pool_resource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
    std::lock_guard<std::mutex> lock(mutex_);
    base_.deallocate_impl(p, bytes, alignment);
}

bool synchronized_pool_resource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

// unsynchronized_pool_resource 实现
unsynchronized_pool_resource::unsynchronized_pool_resource() = default;

void* unsynchronized_pool_resource::do_allocate(std::size_t bytes, std::size_t alignment) {
    return base_.allocate_impl(bytes, alignment);
}

void unsynchronized_pool_resource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
    base_.deallocate_impl(p, bytes, alignment);
}

bool unsynchronized_pool_resource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

} // namespace sgi_pmr