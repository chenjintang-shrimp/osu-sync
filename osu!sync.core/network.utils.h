/*
 * 网络工具类
 * 包含下载和网络相关功能
 */

#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

namespace osu {

// 镜像站配置
struct Mirror {
    std::string name;
    std::string baseUrl;
    bool requiresNoskip;
};

// 下载选项
struct DownloadOptions {
    std::string mirror;     // 要使用的镜像站名称
    size_t concurrent = 25; // 并发下载数量
};

class NetworkUtils {
public:
    // 执行系统命令并返回输出
    static std::string executeCommand(const std::string& command);
    
    // 批量下载谱面
    static bool downloadFile(const std::vector<std::string>& beatmapIds,
                           const fs::path& savePath,
                           const DownloadOptions& options = DownloadOptions());
                           
    // 单个谱面下载（向后兼容）
    static bool downloadFile(const std::string& beatmapId,
                           const fs::path& savePath,
                           const DownloadOptions& options = DownloadOptions()) {
        return downloadFile(std::vector<std::string>{beatmapId}, savePath, options);
    }
    
    // 验证下载的文件
    static bool validateFile(const fs::path& filePath);
    
    // 获取镜像站URL
    static std::string getMirrorURL(const std::string& beatmapId, const std::string& mirror);
    
    // 获取可用的镜像站列表
    static std::unordered_map<std::string, Mirror> getMirrors();
};

} // namespace osu