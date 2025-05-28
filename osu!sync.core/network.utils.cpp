#include "network.utils.h"
#include <cstdlib>
#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <filesystem>

namespace osu {

// 执行命令并获取输出
std::string executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
    #ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
    #else
    FILE* pipe = popen(command.c_str(), "r");
    #endif
    
    if (!pipe) {
        throw std::runtime_error("执行命令失败");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    #ifdef _WIN32
    int exitCode = _pclose(pipe);
    #else
    int exitCode = pclose(pipe);
    #endif
    
    if (exitCode != 0) {
        throw std::runtime_error("命令执行失败，退出代码: " + std::to_string(exitCode));
    }
    
    return result;
}

bool NetworkUtils::downloadFile(const std::string& url, 
                              const fs::path& savePath, 
                              const DownloadOptions& options) {
    try {
        // 提取beatmap ID从URL
        std::string beatmapId;
        for (const auto& mirror : getMirrors()) {
            if (url.find(mirror.second.baseUrl) != std::string::npos) {
                auto pos = url.find(mirror.second.baseUrl);
                beatmapId = url.substr(pos + mirror.second.baseUrl.length());
                // 移除可能的查询参数
                auto queryPos = beatmapId.find('?');
                if (queryPos != std::string::npos) {
                    beatmapId = beatmapId.substr(0, queryPos);
                }
                break;
            }
        }
        
        if (beatmapId.empty()) {
            throw std::runtime_error("无法从URL提取beatmap ID");
        }
        
        // 构建下载器命令
        std::stringstream cmd;
        
        // 获取当前工作目录下的beatmapDownloader路径
        fs::path downloaderPath = fs::current_path() / "beatmapDownloader";
        #ifdef _WIN32
        downloaderPath.replace_extension(".exe");
        #endif
        
        // 检查下载器是否存在
        if (!fs::exists(downloaderPath)) {
            throw std::runtime_error("找不到beatmapDownloader工具: " + downloaderPath.string());
        }
        
        // 构建命令
        cmd << downloaderPath.string();

            // 设置超时
            client->set_connection_timeout(TIMEOUT_SECONDS);
            client->set_read_timeout(TIMEOUT_SECONDS);

            // 检查是否存在未完成的下载
            size_t startPos = 0;
            if (fs::exists(savePath)) {
                startPos = fs::file_size(savePath);
                if (startPos > 0) {
                    return resumeDownload(url, savePath, startPos, progressCallback, downloadedSize);
                }
            }

            // 获取文件大小
            size_t contentLength = getFileSize(url);
            if (contentLength == 0) {
                std::cerr << "无法获取文件大小" << std::endl;
                return false;
            }

            // 创建下载上下文
            std::ofstream file(savePath, std::ios::binary);
            if (!file) {
                std::cerr << "无法创建文件: " << savePath << std::endl;
                return false;
            }

            DownloadContext context{
                &file,
                progressCallback,
                contentLength,
                0
            };

            // 发起下载请求
            std::string path;
            std::string host;
            bool isHttps;
            if (!parseUrl(url, host, path, isHttps)) {
                std::cerr << "无效的URL: " << url << std::endl;
                return false;
            }

            auto res = client->Get(path.c_str(), 
                [&](const char* data, size_t size) {
                    return handleDownloadChunk(data, size, context);
                });

            if (res.error() != httplib::Error::Success) {
                std::cerr << "下载失败: " << httplib::to_string(res.error()) << std::endl;
                return false;
            }

            if (downloadedSize) {
                *downloadedSize = context.downloadedSize;
            }

            return true;

        } catch (const std::exception& e) {
            std::cerr << "下载出错 (重试 " << retryCount + 1 << "/" << MAX_RETRIES << "): " 
                      << e.what() << std::endl;
            
            if (++retryCount < MAX_RETRIES) {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }
        }
    }

    return false;
}

static size_t writeOszToFile(void* data, size_t size, size_t nmenb, void* userp)
{
    ofstream* out_file = static_cast<ofstream*>(userp);
    if (out_file && out_file->is_open())
    {
        out_file->write(static_cast<char*>(data), size * nmenb);
        return size * nmenb;
    }
    return 0;
}

vector<string> downloadBeatmapFromList(fs::path pathToBeatmapList,fs::path downloadDir)
{
    vector<string> bmIdlist;
    ifstream bmList(pathToBeatmapList);
    if (!bmList.is_open())
    {
        cerr << "打开文件失败：" << endl;
        exit(-1);
    }
    json jList = json::parse(bmList);
    for (auto kv : jList)
    {
        CURL* curl = curl_easy_init();
        if (!curl)
        {
            cerr << "curl初始化失败" << endl;
            exit(-1);
        }
        //cout << kv << endl;
        string fullName = kv["fullName"];
        string onlineId = kv["onlineId"];
        if (onlineId == "-1") continue;
        ofstream out_file(downloadDir / (onlineId + ".osz"), ios::binary);
        if (!out_file.is_open())
        {
            cerr << "无法打开文件：" << onlineId << ".osz" << endl;
            curl_easy_cleanup(curl);
            exit(-1);
        }
        //cout << "https://b10.sayobot.cn:25225" + getSayobotMirrorURL(onlineId) << endl;
        curl_easy_setopt(curl, CURLOPT_PORT, 25225);
        //curl_easy_setopt(curl, CURLOPT_URL, "https://b10.sayobot.cn:25225" + getSayobotMirrorURL(onlineId));
        //curl_easy_escape(curl, ("https://b10.sayobot.cn:25225" + getSayobotMirrorURL(onlineId)).c_str();
        string URLpath = "https://b10.sayobot.cn/"+getSayobotMirrorURL(onlineId);
        curl_easy_setopt(curl, CURLOPT_URL, URLpath);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeOszToFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  // ������ϸ���v
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            cerr << "下载失败：" << curl_easy_strerror(res) << endl;
        else
            cout << "谱面" << fullName << "下载成功" << endl;
        out_file.close();
        curl_easy_cleanup(curl);
    }
    return bmIdlist;
}

