#include"archiveManager.hpp"
#include "3rdpartyInclude/nlohmann/json.hpp"
using namespace std;

vector<beatmapSetAttribte> archiveManager::getAllBeatmaps()
{
    vector<beatmapSetAttribte> ret;
    for(auto kv:this->beatmapSets)
        ret.emplace_back(kv);
    return ret;
}

bool archiveManager::queryBeatmapSet(string bsid)
{
    for(auto kv:this->beatmapSets)
        if(kv.beatmapSetId==bsid)
            return true;
    return false;
}

void archiveManager::addBeatmapSet(beatmapSetAttribte bs)
{
    this->beatmapSets.insert(bs);
    return;
}

void archiveManager::addBeatmapSet(set<beatmapSetAttribte> bSet)
{
    for(auto kv:bSet)
    {
        if(this->beatmapSets.contains(kv))
            continue;
        this->beatmapSets.insert(kv);
    }
    return;
}

void archiveManager::addBeatmapSet(vector<beatmapSetAttribte> bSet)
{
    for(auto kv:bSet)
    {
        if(this->beatmapSets.contains(kv))
            continue;
        this->beatmapSets.insert(kv);
    }
    return;
}

void archiveManager::replaceBeatmapSets(vector<beatmapSetAttribte> bSets)
{
    this->beatmapSets.clear();
    for(auto kv:bSets)
        this->beatmapSets.insert(kv);
    return;
}

void archiveManager::replaceBeatmapSets(set<beatmapSetAttribte> bSets)
{
    this->beatmapSets=bSets;
    return;
}

inline set<beatmapSetAttribte> convert(vector<beatmapSetAttribte> a)
{
    set<beatmapSetAttribte> sb;
    for(auto kv:a)
        sb.insert(kv);
    return sb;
}

set<beatmapSetAttribte> archiveManager::mergeBeatmapSets(vector<beatmapSetAttribte> bsidlist,mergeMode mode)
{
    if(mode==given)
        return convert(bsidlist);
    else if(mode==current)
        return this->beatmapSets;
}

errorCode archiveManager::writeCurSetToFile(fs::path archivePath)
{
    try {
        std::ofstream outFile(archivePath, std::ios::out);
        if (!outFile.is_open()) {
            return errorCode::readFileFail;
        }

        nlohmann::json j = nlohmann::json::array();
        for (const auto& beatmapSet : beatmapSets) {
            nlohmann::json bsJson;
            to_json(bsJson, beatmapSet);  // 使用已定义的 to_json 函数
            j.push_back(bsJson);
        }

        outFile << j.dump(4);  // 使用4空格缩进格式化JSON
        outFile.close();
        return errorCode::ok;
    } catch (...) {
        return errorCode::readFileFail;
    }
}

errorCode archiveManager::writeCurSetToFile()
{
    return writeCurSetToFile(this->archiveFile);
}