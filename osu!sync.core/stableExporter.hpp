#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "3rdpartyInclude/nlohmann/json.hpp"
#include "beatmap_types.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;
using osu::BeatmapInfo;

class BeatmapExporter {
public:
    // 从osu! songs文件夹导出铺面信息
    static std::vector<BeatmapInfo> exportBeatmaps(const fs::path& songsPath);
    
    // 将铺面信息保存为JSON文件
    static void saveBeatmapsToJson(const std::vector<BeatmapInfo>& beatmaps, 
                                 const fs::path& outputPath);

private:
    // 从文件夹名称解析铺面信息
    static BeatmapInfo parseFolderName(const std::string& folderName);
    
    // 判断字符串是否为数字
    static bool isNumber(const std::string& str);
};
