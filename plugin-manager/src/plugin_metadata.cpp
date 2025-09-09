#include "plugin_manager/plugin_metadata.h"
#include <regex>
#include <sstream>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <TargetConditionals.h>
#else
    #include <sys/utsname.h>
#endif

namespace plugin_manager {

void MetadataUtils::validateMetadata(const PluginMetadata& metadata) {
    if (metadata.name.empty()) {
        throw MetadataError("Plugin name cannot be empty");
    }
    
    if (metadata.version.empty()) {
        throw MetadataError("Plugin version cannot be empty");
    }
    
    try {
        parseVersion(metadata.version);
    } catch (const MetadataError&) {
        throw MetadataError("Invalid version format: " + metadata.version);
    }
    
    if (metadata.author.empty()) {
        throw MetadataError("Plugin author cannot be empty");
    }
}

bool MetadataUtils::checkVersionCompatibility(const std::string& version1, const std::string& version2) {
    try {
        auto v1 = parseVersion(version1);
        auto v2 = parseVersion(version2);
        // 主版本必须匹配以确保兼容性
        return v1.major == v2.major;
    } catch (const MetadataError&) {
        return false;
    }
}

bool MetadataUtils::checkVersionConstraint(const std::string& version, const std::string& constraint) {
    try {
        auto ver = parseVersion(version);
        
        // 解析约束（例如，">=1.0.0", "~2.3.0"）
        std::regex constraint_regex(R"(([>=<~]+)\s*([\d.]+))");
        std::smatch match;
        
        if (std::regex_search(constraint, match, constraint_regex) && match.size() == 3) {
            std::string op = match[1].str();
            std::string constr_ver_str = match[2].str();
            auto constr_ver = parseVersion(constr_ver_str);
            
            int comparison = compareVersions(version, constr_ver_str);
            
            if (op == ">=") {
                return comparison >= 0;
            } else if (op == ">") {
                return comparison > 0;
            } else if (op == "<=") {
                return comparison <= 0;
            } else if (op == "<") {
                return comparison < 0;
            } else if (op == "==") {
                return comparison == 0;
            } else if (op == "~") {
                // 波浪号范围：主版本必须匹配
                return ver.major == constr_ver.major;
            }
        }
        
        return false;
    } catch (const MetadataError&) {
        return false;
    }
}

Version MetadataUtils::parseVersion(const std::string& version) {
    std::regex version_regex(R"((\d+)\.(\d+)\.(\d+))");
    std::smatch match;
    
    if (std::regex_match(version, match, version_regex) && match.size() == 4) {
        return Version{
            std::stoi(match[1].str()),
            std::stoi(match[2].str()),
            std::stoi(match[3].str())
        };
    }
    
    throw MetadataError("Invalid version format: " + version);
}

int MetadataUtils::compareVersions(const std::string& v1, const std::string& v2) {
    try {
        auto ver1 = parseVersion(v1);
        auto ver2 = parseVersion(v2);
        
        if (ver1.major != ver2.major) {
            return ver1.major > ver2.major ? 1 : -1;
        }
        if (ver1.minor != ver2.minor) {
            return ver1.minor > ver2.minor ? 1 : -1;
        }
        if (ver1.patch != ver2.patch) {
            return ver1.patch > ver2.patch ? 1 : -1;
        }
        return 0;
    } catch (const MetadataError&) {
        throw MetadataError("Cannot compare invalid versions: " + v1 + " and " + v2);
    }
}

bool MetadataUtils::isPlatformSupported(const ExtendedPluginMetadata& metadata) {
    if (metadata.supported_platforms.empty()) {
        return true; // 无平台限制
    }
    
    std::string current_platform;
    
#ifdef _WIN32
    current_platform = "windows";
#elif __APPLE__
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        current_platform = "ios";
    #else
        current_platform = "macos";
    #endif
#elif __ANDROID__
    current_platform = "android";
#elif __linux__
    current_platform = "linux";
#else
    current_platform = "unknown";
#endif
    
    for (const auto& platform : metadata.supported_platforms) {
        if (platform == current_platform) {
            return true;
        }
    }
    
    return false;
}

bool MetadataUtils::isSystemVersionCompatible(const ExtendedPluginMetadata& metadata) {
    if (metadata.min_system_version.empty() && metadata.max_system_version.empty()) {
        return true; // 无版本限制
    }
    
    // 为简单起见，这里使用基本方法
    // 在实际实现中，可能会获取实际的系统版本
    std::string system_version = "1.0.0"; // 占位符
    
    if (!metadata.min_system_version.empty()) {
        if (compareVersions(system_version, metadata.min_system_version) < 0) {
            return false;
        }
    }
    
    if (!metadata.max_system_version.empty()) {
        if (compareVersions(system_version, metadata.max_system_version) > 0) {
            return false;
        }
    }
    
    return true;
}

bool DependencyResolver::checkDependencies(
    const ExtendedPluginMetadata& metadata,
    const std::map<std::string, std::string>& available_plugins) {
    
    return getMissingDependencies(metadata, available_plugins).empty();
}

std::vector<std::string> DependencyResolver::getMissingDependencies(
    const ExtendedPluginMetadata& metadata,
    const std::map<std::string, std::string>& available_plugins) {
    
    std::vector<std::string> missing;
    
    for (const auto& [dep_id, constraint] : metadata.dependencies) {
        auto it = available_plugins.find(dep_id);
        if (it == available_plugins.end()) {
            missing.push_back("Missing dependency: " + dep_id);
        } else if (!MetadataUtils::checkVersionConstraint(it->second, constraint)) {
            missing.push_back("Dependency version mismatch: " + dep_id + 
                             " (required: " + constraint + ", found: " + it->second + ")");
        }
    }
    
    return missing;
}

} // namespace plugin_manager