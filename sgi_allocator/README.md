# SGI 风格 PMR 分配器

一个使用 C++17 的 `std::pmr`（多态内存资源）接口实现的 SGI STL 风格内存分配器，提供同步和非同步版本，类似于 `std::pmr::synchronized_pool_resource` 和 `std::pmr::unsynchronized_pool_resource`。

## 特性

- **SGI STL 启发设计**: 实现具有多个空闲列表的内存池，用于高效的小对象分配
- **双重实现**: 同时提供 `synchronized_pool_resource`（线程安全）和 `unsynchronized_pool_resource`（单线程）变体
- **多态内存资源**: 继承自 `std::pmr::memory_resource`，与标准容器兼容
- **线程安全选项**: `synchronized_pool_resource` 使用互斥锁确保线程安全操作
- **高性能选项**: `unsynchronized_pool_resource` 针对单线程性能优化，零锁定开销
- **大小类优化**: 16 个空闲列表，用于 8 到 128 字节的大小（对齐到 8 字节）
- **大对象处理**: 对于大于 128 字节的对象直接使用系统分配
- **多态分配器**: 模板包装器，用于标准容器
- **代码重用**: 通过基类共享通用功能，提高可维护性

## 要求

- C++20 兼容编译器
- CMake 3.14+
- Google Test（用于单元测试，自动获取）
- Google Benchmark（用于性能测试，自动获取）

## 构建说明

```bash
# 克隆并构建
mkdir build
cd build
cmake ..
make

# 构建包含测试和基准测试（默认）
cmake -DBUILD_BENCHMARKS=ON ..
make

# 运行测试
make test
# 或
./tests/sgi_pmr_allocator_tests

# 运行基准测试
./benchmarks/sgi_pmr_allocator_benchmarks
```

## 使用示例

### 基本用法

```cpp
#include "include/sgi_pmr_allocator.hpp"
#include <vector>
#include <iostream>

int main() {
    // 线程安全版本
    sgi_pmr::synchronized_pool_resource sync_mr;
    void* sync_ptr = sync_mr.allocate(64, 8);
    std::cout << "分配了 64 字节（线程安全）在: " << sync_ptr << std::endl;
    sync_mr.deallocate(sync_ptr, 64, 8);

    // 单线程高性能版本
    sgi_pmr::unsynchronized_pool_resource unsync_mr;
    void* unsync_ptr = unsync_mr.allocate(64, 8);
    std::cout << "分配了 64 字节（单线程）在: " << unsync_ptr << std::endl;
    unsync_mr.deallocate(unsync_ptr, 64, 8);
    
    return 0;
}
```

### 与标准容器一起使用

```cpp
#include "include/sgi_pmr_allocator.hpp"
#include <vector>
#include <string>
#include <iostream>

int main() {
    // 用于多线程应用程序
    sgi_pmr::synchronized_pool_resource sync_mr;
    std::pmr::vector<int> sync_numbers({1, 2, 3, 4, 5}, &sync_mr);
    
    // 用于单线程应用程序（更好性能）
    sgi_pmr::unsynchronized_pool_resource unsync_mr;
    std::pmr::vector<std::string> unsync_strings(&unsync_mr);
    
    // 向两个容器添加元素
    for (int i = 6; i <= 1000; ++i) {
        sync_numbers.push_back(i);
    }
    
    for (int i = 0; i < 100; ++i) {
        unsync_strings.push_back("String " + std::to_string(i));
    }
    
    std::cout << "线程安全向量大小: " << sync_numbers.size() << std::endl;
    std::cout << "单线程向量大小: " << unsync_strings.size() << std::endl;
    std::cout << "第一个字符串: " << unsync_strings[0] << std::endl;
    
    return 0;
}
```

### 多态分配器用法

```cpp
#include "include/sgi_pmr_allocator.hpp"
#include <memory_resource>
#include <vector>
#include <list>
#include <iostream>

int main() {
    // 根据线程需求选择适当的资源
    sgi_pmr::synchronized_pool_resource sync_mr; // 用于线程安全
    sgi_pmr::unsynchronized_pool_resource unsync_mr; // 用于单线程最大性能
    
    // 与不同容器类型和资源一起使用
    std::pmr::vector<int> sync_vec(&sync_mr);
    std::pmr::list<std::string> unsync_lst(&unsync_mr);
    
    for (int i = 0; i < 100; ++i) {
        sync_vec.push_back(i);
        unsync_lst.push_back("String " + std::to_string(i));
    }
    
    std::cout << "线程安全向量大小: " << sync_vec.size() << std::endl;
    std::cout << "单线程列表大小: " << unsync_lst.size() << std::endl;
    
    return 0;
}
```
