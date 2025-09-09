#include <iostream>
#include <memory>
#include "plugin_manager/plugin_loader.h"
#include "example_plugin_interface.h"

using namespace plugin_manager;

int main() {
    std::cout << "Plugin Manager Example Application" << std::endl;
    std::cout << "==================================" << std::endl;

    try {
        // 创建插件管理器
        PluginManager manager;

        // 从当前目录发现并加载插件
        auto plugins = manager.discoverPlugins(".");
        
        std::cout << "Loaded " << plugins.size() << " plugins:" << std::endl;
        
        for (const auto& plugin_lib : plugins) {
            try {
                auto metadata = plugin_lib->getMetadata();
                std::cout << " - " << metadata.name << " v" << metadata.version 
                          << " (" << metadata.description << ")" << std::endl;
                
                // 创建插件实例，假设插件实现了 IExamplePlugin 接口
                auto plugin = plugin_lib->createInstance<example::IExamplePlugin>();
                
                // 初始化插件
                if (plugin->initialize()) {
                    std::cout << "   Plugin initialized successfully" << std::endl;
                    
                    // 测试插件功能
                    std::string test_input = "Hello World";
                    std::string result = plugin->execute(test_input);
                    
                    std::cout << "   Input: " << test_input << std::endl;
                    std::cout << "   Output: " << result << std::endl;
                    std::cout << "   Category: " << plugin->getCategory() << std::endl;
                    
                    // 显示配置
                    auto config = plugin->getConfiguration();
                    if (!config.empty()) {
                        std::cout << "   Configuration: " << std::endl;
                        for (const auto& [key, value] : config) {
                            std::cout << "     " << key << " = " << value << std::endl;
                        }
                    }
                    
                    // Shutdown plugin
                    plugin->shutdown();
                } else {
                    std::cout << "   Plugin initialization failed" << std::endl;
                }
                
            } catch (const std::exception& e) {
                std::cout << "   Error: " << e.what() << std::endl;
            }
            
            std::cout << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Example completed successfully!" << std::endl;
    return 0;
}