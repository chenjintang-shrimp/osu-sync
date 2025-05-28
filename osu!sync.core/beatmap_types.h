#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace osu {

struct BeatmapInfo {
    std::string id;             // 谱面ID
    std::string title;          // 歌曲标题
    std::string artist;         // 艺术家
    std::string creator;        // 创建者
    std::string version;        // 难度版本
    std::string md5;           // 文件MD5校验值（可选）
    std::string localPath;     // 本地保存路径（可选）
    bool downloaded = false;    // 下载状态
};

struct BeatmapCollection {
    std::string name;           // 合集名称
    std::string description;    // 合集描述
    std::vector<BeatmapInfo> beatmaps;
};

// JSON序列化支持
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BeatmapInfo, id, title, artist, creator, version, md5, localPath, downloaded)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BeatmapCollection, name, description, beatmaps)

// 导入状态
struct ImportStatus {
    int totalMaps = 0;          // 总谱面数
    int downloadedMaps = 0;     // 已下载数量
    int failedMaps = 0;         // 失败数量
    double currentProgress = 0;  // 当前谱面下载进度 (0-1)
    std::vector<std::string> errors;  // 错误信息
};

}
