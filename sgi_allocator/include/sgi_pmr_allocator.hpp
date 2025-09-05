#pragma once

#include <memory_resource>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

namespace sgi_pmr {

/**
 * @brief SGI风格内存池资源基类
 *
 * 此类包含内存池的通用功能，
 * 但不直接继承自 std::pmr::memory_resource。
 */
class sgi_pool_resource_base {
protected:
    // 空闲链表节点结构
    union obj {
        union obj* free_list_link;
        char client_data[1];
    };

    // 不同大小类的空闲链表数量
    static constexpr std::size_t NFREELISTS = 16;
    
    // 对齐要求
    static constexpr std::size_t ALIGN = 8;
    
    // 小对象分配的最大大小
    static constexpr std::size_t MAX_BYTES = 128;

    // 空闲链表数组
    obj* free_lists[NFREELISTS];
    
    // 内存池块
    std::vector<char*> memory_chunks;

    /**
     * @brief 向上取整到最近的 ALIGN 倍数
     */
    static std::size_t round_up(std::size_t bytes) {
        return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
    }

    /**
     * @brief 获取给定大小的空闲链表索引
     */
    static std::size_t free_list_index(std::size_t bytes) {
        return ((bytes + ALIGN - 1) / ALIGN - 1);
    }

    /**
     * @brief 为空闲链表分配内存块
     */
    char* chunk_alloc(std::size_t size, int& nobjs);

    /**
     * @brief 重新填充空闲链表
     */
    void* refill(std::size_t size);

public:
    sgi_pool_resource_base();
    virtual ~sgi_pool_resource_base();

    sgi_pool_resource_base(const sgi_pool_resource_base&) = delete;
    sgi_pool_resource_base& operator=(const sgi_pool_resource_base&) = delete;

    /**
     * @brief 分配实现
     */
    void* allocate_impl(std::size_t bytes, std::size_t alignment);

    /**
     * @brief 释放实现
     */
    void deallocate_impl(void* p, std::size_t bytes, std::size_t alignment);
};

/**
 * @brief 线程安全的同步池资源
 *
 * 此资源使用互斥锁确保线程安全。
 */
class synchronized_pool_resource : public std::pmr::memory_resource {
private:
    sgi_pool_resource_base base_;
    std::mutex mutex_;

public:
    synchronized_pool_resource();
    ~synchronized_pool_resource() override = default;

    synchronized_pool_resource(const synchronized_pool_resource&) = delete;
    synchronized_pool_resource& operator=(const synchronized_pool_resource&) = delete;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};

/**
 * @brief 非线程安全的非同步池资源
 *
 * 此资源不使用锁，适用于单线程使用。
 */
class unsynchronized_pool_resource : public std::pmr::memory_resource {
private:
    sgi_pool_resource_base base_;

public:
    unsynchronized_pool_resource();
    ~unsynchronized_pool_resource() override = default;

    unsynchronized_pool_resource(const unsynchronized_pool_resource&) = delete;
    unsynchronized_pool_resource& operator=(const unsynchronized_pool_resource&) = delete;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};

/**
 * @brief 使用SGI内存资源的多态分配器
 */
template <typename T>
class polymorphic_allocator {
private:
    std::pmr::memory_resource* mr_;

public:
    using value_type = T;

    polymorphic_allocator() noexcept : mr_(std::pmr::get_default_resource()) {}
    explicit polymorphic_allocator(std::pmr::memory_resource* mr) noexcept : mr_(mr) {}
    
    template <typename U>
    polymorphic_allocator(const polymorphic_allocator<U>& other) noexcept 
        : mr_(other.resource()) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(mr_->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        mr_->deallocate(p, n * sizeof(T), alignof(T));
    }

    std::pmr::memory_resource* resource() const noexcept { return mr_; }

    template <typename U>
    bool operator==(const polymorphic_allocator<U>& other) const noexcept {
        return mr_ == other.resource();
    }

    template <typename U>
    bool operator!=(const polymorphic_allocator<U>& other) const noexcept {
        return !(*this == other);
    }
};

} // namespace sgi_pmr