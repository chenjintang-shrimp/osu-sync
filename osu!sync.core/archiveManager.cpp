#include"archiveManager.hpp"
#include <utility>
#include <fstream>
#include "3rdpartyInclude/nlohmann/json.hpp"
#include "apiRequest.hpp"
using namespace std;

vector<beatmapSetAttribte> archiveManager::getAllBeatmaps() const {
    vector<beatmapSetAttribte> ret;
    for(const auto& kv:this->beatmapSets)
        ret.emplace_back(kv);
    return ret;
}

bool archiveManager::queryBeatmapSet(const string &beatmapsetId) const {
    for(const auto& kv:this->beatmapSets)
        if(kv.beatmapSetId==beatmapsetId)
            return true;
    return false;
}

errorCode archiveManager::addBeatmapSet(std::string bsid, apiRequest& apiHandle)
{
    pair<errorCode,beatmapSetAttribte> res=apiHandle.getBeatmapSetDetails(bsid);
    if (res.first!=ok)
        return networkError;
    else
        this->beatmapSets.insert(res.second);
}

void archiveManager::addBeatmapSet(const beatmapSetAttribte &beatmapSet)
{
    this->beatmapSets.insert(beatmapSet);
    return;
}

void archiveManager::addBeatmapSet(const set<beatmapSetAttribte> &beatmapSets)
{
    for(const auto& kv:beatmapSets)
    {
        if(this->beatmapSets.contains(kv))
            continue;
        this->beatmapSets.insert(kv);
    }
    return;
}

void archiveManager::addBeatmapSet(const vector<beatmapSetAttribte> &bSet)
{
    for(const auto& kv:bSet)
    {
        if(this->beatmapSets.contains(kv))
            continue;
        this->beatmapSets.insert(kv);
    }
    return;
}

errorCode archiveManager::addBeatmapSets(const std::vector<std::string> bsidlist,apiRequest &apiHandle)
{
    for (auto kv:bsidlist)
    {
        auto res=this->addBeatmapSet(kv,apiHandle);
        if (res!=ok)
            return res;
    }
    return ok;
}

void archiveManager::replaceBeatmapSets(const vector<beatmapSetAttribte> &bSets)
{
    this->beatmapSets.clear();
    for(const auto& kv:bSets)
        this->beatmapSets.insert(kv);
    return;
}

void archiveManager::replaceBeatmapSets(set<beatmapSetAttribte> beatmapSets)
{
    this->beatmapSets=std::move(beatmapSets);
    return;
}

inline set<beatmapSetAttribte> convert(const vector<beatmapSetAttribte> &a)
{
    set<beatmapSetAttribte> sb;
    for(const auto& kv:a)
        sb.insert(kv);
    return sb;
}

errorCode archiveManager::writeCurSetToFile(const fs::path& archivePath) const {
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

errorCode archiveManager::writeCurSetToFile() const
{
    return writeCurSetToFile(this->archiveFile);
}
