#include "example_plugin_interface.h"
#include <string>
#include <map>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace {

class MathPlugin : public example::IExamplePlugin {
public:
    MathPlugin() = default;

    plugin_manager::PluginMetadata getMetadata() const override {
        return plugin_manager::PluginMetadata{
            "MathOperationsPlugin",
            "1.0.0",
            "A plugin for mathematical operations",
            "Example Author",
            "MIT"
        };
    }

    bool initialize() override {
        // 初始化逻辑在这里
        config_["precision"] = "2";
        return true;
    }

    void shutdown() override {
        // 清理逻辑写在这里
    }

    std::string getId() const override {
        return "math_operations";
    }

    std::string execute(const std::string& input) override {
        try {
            // 解析输入：operation operand1 operand2
            std::istringstream iss(input);
            std::string operation;
            double a, b;
            iss >> operation >> a >> b;

            double result = 0.0;

            if (operation == "add") {
                result = a + b;
            } else if (operation == "subtract") {
                result = a - b;
            } else if (operation == "multiply") {
                result = a * b;
            } else if (operation == "divide") {
                if (b == 0) {
                    return "Error: Division by zero";
                }
                result = a / b;
            } else if (operation == "power") {
                result = std::pow(a, b);
            } else {
                return "Error: Unknown operation. Supported: add, subtract, multiply, divide, power";
            }

            // Format result with precision
            int precision = std::stoi(config_["precision"]);
            std::ostringstream oss;
            oss.precision(precision);
            oss << std::fixed << result;
            return oss.str();

        } catch (const std::exception& e) {
            return "Error: " + std::string(e.what());
        }
    }

    std::string getCategory() const override {
        return "mathematics";
    }

    std::map<std::string, std::string> getConfiguration() const override {
        return config_;
    }

    void setConfiguration(const std::string& key, const std::string& value) override {
        if (key == "precision") {
            // Validate precision value
            try {
                int prec = std::stoi(value);
                if (prec < 0 || prec > 10) {
                    throw std::invalid_argument("Precision must be between 0 and 10");
                }
                config_[key] = value;
            } catch (const std::exception&) {
                throw std::invalid_argument("Invalid precision value: " + value);
            }
        } else {
            config_[key] = value;
        }
    }

private:
    std::map<std::string, std::string> config_;
};

} // namespace

namespace {
plugin_manager::PluginMetadata getMathPluginMetadata() {
    return plugin_manager::PluginMetadata{
        "MathOperationsPlugin",
        "1.0.0",
        "A plugin for mathematical operations",
        "Example Author",
        "MIT"
    };
}
}

// Plugin metadata export
PLUGIN_METADATA(getMathPluginMetadata())

// Plugin interface export
extern "C" example::IExamplePlugin* create_plugin_instance() {
    return new MathPlugin();
}

extern "C" void destroy_plugin_instance(example::IExamplePlugin* plugin) {
    delete plugin;
}