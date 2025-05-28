#include "server.h"
#include "logger.h"
#include <fstream>
#include "3rdparty/nlohmann/json.hpp"

using json = nlohmann::json;

// 静态成员初始化
std::string Config::configPath_;
std::string Config::host_ = "0.0.0.0";
int Config::port_ = 8080;
size_t Config::maxFileSize_ = 1024 * 1024 * 100; // 默认100MB
fs::path Config::uploadDir_ = "uploads";
fs::path Config::logDir_ = "logs";

void Config::load(const std::string& configFile) {
    configPath_ = configFile;
    try {
        std::ifstream f(configFile);
        if (!f.is_open()) {
            throw std::runtime_error("无法打开配置文件");
        }

        json config = json::parse(f);
        
        if (config.contains("host")) host_ = config["host"];
        if (config.contains("port")) port_ = config["port"];
        if (config.contains("maxFileSize")) maxFileSize_ = config["maxFileSize"];
        if (config.contains("uploadDir")) uploadDir_ = config["uploadDir"].get<std::string>();
        if (config.contains("logDir")) logDir_ = config["logDir"].get<std::string>();
        
    } catch (const std::exception& e) {
        std::cerr << "加载配置文件失败: " << e.what() << std::endl;
        std::cerr << "使用默认配置" << std::endl;
    }
}

bool Config::reload() {
    if (configPath_.empty()) {
        return false;
    }
    load(configPath_);
    return true;
}

bool FileValidator::isSafePath(const fs::path& path, std::string& errorMessage) {
    if (path.is_absolute()) {
        errorMessage = "不允许使用绝对路径";
        return false;
    }

    for (const auto& component : path) {
        if (component == ".." || component == ".") {
            errorMessage = "不允许使用相对路径导航";
            return false;
        }
    }

    return true;
}

bool FileValidator::isAllowedFileType(const std::string& filename, std::string& errorMessage) {
    // 可以根据需要扩展允许的文件类型
    static const std::vector<std::string> allowedExtensions = {
        ".osz", ".osr", ".osu", ".zip"
    };

    fs::path path(filename);
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (std::find(allowedExtensions.begin(), allowedExtensions.end(), ext) == allowedExtensions.end()) {
        errorMessage = "不支持的文件类型: " + ext;
        return false;
    }

    return true;
}

bool FileValidator::isValidFileSize(size_t fileSize, std::string& errorMessage) {
    if (fileSize > Config::getMaxFileSize()) {
        errorMessage = "文件大小超过限制: " + 
                      std::to_string(fileSize / 1024 / 1024) + "MB > " +
                      std::to_string(Config::getMaxFileSize() / 1024 / 1024) + "MB";
        return false;
    }
    return true;
}

bool FileValidator::validateChecksum(const std::string& content, 
                                  const std::string& expectedHash,
                                  std::string& errorMessage) {
    if (expectedHash.empty()) {
        return true; // 没有提供校验和，跳过验证
    }

    // TODO: 实现文件校验和验证
    return true;
}

FileUploadHandler::FileUploadHandler(const fs::path& baseUploadDir, std::shared_ptr<Logger> logger)
    : baseUploadDir_(baseUploadDir)
    , logger_(logger) {}

void FileUploadHandler::handleUpload(const httplib::Request& req, httplib::Response& res) {
    logger_->info("收到新的上传请求");

    if (!validateRequest(req, res)) {
        return;
    }

    auto file = req.get_file_value("file");
    fs::path savePath = req.has_param("filepath") ? 
                       req.get_param_value("filepath") : 
                       file.filename;
                       
    logger_->info("文件名: " + file.filename + ", 大小: " + 
                 std::to_string(file.content.size() / 1024) + "KB");

    std::string errorMessage;
    if (!FileValidator::isSafePath(savePath, errorMessage)) {
        res.status = 400;
        res.set_content(errorMessage, "text/plain; charset=utf-8");
        return;
    }

    if (!FileValidator::isAllowedFileType(file.filename, errorMessage)) {
        res.status = 400;
        res.set_content(errorMessage, "text/plain; charset=utf-8");
        return;
    }

    if (!FileValidator::isValidFileSize(file.content.size(), errorMessage)) {
        res.status = 400;
        res.set_content(errorMessage, "text/plain; charset=utf-8");
        return;
    }

    fs::path targetPath = baseUploadDir_ / savePath;

    if (!prepareUploadDirectory(targetPath.parent_path(), res)) {
        return;
    }

    if (!saveUploadedFile(file, targetPath, res)) {
        return;
    }

    res.set_content("文件上传成功: " + targetPath.string(), "text/plain; charset=utf-8");
}

bool FileUploadHandler::validateRequest(const httplib::Request& req, httplib::Response& res) {
    if (!req.has_file("file")) {
        res.status = 400;
        res.set_content("未找到上传的文件", "text/plain; charset=utf-8");
        return false;
    }
    return true;
}

bool FileUploadHandler::prepareUploadDirectory(const fs::path& path, httplib::Response& res) {
    try {
        fs::create_directories(path);
        return true;
    } catch (const fs::filesystem_error& e) {
        res.status = 500;
        res.set_content("创建目录失败: " + std::string(e.what()), "text/plain; charset=utf-8");
        return false;
    }
}

bool FileUploadHandler::saveUploadedFile(const httplib::MultipartFormData& file, 
                                       const fs::path& path, 
                                       httplib::Response& res) {
    try {
        std::ofstream ofs(path, std::ios::binary);
        if (!ofs) {
            throw std::runtime_error("无法创建文件");
        }
        
        ofs.write(file.content.data(), file.content.size());
        ofs.close();
        
        return true;
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content("保存文件失败: " + std::string(e.what()), "text/plain; charset=utf-8");
        return false;
    }
}


