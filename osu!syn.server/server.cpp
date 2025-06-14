#include "server.hpp"
#include "logger.hpp"
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
    // 允许所有文件类型
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

FileDownloadHandler::FileDownloadHandler(const fs::path& baseUploadDir, std::shared_ptr<Logger> logger)
    : baseUploadDir_(baseUploadDir)
    , logger_(logger) {}

void FileDownloadHandler::handleDownload(const httplib::Request& req, httplib::Response& res) {
    logger_->info("收到下载请求: " + req.path);
    
    std::string username = extractUsername(req.path);
    if (username.empty()) {
        res.status = 400;
        res.set_content("无效的请求路径", "text/plain; charset=utf-8");
        return;
    }

    fs::path jsonPath = buildJsonPath(username);
    
    // 检查文件是否存在
    if (!fs::exists(jsonPath)) {
        res.status = 404;
        res.set_content("文件未找到", "text/plain; charset=utf-8");
        return;
    }

    // 尝试读取文件
    try {
        std::ifstream file(jsonPath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("无法打开文件");
        }

        // 读取文件内容
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

        // 设置响应头
        res.set_header("Content-Type", "application/json");
        res.set_header("Content-Disposition", "attachment; filename=\"" + username + ".json\"");
        res.set_content(content, "application/json");
        
        logger_->info("文件下载成功: " + jsonPath.string());
    }
    catch (const std::exception& e) {
        logger_->error("文件下载失败: " + std::string(e.what()));
        res.status = 500;
        res.set_content("服务器内部错误", "text/plain; charset=utf-8");
    }
}

std::string FileDownloadHandler::extractUsername(const std::string& path) {
    // 预期格式: /download/username/username.json
    std::string prefix = "/download/";
    if (path.substr(0, prefix.length()) != prefix) {
        return "";
    }

    // 移除前缀
    std::string remaining = path.substr(prefix.length());
    
    // 按 '/' 分割
    size_t slashPos = remaining.find('/');
    if (slashPos == std::string::npos) {
        return "";
    }

    std::string username = remaining.substr(0, slashPos);
    std::string expectedSuffix = username + ".json";
    
    // 验证剩余部分是否匹配 username.json
    if (remaining.substr(slashPos + 1) != expectedSuffix) {
        return "";
    }

    return username;
}

fs::path FileDownloadHandler::buildJsonPath(const std::string& username) {
    return baseUploadDir_ / (username + ".json");
}


