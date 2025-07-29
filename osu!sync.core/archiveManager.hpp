#pragma once
#include <filesystem>
#include <set>
#include <vector>

#include "apiRequest.hpp"
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
    explicit archiveManager(const fs::path &archivePath)
    {
        archiveFile = archivePath;
    }
    [[nodiscard]] std::vector<beatmapSetAttribte> getAllBeatmaps() const;

    [[nodiscard]] bool queryBeatmapSet(const std::string &beatmapsetId) const;

    errorCode addBeatmapSet(std::string bsid, apiRequest& apiHandle);
    void addBeatmapSet(const beatmapSetAttribte &beatmapSet);
    void addBeatmapSet(const std::set<beatmapSetAttribte> &beatmapSets);
    void addBeatmapSet(const std::vector<beatmapSetAttribte> &bSet);
    errorCode addBeatmapSets(std::vector<std::string> bsidlist, apiRequest& apiHandle);
    void replaceBeatmapSets(const std::vector<beatmapSetAttribte> &beatmapSets);
    void replaceBeatmapSets(std::set<beatmapSetAttribte> beatmapSets);

    [[nodiscard]] errorCode writeCurSetToFile(const fs::path& archivePath) const;
    [[nodiscard]] errorCode writeCurSetToFile() const;
};