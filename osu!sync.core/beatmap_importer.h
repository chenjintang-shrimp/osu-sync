#pragma once
#include "beatmap_types.h"
#include <filesystem>
#include <memory>
#include "network.utils.h"
#include <functional>

namespace fs = std::filesystem;

namespace osu {

class BeatmapImporter {
public:
    BeatmapImporter(const fs::path& savePath);
    
    // 从JSON文件导入谱面列表
    ImportStatus importFromJson(const std::string& jsonPath);
    
    // 从JSON字符串导入谱面列表
    ImportStatus importFromJsonString(const std::string& jsonContent);
    
    // 设置要使用的下载镜像
    void setMirror(const std::string& mirrorName);

    // 设置osu!安装目录
    void setOsuPath(const fs::path& osuPath) { osuPath_ = osuPath; }
    
    // 将下载的铺面导入到osu的Songs文件夹
    bool importToOsuFolder(const BeatmapInfo& beatmap);

private:
    fs::path savePath_;
    fs::path osuPath_;  // osu!安装目录
    std::string currentMirror_;
    ImportStatus status_;
    
    // 下载单个谱面
    bool downloadBeatmap(BeatmapInfo& beatmap);
    
    // 验证保存路径
    bool validateSavePath();
};
}
