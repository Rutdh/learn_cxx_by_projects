#include <gtest/gtest.h>
#include "plugin_manager/plugin_interface.h"
#include "plugin_manager/plugin_loader.h"
#include "plugin_manager/plugin_metadata.h"

using namespace plugin_manager;

// 插件管理器测试的测试
class PluginManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// 测试基本插件接口功能
TEST_F(PluginManagerTest, PluginInterfaceBasic) {
    PluginMetadata metadata{
        "TestPlugin",
        "1.0.0",
        "A test plugin",
        "Test Author",
        "MIT"
    };
    
    EXPECT_EQ(metadata.name, "TestPlugin");
    EXPECT_EQ(metadata.version, "1.0.0");
    EXPECT_EQ(metadata.description, "A test plugin");
    EXPECT_EQ(metadata.author, "Test Author");
    EXPECT_EQ(metadata.license, "MIT");
}

// 测试version解析和比较
TEST_F(PluginManagerTest, VersionParsing) {
    auto version = MetadataUtils::parseVersion("1.2.3");
    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
    
    EXPECT_THROW(MetadataUtils::parseVersion("invalid"), MetadataError);
    EXPECT_THROW(MetadataUtils::parseVersion("1.2"), MetadataError);
    EXPECT_THROW(MetadataUtils::parseVersion("1.2.3.4"), MetadataError);
}

// 测试version的比较
TEST_F(PluginManagerTest, VersionComparison) {
    EXPECT_EQ(MetadataUtils::compareVersions("1.0.0", "1.0.0"), 0);
    EXPECT_EQ(MetadataUtils::compareVersions("1.0.1", "1.0.0"), 1);
    EXPECT_EQ(MetadataUtils::compareVersions("1.0.0", "1.0.1"), -1);
    EXPECT_EQ(MetadataUtils::compareVersions("2.0.0", "1.0.0"), 1);
    EXPECT_EQ(MetadataUtils::compareVersions("1.0.0", "2.0.0"), -1);
}

// 测试版本兼容性
TEST_F(PluginManagerTest, VersionCompatibility) {
    EXPECT_TRUE(MetadataUtils::checkVersionCompatibility("1.0.0", "1.2.3"));
    EXPECT_TRUE(MetadataUtils::checkVersionCompatibility("1.2.3", "1.0.0"));
    EXPECT_FALSE(MetadataUtils::checkVersionCompatibility("2.0.0", "1.0.0"));
    EXPECT_FALSE(MetadataUtils::checkVersionCompatibility("1.0.0", "2.0.0"));
}

// 测试版本约束
TEST_F(PluginManagerTest, VersionConstraints) {
    EXPECT_TRUE(MetadataUtils::checkVersionConstraint("1.0.0", ">=1.0.0"));
    EXPECT_TRUE(MetadataUtils::checkVersionConstraint("1.0.0", "<=1.0.0"));
    EXPECT_TRUE(MetadataUtils::checkVersionConstraint("1.0.0", "==1.0.0"));
    EXPECT_TRUE(MetadataUtils::checkVersionConstraint("1.1.0", "~1.0.0"));
    
    EXPECT_FALSE(MetadataUtils::checkVersionConstraint("1.0.0", ">1.0.0"));
    EXPECT_FALSE(MetadataUtils::checkVersionConstraint("1.0.0", "<1.0.0"));
    EXPECT_FALSE(MetadataUtils::checkVersionConstraint("2.0.0", "~1.0.0"));
}

// 测试元数据验证
TEST_F(PluginManagerTest, MetadataValidation) {
    PluginMetadata valid_metadata{
        "TestPlugin",
        "1.0.0",
        "A test plugin",
        "Test Author",
        "MIT"
    };
    
    EXPECT_NO_THROW(MetadataUtils::validateMetadata(valid_metadata));
    
    PluginMetadata invalid_name{
        "",
        "1.0.0",
        "A test plugin",
        "Test Author",
        "MIT"
    };
    EXPECT_THROW(MetadataUtils::validateMetadata(invalid_name), MetadataError);
    
    PluginMetadata invalid_version{
        "TestPlugin",
        "",
        "A test plugin",
        "Test Author",
        "MIT"
    };
    EXPECT_THROW(MetadataUtils::validateMetadata(invalid_version), MetadataError);
    
    PluginMetadata invalid_author{
        "TestPlugin",
        "1.0.0",
        "A test plugin",
        "",
        "MIT"
    };
    EXPECT_THROW(MetadataUtils::validateMetadata(invalid_author), MetadataError);
}

// 测试依赖项解析
TEST_F(PluginManagerTest, DependencyResolution) {
    ExtendedPluginMetadata metadata;
    metadata.dependencies = {
        {"dep1", ">=1.0.0"},
        {"dep2", "~2.0.0"}
    };
    
    std::map<std::string, std::string> available_plugins = {
        {"dep1", "1.5.0"},
        {"dep2", "2.3.0"}
    };
    
    EXPECT_TRUE(DependencyResolver::checkDependencies(metadata, available_plugins));
    EXPECT_TRUE(DependencyResolver::getMissingDependencies(metadata, available_plugins).empty());
    
    // 测试缺失依赖项
    std::map<std::string, std::string> missing_dep = {
        {"dep1", "1.5.0"}
    };
    auto missing = DependencyResolver::getMissingDependencies(metadata, missing_dep);
    EXPECT_FALSE(missing.empty());
    EXPECT_EQ(missing.size(), 1);
    
    // 测试版本不匹配
    std::map<std::string, std::string> version_mismatch = {
        {"dep1", "0.9.0"},
        {"dep2", "2.3.0"}
    };
    auto mismatches = DependencyResolver::getMissingDependencies(metadata, version_mismatch);
    EXPECT_FALSE(mismatches.empty());
}

// 测试平台检测（基本测试）
TEST_F(PluginManagerTest, PlatformDetection) {
    ExtendedPluginMetadata metadata;
    
    // 测试无平台限制
    EXPECT_TRUE(MetadataUtils::isPlatformSupported(metadata));
    
    // 测试当前平台
    metadata.supported_platforms = {"linux", "windows", "macos"};
    EXPECT_TRUE(MetadataUtils::isPlatformSupported(metadata));
    
    // 测试不受支持的平台
    metadata.supported_platforms = {"nonexistent"};
    EXPECT_FALSE(MetadataUtils::isPlatformSupported(metadata));
}

// 运行测试的主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}