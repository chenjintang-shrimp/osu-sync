#include <fstream>
#include <algorithm>
#include "stableExporter.h"

bool BeatmapExporter::isNumber(const std::string& str) {
    return !str.empty() && 
           std::all_of(str.begin(), str.end(), ::isdigit);
}

BeatmapInfo BeatmapExporter::parseFolderName(const std::string& folderName) {
    BeatmapInfo info;
    info.folderName = folderName;
    
    // 查找第一个空格的位置
    size_t spacePos = folderName.find_first_of(" ");
    if (spacePos == std::string::npos) {
        // 如果没有找到空格，整个文件夹名作为歌曲名
        info.onlineId = -1;
        info.songName = folderName;
        return info;
    }
    
    // 提取可能的在线ID
    std::string possibleId = folderName.substr(0, spacePos);
    info.onlineId = isNumber(possibleId) ? std::stoi(possibleId) : -1;
    
    // 提取歌曲名（空格后的所有内容）
    info.songName = folderName.substr(spacePos + 1);
    
    return info;
}

std::vector<BeatmapInfo> BeatmapExporter::exportBeatmaps(const fs::path& songsPath) {
    std::vector<BeatmapInfo> beatmaps;
    
    if (!fs::exists(songsPath) || !fs::is_directory(songsPath)) {
        return beatmaps;
    }
    
    // 遍历Songs文件夹下的所有子文件夹
    for (const auto& entry : fs::directory_iterator(songsPath)) {
        if (!entry.is_directory()) {
            continue;
        }
        
        // 解析文件夹名称
        BeatmapInfo info = parseFolderName(entry.path().filename().string());
        beatmaps.push_back(info);
    }
    
    return beatmaps;
}

void BeatmapExporter::saveBeatmapsToJson(const std::vector<BeatmapInfo>& beatmaps, 
                                        const fs::path& outputPath) {
    // 创建JSON对象
    json j = beatmaps;
    
    // 打开输出文件
    std::ofstream outFile(outputPath);
    if (!outFile) {
        throw std::runtime_error("无法创建输出文件：" + outputPath.string());
    }
    
    // 写入JSON（使用4个空格缩进）
    outFile << j.dump(4, ' ') << std::endl;
}





