#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace osu {

// 下载进度回调函数类型
using ProgressCallback = std::function<void(size_t current, size_t total)>;

// 镜像站配置
struct Mirror {
    std::string name;
    std::string baseUrl;
    bool requiresNoskip;
};

// 下载选项
struct DownloadOptions {
    bool resumable = true;           // 是否支持断点续传
    int maxRetries = 3;             // 最大重试次数
    int timeout = 30;               // 超时时间(秒)
    size_t bufferSize = 81920;      // 缓冲区大小(80KB)
    ProgressCallback onProgress;     // 进度回调函数
    std::string mirror = "sayobot"; // 使用的镜像站

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
    
    // 执行命令行并获取输出
    static std::string executeCommand(const std::string& command);

    // 上传数据
    static bool uploadData(
        const fs::path& filename,
        const std::string& username,
        const std::string& remoteURL
    );

private:
    // 创建HTTP客户端
    static std::unique_ptr<httplib::Client> createHttpClient(const std::string& url);
    
    // 获取文件大小
    static size_t getFileSize(const std::string& url);
    
    // 处理下载数据块
    static bool handleDownloadChunk(const char* data, size_t size, DownloadContext& context);
    
    // 解析URL
    static bool parseUrl(const std::string& url, std::string& host, std::string& path, bool& isHttps);
    
    // 生成Sayobot镜像URL
    static std::string getSayobotMirrorURL(const std::string& beatmapId);
};

} // namespace osu
}