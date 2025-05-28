/*
 * 网络工具类头文件
 * 提供beatmap下载和网络相关功能
 */

#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

namespace osu {

// 进度回调函数类型
using ProgressCallback = std::function<void(size_t current, size_t total)>;

// 镜像站配置
struct Mirror {
    std::string name;
    std::string baseUrl;
    bool requiresNoskip;
};

// 下载选项
struct DownloadOptions {
    int maxRetries = 3;             // 最大重试次数
    int timeout = 30;               // 超时时间(秒)
    ProgressCallback onProgress;     // 进度回调函数
    std::string mirror = "sayobot"; // 使用的镜像站
};

class NetworkUtils {
public:
    // 从URL下载文件，使用beatmapDownloader工具
    static bool downloadFile(
        const std::string& url,
        const fs::path& savePath,
        const DownloadOptions& options = DownloadOptions{}
    );
    
    // 获取可用的镜像站列表
    static std::unordered_map<std::string, Mirror> getMirrors();
    
    // 验证下载的文件
    static bool validateFile(const fs::path& filePath);
    
    // 根据beatmap ID生成对应镜像站的下载URL
    static std::string getMirrorURL(const std::string& beatmapId, const std::string& mirror = "sayobot");

private:
    // 执行命令行并获取输出 
    static std::string executeCommand(const std::string& command);
};

} // namespace osu