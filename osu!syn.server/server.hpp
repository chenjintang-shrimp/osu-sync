#pragma once
#include <string>
#include <filesystem>
#include "httplib.h"
#include "logger.hpp"

namespace fs = std::filesystem;

class FileValidator {
public:
    static bool isSafePath(const fs::path& path, std::string& errorMessage);
    static bool isAllowedFileType(const std::string& filename, std::string& errorMessage);
    static bool isValidFileSize(size_t fileSize, std::string& errorMessage);
    static bool validateChecksum(const std::string& content, const std::string& expectedHash, std::string& errorMessage);
};  // 添加缺失的闭合大括号

class Config {
public:
    static void load(const std::string& configFile);
    static bool reload();  // 重新加载配置
    
    static std::string getHost() { return host_; }
    static int getPort() { return port_; }
    static size_t getMaxFileSize() { return maxFileSize_; }
    static const fs::path& getUploadDir() { return uploadDir_; }
    static const fs::path& getLogDir() { return logDir_; }
    
private:
    static std::string configPath_;
    static std::string host_;
    static int port_;
    static size_t maxFileSize_;
    static fs::path uploadDir_;
    static fs::path logDir_;
};

class FileUploadHandler {
public:
    explicit FileUploadHandler(const fs::path& baseUploadDir, std::shared_ptr<Logger> logger);
    
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

class FileDownloadHandler {
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
