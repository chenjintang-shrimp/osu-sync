/*
 * 网络工具类实现文件
 * 使用beatmapDownloader命令行工具实现下载功能
 */
#pragma warning(disable : 4996)
#include "network.utils.h"
#include<fstream>
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

bool NetworkUtils::downloadFile(const std::vector<std::string>& beatmapIds,
                              const fs::path& savePath,
                              const DownloadOptions& options) {
    try {
        // 构建下载器路径
        fs::path downloaderPath;
        
        // 首先检查当前目录
        downloaderPath = fs::current_path() / "beatmapDownloader";
        #ifdef _WIN32
        downloaderPath.replace_extension(".exe");
        #endif
        
        // 如果当前目录找不到，尝试相对于可执行文件的路径
        if (!fs::exists(downloaderPath)) {
            downloaderPath = fs::current_path().parent_path() / "beatmapDownloader";
            #ifdef _WIN32
            downloaderPath.replace_extension(".exe");
            #endif
        }
        
        if (!fs::exists(downloaderPath)) {
            throw std::runtime_error("找不到beatmapDownloader工具，已尝试路径: " + 
                fs::current_path().string() + " 和 " + 
                fs::current_path().parent_path().string());
        }

        // 创建保存目录
        if (!fs::exists(savePath)) {
            fs::create_directories(savePath);
        }

        // 将所有ID合并成一个字符串
        std::string combinedIds;
        for (size_t i = 0; i < beatmapIds.size(); ++i) {
            if (i > 0) combinedIds += ",";
            combinedIds += beatmapIds[i];
        }

        // 构建命令
        std::stringstream cmd;
        cmd << downloaderPath.string();
        
        // 添加镜像选项（如果指定）
        if (!options.mirror.empty()) {
            cmd << " --mirror " << options.mirror;
        }
        
        // 添加谱面ID、保存路径和并发数
        cmd << " \"" << combinedIds << "\" " 
            << "\"" << savePath.string() << "\" " 
            << options.concurrent;
            
        // 执行下载命令
        std::string output = executeCommand(cmd.str());
        
        // 验证每个谱面文件
        bool allSuccess = true;
        for (const auto& id : beatmapIds) {
            fs::path beatmapPath = savePath / (id + ".osz");
            if (!validateFile(beatmapPath)) {
                allSuccess = false;
            }
        }
        
        return allSuccess;

    } catch (const std::exception& e) {
        std::cerr << "下载错误: " << e.what() << std::endl;
        return false;
    }
}

bool NetworkUtils::validateFile(const fs::path& filePath) {
    try {
        if (!fs::exists(filePath)) {
            std::cerr << "文件不存在: " << filePath << std::endl;
            return false;
        }

        auto fileSize = fs::file_size(filePath);
        if (fileSize == 0) {
            std::cerr << "文件大小为0: " << filePath << std::endl;
            return false;
        }

        if (fileSize < 22) { // ZIP文件的最小大小
            std::cerr << "文件太小，不是有效的ZIP文件: " << filePath << std::endl;
            return false;
        }

        // 检查.osz文件头（ZIP格式）
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "无法打开文件: " << filePath << std::endl;
            return false;
        }

        // 读取并验证ZIP文件头（PK\x03\x04）
        char header[4];
        if (!file.read(header, 4)) {
            std::cerr << "无法读取文件头: " << filePath << std::endl;
            return false;
        }

        // 验证ZIP文件头
        if (!(header[0] == 0x50 && header[1] == 0x4B && 
              header[2] == 0x03 && header[3] == 0x04)) {
            std::cerr << "无效的ZIP文件头: " << filePath << std::endl;
            return false;
        }

        return true;

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
    std::string url;

    // 针对不同镜像站的特殊处理
    std::string baseUrl = mirrorConfig.baseUrl;
    if (mirror == "sayobot") {
        // Sayobot特殊的路径格式：https://txy1.sayobot.cn/download/beatmap/{set_id}/full
        if (beatmapId.length() <= 4) {
            url = baseUrl + beatmapId + "/full";
        } else {
            url = baseUrl + beatmapId + "/full";
        }
        url += "?filename=" + beatmapId + ".osz";
    } else {
        // 其他镜像站使用直接拼接方式
        url = baseUrl + beatmapId;
        // 对于需要noskip参数的镜像站
        if (mirrorConfig.requiresNoskip) {
            url += "?noskip=1";
        }
    }

    return url;
}

std::unordered_map<std::string, Mirror> NetworkUtils::getMirrors() {
    static std::unordered_map<std::string, Mirror> mirrors = {
        {"sayobot", {"Sayobot", "https://txy1.sayobot.cn/download/beatmap/", false}},
        {"catboy", {"Catboy", "https://catboy.best/d/", false}},
        {"chimu", {"Chimu", "https://api.chimu.moe/v1/download/", false}},
        {"nerinyan", {"Nerinyan", "https://api.nerinyan.moe/d/", true}},
        {"kitsu", {"Kitsu", "https://kitsu.moe/api/d/", false}}  // 添加新的镜像
    };
    
    // 检查环境变量是否有自定义镜像
    const char* customMirror = std::getenv("OSU_SYNC_MIRROR");
    if (customMirror != nullptr) {
        std::string mirrorUrl(customMirror);
        if (!mirrorUrl.empty()) {
            mirrors["custom"] = {"Custom", mirrorUrl, false};
        }
    }
    
    return mirrors;
}

} // namespace osu