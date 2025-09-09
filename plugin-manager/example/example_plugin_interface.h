#ifndef EXAMPLE_PLUGIN_INTERFACE_H
#define EXAMPLE_PLUGIN_INTERFACE_H

#include "plugin_manager/plugin_interface.h"
#include <string>
#include <map>

namespace example {

/**
 * @brief 用于演示的示例插件接口
 *
 * 此接口扩展了基础 IPlugin，为示例插件提供特定功能。
 * 此处为了演示，增加了一个新的纯虚函数：execute，参数和返回值都是string。
 * 在实际项目中，你可以增加任何多个你需要的纯虚函数，参数和返回值类型可以根据需要进行定义。
 */
class IExamplePlugin : public plugin_manager::IPlugin {
public:
    virtual ~IExamplePlugin() = default;

    /**
     * @brief 执行插件的主要功能
     * @param input 要处理的输入字符串
     * @return 处理后的输出字符串
     */
    virtual std::string execute(const std::string& input) = 0;

    /**
     * @brief 获取插件的类别
     * @return 类别字符串
     */
    virtual std::string getCategory() const = 0;

    /**
     * @brief 获取配置选项
     * @return 配置键值对的映射
     */
    virtual std::map<std::string, std::string> getConfiguration() const = 0;

    /**
     * @brief 设置配置选项
     * @param key 配置键
     * @param value 配置值
     */
    virtual void setConfiguration(const std::string& key, const std::string& value) = 0;
};

} // namespace example

// 用于声明示例插件接口的宏
#define EXAMPLE_PLUGIN_INTERFACE() \
    PLUGIN_INTERFACE(example::IExamplePlugin)

#endif // EXAMPLE_PLUGIN_INTERFACE_H