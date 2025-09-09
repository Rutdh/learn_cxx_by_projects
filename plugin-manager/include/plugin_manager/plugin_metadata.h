#ifndef PLUGIN_MANAGER_PLUGIN_METADATA_H
#define PLUGIN_MANAGER_PLUGIN_METADATA_H

#include "plugin_interface.h"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <regex>
#include <stdexcept>

namespace plugin_manager {

/**
 * @brief 元数据验证错误的异常类
 */
class MetadataError : public std::runtime_error {
public:
    explicit MetadataError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief 包含依赖项和兼容性信息的扩展插件元数据
 */
struct ExtendedPluginMetadata : public PluginMetadata {
    std::map<std::string, std::string> dependencies; // plugin_id -> version constraint
    std::vector<std::string> supported_platforms;
    std::string min_system_version;
    std::string max_system_version;
    
    // 额外的元数据字段
    std::map<std::string, std::string> additional_data;
};

/**
 * @brief 用于插件元数据操作的工具类
 */
class MetadataUtils {
public:
    /**
     * @brief 验证插件元数据
     * @param metadata 要验证的元数据
     * @throws MetadataError 验证失败
     */
    static void validateMetadata(const PluginMetadata& metadata);
    
    /**
     * @brief 检查两个版本是否兼容
     * @param version1 第一个版本字符串
     * @param version2 第二个版本字符串
     * @return 如果版本兼容则返回 true
     */
    static bool checkVersionCompatibility(const std::string& version1, const std::string& version2);
    
    /**
     * @brief 检查版本是否满足约束
     * @param version 要检查的版本
     * @param constraint 版本约束（例如，">=1.0.0", "~2.3.0"）
     * @return 如果版本满足约束则返回 true
     */
    static bool checkVersionConstraint(const std::string& version, const std::string& constraint);
    
    /**
     * @brief 将版本字符串解析为主要、次要、补丁组件
     * @param version 版本字符串
     * @return Version structure
     * @throws MetadataError(如果版本格式无效)
     */
    static Version parseVersion(const std::string& version);
    
    /**
     * @brief 比较两个版本字符串
     * @param v1 第一个版本
     * @param v2 第二个版本
     * @return -1 当 v1 < v2, 0 当 v1 == v2, 1 当 v1 > v2
     */
    static int compareVersions(const std::string& v1, const std::string& v2);
    
    /**
     * @brief 检查当前平台是否受支持
     * @param metadata 包含受支持平台的插件元数据
     * @return 如果当前平台受支持则返回 true
     */
    static bool isPlatformSupported(const ExtendedPluginMetadata& metadata);
    
    /**
     * @brief 检查系统版本是否兼容
     * @param metadata 包含版本约束的插件元数据
     * @return 如果系统版本兼容则返回 true
     */
    static bool isSystemVersionCompatible(const ExtendedPluginMetadata& metadata);
};

/**
 * @brief 插件依赖项解析器
 */
class DependencyResolver {
public:
    /**
     * @brief 检查所有依赖项是否满足
     * @param metadata 包含依赖项的插件元数据
     * @param available_plugins 可用插件的映射（plugin_id -> version）
     * @return 如果所有依赖项都满足则返回 true
     */
    static bool checkDependencies(
        const ExtendedPluginMetadata& metadata,
        const std::map<std::string, std::string>& available_plugins);
    
    /**
     * @brief 获取缺失的依赖项
     * @param metadata 包含依赖项的插件元数据
     * @param available_plugins 可用插件的映射（plugin_id -> version）
     * @return 缺失依赖项消息的向量
     */
    static std::vector<std::string> getMissingDependencies(
        const ExtendedPluginMetadata& metadata,
        const std::map<std::string, std::string>& available_plugins);
};

} // namespace plugin_manager

#endif // PLUGIN_MANAGER_PLUGIN_METADATA_H