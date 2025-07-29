#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <set>

#include "errcodes.hpp"
#include "beatmapSetAttribute.hpp"

namespace fs=std::filesystem;
enum readMethod{db,folder};

/*
    class BeatmapUtils:
*/

class osuInstance
{
private:
    fs::path osuFolderPath;
public:
    explicit osuInstance(const fs::path &osuFolderPath)
    {
        this->osuFolderPath=osuFolderPath;
    }
    std::pair<errorCode,std::vector<std::string>> getAllBeatmapSets(readMethod method) const;

    errorCode downloadBeatmaps(const std::vector<std::string>& beatmaps);
    errorCode downloadBeatmaps(const std::vector<std::string>& beatmaps,fs::path downloadPath);
    //只支持db读取
    std::pair<errorCode,beatmapSetAttribte> getBeatmapSetDetails(const std::string &bsid) const;
};