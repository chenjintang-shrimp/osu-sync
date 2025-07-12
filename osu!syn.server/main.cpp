#include <iostream>
#include <filesystem>
#include <locale>
#include <stdexcept>
#include <string>
#include <utility>

#include "3rdparty/nlohmann/json.hpp"
#include "3rdparty/httplib.h"
#include "logger.hpp"
#include "server.hpp"

namespace fs = std::filesystem;

class Server
{
public:
    Server()
    {
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

    void run()
    {
        logger_->info("服务器启动中...");
        logger_->info("监听地址: " + Config::getHost() + ":" + std::to_string(Config::getPort()));

        if (!server_.listen(Config::getHost(), Config::getPort()))
        {
            throw std::runtime_error("服务器启动失败");
        }
    }

private:
    void setupRoutes()
    {
        // 文件上传路由
        server_.Post("/upload", [this](const httplib::Request& req, httplib::Response& res)
        {
            logger_->info("收到上传请求");
            uploadHandler_->handleUpload(req, res);
            logger_->info("处理上传请求完成: " + std::to_string(res.status));
        });

        // 文件下载路由
        server_.Get(R"(/download/([^/]+)/([^/]+\.json))", [this](const httplib::Request& req, httplib::Response& res)
        {
            logger_->info("收到下载请求");
            downloadHandler_->handleDownload(req, res);
            logger_->info("处理下载请求完成: " + std::to_string(res.status));
        });

        // 健康检查路由
        server_.Get("/health", [](const httplib::Request&, httplib::Response& res)
        {
            res.set_content("OK", "text/plain");
        });

        //oauth2请求路由
        server_.Get("/start-auth", [&](const httplib::Request&, httplib::Response& res)
        {
            logger_->info("收到oauth验证请求");
            const std::string client_id = Config::getOauthClientID();
            const std::string redirect_uri = Config::getOauthRedirectUri();
            const std::string scope = "public";
            const std::string auth_url = "http://osu.ppy.sh/oauth/authorize?client_id=" + client_id +
                "&redirect_uri=" + redirect_uri +
                "&response_type=code&scope=" + scope;
            res.set_content(auth_url, "text/plain");
        });

        //处理oauth2回调路由
        server_.Get("/callback", [&](const httplib::Request& req, httplib::Response& res)
        {
            logger_->info("接到oauth请求回调");
            auto code = req.get_param_value("code");
            if (code.empty())
            {
                logger_->error("缺少回调code");
                res.status = 400;
                res.set_content("缺少oauth回调code", "text/plain");
                return;
            }
            //向ppy请求token
            logger_->info("开始请求token");
            httplib::Client client("http://osu.ppy.sh");
            client.set_default_headers(
                {
                    {"Accept", "application/json"},
                    {"Content-Type", "x-www-form-urlencoded"}
                });
            httplib::Params params = {
                {"client_id", Config::getOauthClientID()},
                {"client_secret", Config::getOauthClientSecret()},
                {"code", code},
                {"grant_type", "authorization_code"},
                {"redirect_uri", Config::getOauthRedirectUri()}
                // {"scope","public"}
            };
            auto result = client.Post("/oauth/token", params);
            if (result && result->status == 200)
            {
                logger_->info("请求token成功");
                nlohmann::json json_response = nlohmann::json::parse(result->body);
                std::string token = json_response["access_token"].get<std::string>();
                int expired_time = json_response["expires_in"].get<int>();

                nlohmann::json response_json;
                response_json["access_token"] = token;
                response_json["expire_in"] = expired_time;

                std::string responseHTML;
                std::ifstream htmlFile("./tokenCopy.html");
                if (htmlFile.is_open())
                {
                    htmlFile>>responseHTML;
                    htmlFile.close();
                }
                else
                    throw std::runtime_error("无法打开tokenCopy.html");
                // size_t pos = responseHTML.find("{{JSON_CONTENT}}");
                // if (pos!=std::string::npos)
                //     responseHTML.replace(pos,16,response_json.dump(4));
                logger_->info("成功获得token;返回html:\n"+responseHTML);
                res.set_content(responseHTML, "text/html");

                return;
            }
            else
            {
                logger_->error("请求oauth token失败");
                logger_->error(result->body);
                res.status = 500;
                res.set_content("无法从osu!api拿到token，你被ppy做局了！", "text/plain");
            }
        });
    }

    void setupErrorHandlers()
    {
        server_.set_error_handler([this](const httplib::Request&, httplib::Response& res)
        {
            logger_->error("发生错误: " + std::to_string(res.status));
            res.set_content("服务器错误", "text/plain; charset=utf-8");
        });

        server_.set_exception_handler([this](const httplib::Request&, httplib::Response& res, std::exception_ptr ep)
        {
            try
            {
                std::rethrow_exception(std::move(ep));
            }
            catch (const std::exception& e)
            {
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

int main(int argc, char* argv[])
{
    try
    {
        Server server;

        server.run();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "严重错误: " << e.what() << std::endl;
        return 1;
    }
}
