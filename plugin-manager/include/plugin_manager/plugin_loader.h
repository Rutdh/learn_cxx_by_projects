#ifndef PLUGIN_MANAGER_PLUGIN_LOADER_H
#define PLUGIN_MANAGER_PLUGIN_LOADER_H

#include "plugin_interface.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <system_error>
#include <filesystem>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace plugin_manager {

namespace fs = std::filesystem;

/**
 * @brief 插件加载错误的异常类
 */
class PluginLoadError : public std::runtime_error {
public:
    explicit PluginLoadError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief PluginLibrary是已加载的插件库
 */
class PluginLibrary {
public:
    PluginLibrary(const fs::path& library_path);
    ~PluginLibrary();

    PluginLibrary(const PluginLibrary&) = delete;
    PluginLibrary& operator=(const PluginLibrary&) = delete;

    PluginLibrary(PluginLibrary&& other) noexcept;
    PluginLibrary& operator=(PluginLibrary&& other) noexcept;

    /**
     * @brief 从库中获取插件元数据
     */
    PluginMetadata getMetadata() const;

    /**
     * @brief 创建插件实例
     * @tparam InterfaceType 插件接口类型
     * @return 指向插件实例的共享指针
     */
    template<typename InterfaceType>
    std::shared_ptr<InterfaceType> createInstance() {
        static_assert(std::is_base_of_v<IPlugin, InterfaceType>,
                     "InterfaceType must derive from IPlugin");

        using CreateFunc = InterfaceType* (*)();
        using DestroyFunc = void (*)(InterfaceType*);

        auto create_func = getSymbol<CreateFunc>("create_plugin_instance");
        auto destroy_func = getSymbol<DestroyFunc>("destroy_plugin_instance");

        if (!create_func || !destroy_func) {
            throw PluginLoadError("Failed to find plugin creation/destruction symbols");
        }

        InterfaceType* raw_ptr = create_func();
        if (!raw_ptr) {
            throw PluginLoadError("Plugin creation function returned null");
        }

        return std::shared_ptr<InterfaceType>(raw_ptr, [destroy_func](InterfaceType* ptr) {
            destroy_func(ptr);
        });
    }

    /**
     * @brief 检查库是否成功加载
     */
    bool isValid() const { return handle_ != nullptr; }

    /**
     * @brief 获取库文件路径
     */
    const fs::path& getPath() const { return library_path_; }

private:
    void* handle_;
    fs::path library_path_;

    template<typename T>
    T getSymbol(const std::string& symbol_name) const {
        if (!isValid()) {
            return nullptr;
        }

#if defined(_WIN32)
        auto symbol = reinterpret_cast<T>(GetProcAddress(static_cast<HMODULE>(handle_), symbol_name.c_str()));
#else
        auto symbol = reinterpret_cast<T>(dlsym(handle_, symbol_name.c_str()));
#endif
        return symbol;
    }

    void closeLibrary();
};

/**
 * @brief 用于加载和管理插件的主插件管理器类
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    /**
     * @brief 从指定库路径加载插件
     * @param library_path 插件库的路径
     * @return 指向已加载插件库的shared_ptr
     */
    std::shared_ptr<PluginLibrary> loadPlugin(const fs::path& library_path);

    /**
     * @brief 卸载插件库
     * @param library_path 要卸载的插件库路径
     */
    void unloadPlugin(const fs::path& library_path);

    /**
     * @brief 卸载所有已加载的插件
     */
    void unloadAllPlugins();

    /**
     * @brief 从目录中发现并加载插件
     * @param directory_path 要搜索插件的目录
     * @param pattern 要匹配的文件模式（例如，"*.so", "*.dll"）
     * @return 已加载插件库的集合
     */
    std::vector<std::shared_ptr<PluginLibrary>> discoverPlugins(
        const fs::path& directory_path,
        const std::string& pattern = default_plugin_pattern());

    /**
     * @brief 获取所有当前已加载的插件
     */
    const std::unordered_map<fs::path, std::shared_ptr<PluginLibrary>>& getLoadedPlugins() const {
        return loaded_plugins_;
    }

    /**
     * @brief 获取当前平台的默认插件文件类型
     */
    static std::string default_plugin_pattern();

private:
    std::unordered_map<fs::path, std::shared_ptr<PluginLibrary>> loaded_plugins_;
};

} // namespace plugin_manager

#endif // PLUGIN_MANAGER_PLUGIN_LOADER_H