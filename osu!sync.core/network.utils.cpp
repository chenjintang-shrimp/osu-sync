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

std::string NetworkUtils::executeCommand(const std::string& command, bool showOutput) {
    std::array<char, 1024> buffer;
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
        if (showOutput) {
            std::cout << buffer.data();
            std::cout.flush(); // 确保立即显示输出
        }
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
        // 构建aria2c路径
        fs::path aria2Path = "aria2c";
        
        // 检查aria2c是否可用
        std::string checkCommand;
        #ifdef _WIN32
        checkCommand = "where aria2c";
        #else
        checkCommand = "which aria2c";
        #endif
        
        try {
            executeCommand(checkCommand);
        } catch (const std::exception&) {
            throw std::runtime_error("未找到aria2c，请确保已安装并添加到PATH环境变量中");
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
        }        // 创建输入文件以存储下载URL
        fs::path inputFile = fs::temp_directory_path() / "aria2_input.txt";
        std::ofstream urlFile(inputFile);
        if (!urlFile) {
            throw std::runtime_error("无法创建下载输入文件");
        }

        // 为每个谱面ID生成下载URL并写入文件
        for (const auto& id : beatmapIds) {
            std::string url = getMirrorURL(id, options.mirror);
            urlFile << url << "\n\tout=" << id << ".osz\n";
        }
        urlFile.close();

        // 构建aria2c命令
        std::stringstream cmd;
        cmd << "aria2c"
            << " --input-file=\"" << inputFile.string() << "\""
            << " --dir=\"" << savePath.string() << "\""
            << " --max-concurrent-downloads=" << options.concurrent
            << " --max-connection-per-server=16"  // 每个服务器的最大连接数
            << " --min-split-size=1M"            // 最小文件分片大小
            << " --split=16"                      // 单文件最大连接数
            << " --max-tries=3"                   // 最大重试次数
            << " --retry-wait=3"                  // 重试等待时间
            << " --connect-timeout=10"            // 连接超时
            << " --allow-overwrite=true"          // 允许覆盖已存在的文件
            << " --auto-file-renaming=false"      // 禁止自动重命名
            << " --continue=true"                 // 支持断点续传
            << " --console-log-level=notice"      // 日志级别
            << " --summary-interval=1"            // 进度更新间隔
            << " --download-result=full";         // 显示详细的下载结果        // 执行下载命令并显示输出
        std::cout << "开始下载 " << beatmapIds.size() << " 个谱面...\n";
        std::string output = executeCommand(cmd.str(), true);

        // 下载完成后删除临时文件
        fs::remove(inputFile);
        
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
    std::string url;    // 针对不同镜像站的特殊处理
    std::string baseUrl = mirrorConfig.baseUrl;
    if (mirror == "sayobot") {
        // Sayobot特殊的路径格式：https://b2.sayobot.cn:25225/beatmaps/XXX/YYYY/full
        long long iBid=std::stoll(beatmapId);
        std::string firstPart=std::to_string((iBid/10000));
        std::string secondPart=beatmapId.substr(firstPart.size());
        url=baseUrl+firstPart+"/"+secondPart+"/full";
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

std::unordered_map<std::string, Mirror> NetworkUtils::getMirrors() {    static std::unordered_map<std::string, Mirror> mirrors = {
        {"sayobot", {"Sayobot", "https://b2.sayobot.cn:25225/beatmaps/", false}},
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