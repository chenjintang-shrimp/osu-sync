#include "beatmap_importer.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

namespace osu {

BeatmapImporter::BeatmapImporter(const fs::path& savePath)
    : savePath_(savePath)
    , currentMirror_("sayobot") // 默认使用sayobot镜像
{
    validateSavePath();
}

ImportStatus BeatmapImporter::importFromJson(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        status_.errors.push_back("无法打开谱面列表文件: " + jsonPath);
        return status_;
    }
    
    std::string jsonContent((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    return importFromJsonString(jsonContent);
}

ImportStatus BeatmapImporter::importFromJsonString(const std::string& jsonContent) {
    try {
        // 重置状态
        status_ = ImportStatus{};
        
        // 解析JSON
        auto collection = nlohmann::json::parse(jsonContent).get<BeatmapCollection>();
        status_.totalMaps = collection.beatmaps.size();
        
        std::cout << "开始导入谱面合集: " << collection.name << std::endl;
        std::cout << "描述: " << collection.description << std::endl;
        std::cout << "总计谱面数: " << status_.totalMaps << std::endl;
        
        // 下载每个谱面
        for (auto& beatmap : collection.beatmaps) {
            if (downloadBeatmap(beatmap)) {
                status_.downloadedMaps++;
            } else {
                status_.failedMaps++;
            }
            updateProgress();
        }
        
    } catch (const std::exception& e) {
        status_.errors.push_back(std::string("解析谱面列表失败: ") + e.what());
    }
    
    return status_;
}

void BeatmapImporter::setProgressCallback(ProgressCallback callback) {
    progressCallback_ = callback;
}

void BeatmapImporter::setMirror(const std::string& mirrorName) {
    currentMirror_ = mirrorName;
}

bool BeatmapImporter::downloadBeatmap(BeatmapInfo& beatmap) {
    try {
        std::cout << "下载谱面: " << beatmap.title << " (" << beatmap.id << ")" << std::endl;
        
        // 构建保存路径
        fs::path beatmapPath = savePath_ / (beatmap.id + ".osz");
        
        // 根据当前镜像生成下载URL
        std::string url;
        if (currentMirror_ == "sayobot") {
            url = "https://txy1.sayobot.cn/download/beatmap/" + beatmap.id;
        } else if (currentMirror_ == "catboy") {
            url = "https://catboy.best/d/" + beatmap.id;
        } else if (currentMirror_ == "chimu") {
            url = "https://api.chimu.moe/v1/download/" + beatmap.id;
        } else if (currentMirror_ == "nerinyan") {
            url = "https://api.nerinyan.moe/d/" + beatmap.id + "?noskip=1";
        } else {
            throw std::runtime_error("未知的镜像站: " + currentMirror_);
        }
        
        // 使用新的网络工具下载文件
        size_t downloadedSize = 0;
        bool success = NetworkUtils::downloadFile(
            url, 
            beatmapPath,
            [this](size_t current, size_t total) {
                // 更新下载进度
                status_.currentProgress = static_cast<double>(current) / total;
                updateProgress();
            },
            &downloadedSize
        );
        
        if (!success) {
            throw std::runtime_error("下载失败");
        }
        
        // 验证下载的文件
        if (!NetworkUtils::validateDownloadedFile(beatmapPath)) {
            throw std::runtime_error("文件验证失败");
        }
        
        beatmap.localPath = beatmapPath.string();
        beatmap.downloaded = true;
        return true;
        
    } catch (const std::exception& e) {
        status_.errors.push_back("下载谱面 " + beatmap.id + " 失败: " + e.what());
        return false;
    }
}

void BeatmapImporter::updateProgress() {
    if (progressCallback_) {
        progressCallback_(status_);
    }
}

bool BeatmapImporter::validateSavePath() {
    try {
        if (!fs::exists(savePath_)) {
            fs::create_directories(savePath_);
        }
        return true;
    } catch (const std::exception& e) {
        status_.errors.push_back("创建保存目录失败: " + std::string(e.what()));
        return false;
    }
}
