#include "fsutils.windows.h"
#include "beatmap_types.h"
#include <fstream>

namespace {

bool isValidDirectory(const std::filesystem::path& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

} // anonymous namespace

std::vector<std::string> getBeatmapFolderNames(const fs::path& songsFolderPath)
{
    if (!isValidDirectory(songsFolderPath))
        return {"no data"};
        
    std::vector<std::string> beatmapFolderNames;
    for (const auto& entry : fs::directory_iterator(songsFolderPath))
    {
        if (entry.is_directory())
            beatmapFolderNames.push_back(entry.path().filename().string());
    }
    return beatmapFolderNames;
}

void writeBeatmapDataToFile(const std::vector<osu::BeatmapInfo>& data, const fs::path& filePath)
{
    std::ofstream(filePath) << nlohmann::json(data).dump(4);
}