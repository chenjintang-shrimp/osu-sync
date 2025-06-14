#include <iostream>
#include <filesystem>
#include <locale>
#include <stdexcept>
#include <string>
#include "3rdparty/httplib.h"
#include "logger.hpp"
#include "server.hpp"

namespace fs = std::filesystem;

class Server {
public:
    Server() {
        // 设置UTF-8编码
        #ifdef _WIN32
            setlocale(LC_ALL, "zh_CN.UTF-8");
        #else
            setlocale(LC_ALL, "en_US.UTF-8");
            std::locale::global(std::locale("en_US.UTF-8"));
        #endif
        
        // 加载配置
        Config::load("config.json");
        
        // 初始化日志系统
        logger_ = std::make_shared<Logger>(Config::getLogDir());
        uploadHandler_ = std::make_unique<FileUploadHandler>(Config::getUploadDir(), logger_);
        downloadHandler_ = std::make_unique<FileDownloadHandler>(Config::getUploadDir(), logger_);
        
        setupRoutes();
        setupErrorHandlers();
    }
    
    void run() {
        logger_->info("服务器启动中...");
        logger_->info("监听地址: " + Config::getHost() + ":" + std::to_string(Config::getPort()));
        
        if (!server_.listen(Config::getHost(), Config::getPort())) {
            throw std::runtime_error("服务器启动失败");
        }
    }

private:
    void setupRoutes() {
        // 文件上传路由
        server_.Post("/upload", [this](const httplib::Request& req, httplib::Response& res) {
            logger_->info("收到上传请求");
            uploadHandler_->handleUpload(req, res);
            logger_->info("处理上传请求完成: " + std::to_string(res.status));
        });

        // 文件下载路由
        server_.Get(R"(/download/([^/]+)/([^/]+\.json))", [this](const httplib::Request& req, httplib::Response& res) {
            logger_->info("收到下载请求");
            downloadHandler_->handleDownload(req, res);
            logger_->info("处理下载请求完成: " + std::to_string(res.status));
        });

        // 健康检查路由
        server_.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("OK", "text/plain");
        });
    }
    
    void setupErrorHandlers() {
        server_.set_error_handler([this](const httplib::Request&, httplib::Response& res) {
            logger_->error("发生错误: " + std::to_string(res.status));
            res.set_content("服务器错误", "text/plain; charset=utf-8");
        });

        server_.set_exception_handler([this](const httplib::Request&, httplib::Response& res, std::exception_ptr ep) {
            try {
                std::rethrow_exception(ep);
            } catch (const std::exception& e) {
                logger_->error("发生异常: " + std::string(e.what()));
                res.status = 500;
                res.set_content("服务器内部错误", "text/plain; charset=utf-8");
            }
        });
    }

    httplib::Server server_;
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<FileUploadHandler> uploadHandler_;
    std::unique_ptr<FileDownloadHandler> downloadHandler_;
};

int main(int argc, char* argv[]) {
    try {
        Server server;

	        server.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "严重错误: " << e.what() << std::endl;
        return 1;
    }
}