/*
 * 网络工具类实现文件
 * 使用beatmapDownloader命令行工具实现下载功能
 */

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

std::string NetworkUtils::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
    #ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
    #else
    FILE* pipe = popen(command.c_str(), "r");
    #endif
    
    if (!pipe) {
        throw std::runtime_error("命令执行失败");
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
        throw std::runtime_error("命令执行失败，退出码: " + std::to_string(exitCode));
    }
    
    return result;
}

bool NetworkUtils::downloadFile(const std::string& url, 
                              const fs::path& savePath, 
                              const DownloadOptions& options) {
    try {
        // 从URL中提取beatmap ID
        std::string beatmapId;
        for (const auto& mirror : getMirrors()) {
            if (url.find(mirror.second.baseUrl) != std::string::npos) {
                auto pos = url.find(mirror.second.baseUrl);
                beatmapId = url.substr(pos + mirror.second.baseUrl.length());
                // Remove any query parameters
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

        // 构建下载器路径
        fs::path downloaderPath = fs::current_path() / "beatmapDownloader";
        #ifdef _WIN32
        downloaderPath.replace_extension(".exe");
        #endif
        
        if (!fs::exists(downloaderPath)) {
            throw std::runtime_error("找不到beatmapDownloader工具: " + downloaderPath.string());
        }

        // 构建命令
        std::stringstream cmd;
        cmd << downloaderPath.string()
            << " -id " << beatmapId
            << " -o " << savePath.string()
            << " -m " << options.mirror
            << " -t " << options.timeout;

        if (options.maxRetries > 0) {
            cmd << " -r " << options.maxRetries;
        }
            
        // 执行下载命令
        std::string output = executeCommand(cmd.str());
        
        // 验证下载的文件
        return validateFile(savePath);

    } catch (const std::exception& e) {
        std::cerr << "下载错误: " << e.what() << std::endl;
        return false;
    }
}

bool NetworkUtils::validateFile(const fs::path& filePath) {
    try {
        if (!fs::exists(filePath)) {
            return false;
        }

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

std::string NetworkUtils::getMirrorURL(const std::string& beatmapId, const std::string& mirror) {
    // 获取指定镜像站的配置
    auto mirrors = getMirrors();
    auto it = mirrors.find(mirror);
    if (it == mirrors.end()) {
        throw std::runtime_error("未知的镜像站: " + mirror);
    }

    const auto& mirrorConfig = it->second;
    std::string url = mirrorConfig.baseUrl + beatmapId;

    // 针对不同镜像站的特殊处理
    if (mirror == "sayobot") {
        // Sayobot特殊的路径格式
        std::string basePath = "beatmaps";
        if (beatmapId.length() <= 4) {
            url = basePath + "/0/" + beatmapId;
        } else {
            url = basePath + "/" + beatmapId.substr(0, beatmapId.length() - 4) + 
                  "/" + beatmapId.substr(beatmapId.length() - 4);
        }
        url += "/full?filename=" + beatmapId;
    }
    // 可以在这里添加其他镜像站的特殊处理逻辑

    return url;
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
