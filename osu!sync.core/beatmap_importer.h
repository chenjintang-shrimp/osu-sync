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

private:
    fs::path savePath_;
    std::string currentMirror_;
    ImportStatus status_;
    
    // 下载单个谱面
    bool downloadBeatmap(BeatmapInfo& beatmap);
    
    // 验证保存路径
    bool validateSavePath();
};
}
