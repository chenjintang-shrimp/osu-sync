#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <vector>
#include "beatmapSetAttribute.hpp"
#include "errcodes.hpp"

namespace fs = std::filesystem;

enum mergeMode
{
    given,
    current,
    merge
}; // 取给定，取来自存档的，合并双方（即将一方没有的插入另一方）

class archiveManager
{
private:
    fs::path archiveFile;
    std::set<beatmapSetAttribte> beatmapSets;

public:
    archiveManager(fs::path archivePath)
    {
        archiveFile = archivePath;
    }
    std::vector<beatmapSetAttribte> getAllBeatmaps();

    bool queryBeatmapSet(std::string bsid);

    void addBeatmapSet(std::string bsid);//TODO:从osu!api取得谱面信息
    void addBeatmapSet(beatmapSetAttribte beatmapSet);
    void addBeatmapSet(std::set<beatmapSetAttribte> beatmapSets);
    void addBeatmapSet(std::vector<beatmapSetAttribte> beatmapSets);
    void replaceBeatmapSets(std::vector<beatmapSetAttribte> beatmapSets);
    void replaceBeatmapSets(std::set<beatmapSetAttribte> beatmapSets);

    std::set<beatmapSetAttribte> mergeBeatmapSets(std::vector<beatmapSetAttribte> bsidlist,mergeMode mode); // 返回合并后的谱面集

    errorCode writeCurSetToFile(fs::path archivePath);
    errorCode writeCurSetToFile();
};