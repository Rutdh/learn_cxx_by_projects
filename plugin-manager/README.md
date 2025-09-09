# C++20 插件管理器

一个现代的 C++20 插件管理器库，受 boost::dll 和 Qt 插件系统的启发，具有动态加载、元数据管理和依赖项解析功能。

## 特性

- **跨平台**: 支持 Windows、Linux 和 macOS
- **现代 C++20**: 利用最新的 C++ 标准和功能
- **类型安全**: 基于模板的插件实例化
- **元数据系统**: 具有验证功能的全面插件元数据
- **依赖项解析**: 自动依赖项检查和版本约束
- **发现机制**: 从目录自动发现插件
- **可扩展**: 易于使用自定义插件接口进行扩展

## 项目结构

```
plugin-manager/
├── include/plugin_manager/     # 公共头文件
│   ├── plugin_interface.h      # 基础插件接口
│   ├── plugin_loader.h         # 插件加载器和管理器
│   └── plugin_metadata.h       # 元数据实用工具
├── src/                        # 实现文件
│   ├── plugin_loader.cpp       # 插件加载器实现
│   └── plugin_metadata.cpp     # 元数据实用工具实现
├── test/                       # 单元测试
│   └── test_plugin_manager.cpp # 基于 GTest 的测试
├── example/                    # 示例用法
│   ├── example_plugin_interface.h # 示例插件接口
│   ├── string_plugin.cpp       # 字符串操作插件
│   ├── math_plugin.cpp         # 数学运算插件
│   └── example_app.cpp         # 示例应用程序
└── CMakeLists.txt              # CMake 构建配置
```

## 要求

- C++20 兼容编译器 (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.12+
- Google Test (由 CMake 自动获取)

## 构建

```bash
# 创建构建目录
mkdir build
cd build

# 使用 CMake 配置
cmake ..

# 构建项目
cmake --build .

# 运行测试
ctest
```

## 用法

### 基本插件接口

通过实现 `IPlugin` 接口创建插件：

```cpp
#include "plugin_manager/plugin_interface.h"

class MyPlugin : public plugin_manager::IPlugin {
public:
    PluginMetadata getMetadata() const override {
        return {"MyPlugin", "1.0.0", "Description", "Author", "License"};
    }
    
    bool initialize() override { return true; }
    void shutdown() override {}
    std::string getId() const override { return "my_plugin"; }
};
```

### 加载插件

```cpp
#include "plugin_manager/plugin_loader.h"

plugin_manager::PluginManager manager;
auto plugin_lib = manager.loadPlugin("path/to/plugin.so");
auto plugin = plugin_lib->createInstance<MyPlugin>();
```

### 插件元数据

插件可以使用 `PLUGIN_METADATA` 宏导出元数据：

```cpp
PLUGIN_METADATA({
    "PluginName",
    "1.0.0",
    "Plugin description",
    "Author Name",
    "MIT"
})
```

### 插件元数据的作用

插件元数据是插件系统的核心组成部分，它为插件提供了自我描述的能力，具有以下重要作用：

1. **插件识别与发现**：元数据包含插件名称、版本、描述等信息，使系统能够识别和区分不同的插件，无需加载插件即可了解其基本功能。

2. **版本管理与兼容性**：通过版本信息，系统可以检查插件与应用程序或其他插件的兼容性，支持语义版本控制和版本约束检查。

3. **依赖项解析**：元数据可以声明插件依赖的其他插件及其版本要求，系统可以自动解析这些依赖关系，确保所有必需插件可用且版本兼容。

4. **授权与许可管理**：许可证信息帮助用户了解插件的使用条款，确保合规使用。

5. **插件验证与过滤**：系统可以根据元数据验证插件的完整性和有效性，过滤掉不兼容或不可用的插件。

6. **用户界面集成**：元数据中的描述信息可以直接用于用户界面显示，为用户提供插件选择和管理的友好体验。

7. **自动化部署**：在大型系统中，元数据支持自动化工具进行插件的发现、安装、更新和卸载。

### 示例插件

项目包含两个示例插件：

1. **字符串插件**: 提供字符串操作功能（大写/小写）
2. **数学插件**: 执行基本数学运算

运行示例应用程序查看它们的作用：

```bash
./example_app
```

## API 参考

### PluginManager

- `loadPlugin(path)`: 从指定路径加载插件
- `unloadPlugin(path)`: 卸载特定插件
- `unloadAllPlugins()`: 卸载所有已加载的插件
- `discoverPlugins(directory, pattern)`: 从目录发现并加载插件

### PluginLibrary

- `getMetadata()`: 检索插件元数据
- `createInstance<InterfaceType>()`: 创建类型化插件实例
- `isValid()`: 检查库是否成功加载
- `getPath()`: 获取库文件路径

### MetadataUtils

- `validateMetadata(metadata)`: 验证插件元数据
- `checkVersionCompatibility(v1, v2)`: 检查两个版本是否兼容
- `checkVersionConstraint(version, constraint)`: 检查版本是否满足约束
- `parseVersion(version)`: 将版本字符串解析为组件
- `compareVersions(v1, v2)`: 比较两个版本字符串

### DependencyResolver

- `checkDependencies(metadata, available_plugins)`: 检查所有依赖项是否满足
- `getMissingDependencies(metadata, available_plugins)`: 获取缺失的依赖项消息

## 版本约束

插件管理器支持带有约束的语义版本控制：

- `>=1.0.0`: 大于或等于版本
- `>1.0.0`: 大于版本
- `<=1.0.0`: 小于或等于版本
- `<1.0.0`: 小于版本
- `==1.0.0`: 完全等于版本
- `~1.0.0`: 大约等效于版本（相同的主要和次要版本）

## 平台支持

- **Windows**: `.dll` 文件
- **Linux**: `.so` 文件
- **macOS**: `.dylib` 文件

## 与 boost::dll 和 Qt 插件系统的比较

本项目参考了 boost::dll 和 Qt 插件系统的优点，并结合现代 C++20 特性进行了实现。以下是详细的比较和分析：

### 借鉴的优点

**来自 boost::dll 的优点：**
- **跨平台支持**：像 boost::dll 一样，本库支持 Windows、Linux 和 macOS 的动态库加载
- **类型安全**：采用模板化的实例创建方式，确保类型安全
- **低级别控制**：提供对动态库加载和符号查找的直接控制

**来自 Qt 插件系统的优点：**
- **元数据系统**：借鉴了 Qt 的插件元数据概念，支持丰富的插件信息描述
- **接口导向**：采用明确的接口定义，插件必须实现特定接口
- **依赖管理**：引入了版本约束和依赖项解析机制

### 优势特性

1. **现代 C++20 特性**：使用 concepts、smart pointers 等现代特性，代码更简洁安全
2. **轻量级设计**：相比 Qt 插件系统的庞大，本库更加轻量，易于集成
3. **标准化构建**：使用现代 CMake 和 FetchContent，依赖管理更简单
4. **全面的测试覆盖**：包含完整的 Google Test 单元测试套件

### 当前局限性

1. **功能范围**：相比 boost::dll，缺少一些高级功能如符号重命名、自定义加载策略
2. **生态系统**：不如 Qt 插件系统那样有完整的工具链和 IDE 支持
3. **线程安全**：当前实现不是线程安全的，需要在多线程环境中额外处理
4. **错误处理**：错误处理相对简单，缺乏详细的错误分类和恢复机制
