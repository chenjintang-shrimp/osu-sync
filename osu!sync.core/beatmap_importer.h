#pragma once
#include "beatmap_types.h"
#include <filesystem>
#include <memory>
#include <future>
#include <mutex>
#include "network.utils.h"

namespace fs = std::filesystem;

namespace osu {

class BeatmapImporter {
public:
    // 构造函数，可以指定并发下载数
    explicit BeatmapImporter(const fs::path& savePath, size_t maxConcurrent = 3);
    
    // 从JSON文件导入谱面列表
    ImportStatus importFromJson(const std::string& jsonPath);
    
    // 从JSON字符串导入谱面列表
    ImportStatus importFromJsonString(const std::string& jsonContent);
    
    // 设置下载镜像
    void setMirror(const std::string& mirrorName) { currentMirror_ = mirrorName; }
    
    // 设置osu!安装目录
    void setOsuPath(const fs::path& osuPath) { osuPath_ = osuPath; }
    
    // 设置最大并发下载数
    void setMaxConcurrent(size_t maxConcurrent) { maxConcurrent_ = maxConcurrent; }
    
    // 获取当前状态
    const ImportStatus& getStatus() const { return status_; }

private:
    fs::path savePath_;         // 谱面保存路径
    fs::path osuPath_;          // osu!安装目录
    std::string currentMirror_; // 当前使用的下载镜像
    ImportStatus status_;       // 导入状态
    size_t maxConcurrent_;     // 最大并发下载数
    std::mutex statusMutex_;   // 用于保护状态更新
    
    // 验证并确保保存路径存在
    bool validateSavePath();
    
    // 将下载的谱面导入到osu的Songs文件夹
    bool importToOsuFolder(const BeatmapInfo& beatmap);

    // 下载单个谱面的任务
    void downloadTask(BeatmapInfo& beatmap, const DownloadOptions& options);
};

}