bool NetworkUtils::resumeDownload(const std::string& url, 
                                const fs::path& savePath,
                                size_t startPosition,
                                ProgressCallback progressCallback,
                                size_t* downloadedSize) {
    auto client = createHttpClient(url);
    if (!client) {
        return false;
    }

    std::string path;
    std::string host;
    bool isHttps;
    if (!parseUrl(url, host, path, isHttps)) {
        return false;
    }

    // 打开文件进行追加
    std::ofstream file(savePath, std::ios::binary | std::ios::app);
    if (!file) {
        return false;
    }

    // 设置断点续传的请求头
    httplib::Headers headers = {
        {"Range", "bytes=" + std::to_string(startPosition) + "-"}
    };

    size_t contentLength = getFileSize(url);
    DownloadContext context{
        &file,
        progressCallback,
        contentLength,
        startPosition
    };

    auto res = client->Get(path.c_str(), headers,
        [&](const char* data, size_t size) {
            return handleDownloadChunk(data, size, context);
        });

    if (res.error() != httplib::Error::Success) {
        return false;
    }

    if (downloadedSize) {
        *downloadedSize = context.downloadedSize;
    }

    return true;
}

bool NetworkUtils::validateDownloadedFile(const fs::path& filePath) {
    if (!fs::exists(filePath)) {
        return false;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false;
    }

    // 验证文件大小
    auto fileSize = fs::file_size(filePath);
    if (fileSize == 0) {
        return false;
    }

    // 检查.osz文件头（ZIP格式）
    char header[4];
    file.read(header, 4);
    return (header[0] == 0x50 && header[1] == 0x4B);  // ZIP文件标识
}

bool NetworkUtils::validateFile(const fs::path& filePath) {
    try {
        if (!fs::exists(filePath)) {
            return false;
        }

        // 验证文件大小
        auto fileSize = fs::file_size(filePath);
        if (fileSize == 0) {
            return false;
        }

        // 检查.osz文件头（ZIP格式）
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            return false;
        }

        char header[4];
        if (!file.read(header, 4)) {
            return false;
        }

        // 验证ZIP文件头
        return (header[0] == 0x50 && header[1] == 0x4B);

    } catch (const std::exception&) {
        return false;
    }
}

std::unique_ptr<httplib::Client> NetworkUtils::createHttpClient(const std::string& url) {
    std::string host;
    std::string path;
    bool isHttps;
    
    if (!parseUrl(url, host, path, isHttps)) {
        return nullptr;
    }

    #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    if (isHttps) {
        return std::make_unique<httplib::Client>(host);
    }
    #endif

    return std::make_unique<httplib::Client>(host);
}

size_t NetworkUtils::getFileSize(const std::string& url) {
    auto client = createHttpClient(url);
    if (!client) {
        return 0;
    }

    std::string path;
    std::string host;
    bool isHttps;
    if (!parseUrl(url, host, path, isHttps)) {
        return 0;
    }

    auto res = client->Head(path.c_str());
    if (res.error() != httplib::Error::Success) {
        return 0;
    }

    if (res->has_header("Content-Length")) {
        return std::stoull(res->get_header_value("Content-Length"));
    }

    return 0;
}

bool NetworkUtils::handleDownloadChunk(const char* data, 
                                     size_t size, 
                                     DownloadContext& context) {
    if (!context.fileStream || !context.fileStream->is_open()) {
        return false;
    }

    context.fileStream->write(data, size);
    context.downloadedSize += size;

    if (context.progressCallback) {
        context.progressCallback(context.downloadedSize, context.contentLength);
    }

    return true;
}

bool NetworkUtils::parseUrl(const std::string& url, 
                          std::string& host, 
                          std::string& path, 
                          bool& isHttps) {
    try {
        size_t protocolEnd = url.find("://");
        if (protocolEnd == std::string::npos) {
            return false;
        }

        std::string protocol = url.substr(0, protocolEnd);
        isHttps = (protocol == "https");

        size_t hostStart = protocolEnd + 3;
        size_t pathStart = url.find('/', hostStart);
        
        if (pathStart == std::string::npos) {
            host = url.substr(hostStart);
            path = "/";
        } else {
            host = url.substr(hostStart, pathStart - hostStart);
            path = url.substr(pathStart);
        }

        return true;
    } catch (...) {
        return false;
    }
}

std::string NetworkUtils::getSayobotMirrorURL(const std::string& beatmapId) {
    std::string basePath = "beatmaps";
    std::string ret = basePath;
    
    if (beatmapId.length() <= 4) {
        ret += "/0/" + beatmapId;
    } else {
        ret += "/" + beatmapId.substr(0, beatmapId.length() - 4) + 
               "/" + beatmapId.substr(beatmapId.length() - 4);
    }
    
    ret += "/full?filename=" + beatmapId;
    return ret;
}

std::unordered_map<std::string, NetworkUtils::Mirror> NetworkUtils::getMirrors() {
    static std::unordered_map<std::string, Mirror> mirrors = {
        {"sayobot", {"Sayobot", "https://txy1.sayobot.cn/download/beatmap/", false}},
        {"catboy", {"Catboy", "https://catboy.best/d/", false}},
        {"chimu", {"Chimu", "https://api.chimu.moe/v1/download/", false}},
        {"nerinyan", {"Nerinyan", "https://api.nerinyan.moe/d/", true}}
    };
    return mirrors;
}

} // namespace osu
