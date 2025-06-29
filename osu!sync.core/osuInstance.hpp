#pragma once
#include<iostream>
#include<filesystem>
#include<fstream>
#include<vector>
#include<map>
// #include<cstdio>

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
    osuInstance(fs::path osuFolderPath)
    {
        this->osuFolderPath=osuFolderPath;
    }
    std::vector<std::string> getAllBeatmapSets(readMethod method);
    
    //只支持db读取
    std::map<std::string,std::string> getBeatmapSetDetails(std::string bsid);
};