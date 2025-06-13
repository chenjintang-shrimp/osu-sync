#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "network.utils.h"
#include "beatmap_types.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace osu {

class CollectionDownloader {
public:
    // 从服务器下载谱面清单
    static std::vector<BeatmapInfo> downloadBeatmapList(const std::string& username,const std::string& serverUrl);
private:
    // 检查服务器响应
    static void validateServerResponse(const int status, const std::string& path);
};
}
