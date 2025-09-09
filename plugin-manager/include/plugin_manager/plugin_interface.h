#ifndef PLUGIN_MANAGER_PLUGIN_INTERFACE_H
#define PLUGIN_MANAGER_PLUGIN_INTERFACE_H

#include <string>
#include <string_view>
#include <memory>
#include <type_traits>

namespace plugin_manager {

/**
 * @brief 插件元数据结构，包含插件信息，该定义仅仅只是演示，实际项目中可以根据需要进行定义
 */
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
};

/**
 * @brief 所有插件的基础接口
 *
 * 所有插件必须实现此接口。它提供了插件标识和生命周期管理的基本功能。
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;

    /**
     * @brief 获取插件元数据
     * @return 包含插件信息的PluginMetadata结构
     */
    virtual PluginMetadata getMetadata() const = 0;

    /**
     * @brief 初始化插件
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool initialize() = 0;

    /**
     * @brief 关闭插件
     */
    virtual void shutdown() = 0;

    /**
     * @brief 获取插件的唯一标识符，这里用的是string，实际项目中可以根据需要选择标志符类型
     * @return 包含插件ID的字符串
     */
    virtual std::string getId() const = 0;
};

/**
 * @brief 具有特定功能的类型化插件的基类
 *
 * @tparam InterfaceType 此插件实现的特定接口
 */
template<typename InterfaceType>
class TypedPlugin : public IPlugin {
    static_assert(std::is_base_of_v<IPlugin, InterfaceType>,
                  "InterfaceType must derive from IPlugin");
};

// 插件版本信息
struct Version {
    int major;
    int minor;
    int patch;
    
    constexpr bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    constexpr bool operator!=(const Version& other) const {
        return !(*this == other);
    }
};

// 当前插件系统版本
constexpr Version PLUGIN_SYSTEM_VERSION{1, 0, 0};

} // namespace plugin_manager

// 声明插件元数据的宏（类似于Qt的Q_PLUGIN_METADATA）
#define PLUGIN_METADATA(metadata) \
    extern "C" plugin_manager::PluginMetadata plugin_metadata() { \
        return metadata; \
    }

// 声明插件接口的宏
#define PLUGIN_INTERFACE(InterfaceType) \
    extern "C" InterfaceType* create_plugin_instance() { \
        return new InterfaceType(); \
    } \
    extern "C" void destroy_plugin_instance(InterfaceType* plugin) { \
        delete plugin; \
    }

#endif // PLUGIN_MANAGER_PLUGIN_INTERFACE_H