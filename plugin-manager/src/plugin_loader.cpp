#include "plugin_manager/plugin_loader.h"
#include <stdexcept>
#include <iostream>

namespace plugin_manager {

PluginLibrary::PluginLibrary(const fs::path& library_path) 
    : handle_(nullptr), library_path_(library_path) {
    
#if defined(_WIN32)
    handle_ = LoadLibraryW(library_path_.c_str());
    if (!handle_) {
        DWORD error = GetLastError();
        throw PluginLoadError("Failed to load library: " + library_path_.string() + 
                             ", error code: " + std::to_string(error));
    }
#else
    handle_ = dlopen(library_path_.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle_) {
        const char* error = dlerror();
        throw PluginLoadError("Failed to load library: " + library_path_.string() + 
                             ", error: " + (error ? error : "unknown"));
    }
#endif
}

PluginLibrary::~PluginLibrary() {
    closeLibrary();
}

PluginLibrary::PluginLibrary(PluginLibrary&& other) noexcept
    : handle_(other.handle_), library_path_(std::move(other.library_path_)) {
    other.handle_ = nullptr;
}

PluginLibrary& PluginLibrary::operator=(PluginLibrary&& other) noexcept {
    if (this != &other) {
        closeLibrary();
        handle_ = other.handle_;
        library_path_ = std::move(other.library_path_);
        other.handle_ = nullptr;
    }
    return *this;
}

void PluginLibrary::closeLibrary() {
    if (handle_) {
#if defined(_WIN32)
        FreeLibrary(static_cast<HMODULE>(handle_));
#else
        dlclose(handle_);
#endif
        handle_ = nullptr;
    }
}

PluginMetadata PluginLibrary::getMetadata() const {
    using MetadataFunc = PluginMetadata (*)();
    
    auto metadata_func = getSymbol<MetadataFunc>("plugin_metadata");
    if (!metadata_func) {
        throw PluginLoadError("Failed to find plugin metadata symbol");
    }
    
    return metadata_func();
}

PluginManager::PluginManager() = default;

PluginManager::~PluginManager() {
    unloadAllPlugins();
}

std::shared_ptr<PluginLibrary> PluginManager::loadPlugin(const fs::path& library_path) {
    auto abs_path = fs::absolute(library_path);
    
    if (loaded_plugins_.find(abs_path) != loaded_plugins_.end()) {
        throw PluginLoadError("Plugin already loaded: " + abs_path.string());
    }
    
    try {
        auto plugin_lib = std::make_shared<PluginLibrary>(abs_path);
        loaded_plugins_[abs_path] = plugin_lib;
        return plugin_lib;
    } catch (const PluginLoadError& e) {
        throw;
    } catch (const std::exception& e) {
        throw PluginLoadError("Failed to load plugin: " + std::string(e.what()));
    }
}

void PluginManager::unloadPlugin(const fs::path& library_path) {
    auto abs_path = fs::absolute(library_path);
    loaded_plugins_.erase(abs_path);
}

void PluginManager::unloadAllPlugins() {
    loaded_plugins_.clear();
}

std::vector<std::shared_ptr<PluginLibrary>> PluginManager::discoverPlugins(
    const fs::path& directory_path, 
    const std::string& pattern) {
    
    std::vector<std::shared_ptr<PluginLibrary>> discovered_plugins;
    
    if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) {
        return discovered_plugins;
    }
    
    try {
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (entry.is_regular_file()) {
                const auto& path = entry.path();
                if (path.filename().string().find(pattern) != std::string::npos) {
                    try {
                        auto plugin = loadPlugin(path);
                        discovered_plugins.push_back(plugin);
                    } catch (const PluginLoadError& e) {
                        // 跳过不是有效插件的文件
                        continue;
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        throw PluginLoadError("Failed to discover plugins: " + std::string(e.what()));
    }
    
    return discovered_plugins;
}

std::string PluginManager::default_plugin_pattern() {
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

} // namespace plugin_manager