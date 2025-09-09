#include "example_plugin_interface.h"
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

namespace {

class StringPlugin : public example::IExamplePlugin {
public:
    StringPlugin() = default;

    plugin_manager::PluginMetadata getMetadata() const override {
        return plugin_manager::PluginMetadata{
            "StringUtilityPlugin",
            "1.0.0",
            "A plugin for string manipulation utilities",
            "Example Author",
            "MIT"
        };
    }

    bool initialize() override {
        // 初始化逻辑在这里
        config_["case"] = "lower";
        return true;
    }

    void shutdown() override {
        // 清理逻辑写在这里
    }

    std::string getId() const override {
        return "string_utility";
    }

    std::string execute(const std::string& input) override {
        if (config_["case"] == "upper") {
            std::string result = input;
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            return result;
        } else if (config_["case"] == "lower") {
            std::string result = input;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        } else {
            return input; // 无变化
        }
    }

    std::string getCategory() const override {
        return "text_processing";
    }

    std::map<std::string, std::string> getConfiguration() const override {
        return config_;
    }

    void setConfiguration(const std::string& key, const std::string& value) override {
        config_[key] = value;
    }

private:
    std::map<std::string, std::string> config_;
};

} // namespace

namespace {
plugin_manager::PluginMetadata getStringPluginMetadata() {
    return plugin_manager::PluginMetadata{
        "StringUtilityPlugin",
        "1.0.0",
        "A plugin for string manipulation utilities",
        "Example Author",
        "MIT"
    };
}
}

// Plugin metadata export
PLUGIN_METADATA(getStringPluginMetadata())

// Plugin interface export
extern "C" example::IExamplePlugin* create_plugin_instance() {
    return new StringPlugin();
}

extern "C" void destroy_plugin_instance(example::IExamplePlugin* plugin) {
    delete plugin;
}