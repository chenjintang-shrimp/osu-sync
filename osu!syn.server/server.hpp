#pragma once
#include <string>
#include <filesystem>
#include "3rdparty/httplib.h"
#include "logger.hpp"

namespace fs = std::filesystem;

class FileValidator
{
public:
    static bool isSafePath(const fs::path& path, std::string& errorMessage);
    static bool isAllowedFileType(const std::string& filename, std::string& errorMessage);
    static bool isValidFileSize(size_t fileSize, std::string& errorMessage);
    static bool validateChecksum(const std::string& content, const std::string& expectedHash,
                                 std::string& errorMessage);
}; // 添加缺失的闭合大括号

class Config
{
public:
    static void load(const std::string& configFile);
    static bool reload(); // 重新加载配置

    // Getters
    static const std::string& getHost() { return host_; }
    static int getPort() { return port_; }
    static int getLogLevel() { return logLevel_; }
    static size_t getMaxFileSize() { return maxFileSize_; }
    static bool isDebugMode() { return debugMode_; }
    static const std::string& getOauthClientID() { return oauthClientID_; }
    static const std::string& getOauthClientSecret() { return oauthClientSecret_; }
    static const std::string& getOauthRedirectUri() { return oauthRedirectUri_; }
    static const fs::path& getUploadDir() { return uploadDir_; }
    static const fs::path& getLogDir() { return logDir_; }

private:
    static std::string configPath_;
    static std::string host_;
    static int port_;
    static size_t maxFileSize_;
    static fs::path uploadDir_;
    static fs::path logDir_;
    static std::string oauthClientID_;
    static std::string oauthClientSecret_;
    static std::string oauthRedirectUri_;
    
    // 新增配置项
    static int logLevel_;  // 日志级别 (0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR)
    static bool debugMode_; // 是否为调试模式
};

class FileUploadHandler
{
public:
    explicit FileUploadHandler(fs::path baseUploadDir, const std::shared_ptr<Logger>& logger);

    void handleUpload(const httplib::Request& req, httplib::Response& res);

private:
    fs::path baseUploadDir_;
    std::shared_ptr<Logger> logger_;

    bool validateRequest(const httplib::Request& req, httplib::Response& res);
    bool prepareUploadDirectory(const fs::path& path, httplib::Response& res);
    bool saveUploadedFile(const httplib::MultipartFormData& file,
                          const fs::path& path,
                          httplib::Response& res);
};

class FileDownloadHandler
{
public:
    explicit FileDownloadHandler(const fs::path& baseUploadDir, std::shared_ptr<Logger> logger);

    // 处理下载请求
    void handleDownload(const httplib::Request& req, httplib::Response& res);

private:
    fs::path baseUploadDir_;
    std::shared_ptr<Logger> logger_;

    // 从请求路径中提取用户名
    std::string extractUsername(const std::string& path);

    // 验证并构建JSON文件路径
    fs::path buildJsonPath(const std::string& username);
};
