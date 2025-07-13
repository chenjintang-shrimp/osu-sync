#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
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

    //只支持db读取
    std::pair<errorCode,beatmapSetAttribte> getBeatmapSetDetails(const std::string &bsid) const;
};