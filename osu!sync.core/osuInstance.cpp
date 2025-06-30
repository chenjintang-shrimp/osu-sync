#include "osuInstance.hpp"
#include "systemUtils.hpp"
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;
using namespace std;

inline bool isNumber(string str)
{
    bool flag = true;
    for (auto kv : str)
        if (!isdigit(kv))
        {
            flag = false;
            break;
        }
    return flag;
}

pair<errorCode, vector<string>> osuInstance::getAllBeatmapSets(readMethod method)
{
    vector<string> bslists;
    switch (method)
    {
    case db:
    {
        // 默认情况下readercore就在同一目录下
        string command = "stdDbReaderCore --list " + (osuFolderPath / "osu!.db").generic_string();
        pair<errorCode, string> res = executeCommand(command);
        if (res.first == ok)
        {
            string result = res.second;
            string tmp;
            for (auto kv : result)
            {
                if (kv == '\n')
                {
                    bslists.push_back(tmp);
                    tmp = "";
                    continue;
                }
                tmp += kv;
            }
            break;
        }
        else
            return {commandFail, vector<string>()};
    }

    case folder:
    {
        fs::path songsFolderPath = osuFolderPath / "Songs";
        for (const auto &entry : fs::directory_iterator(songsFolderPath))
        {
            if (entry.is_directory())
            {
                string bsFolderName = entry.path().filename().string();
                // 找到第一个空格
                string bsid = bsFolderName.substr(0, bsFolderName.find(" "));
                if (isNumber(bsid))
                    bslists.push_back(bsid);
            }
        }
        break;
    }

    default:
        break;
    }
    sort(bslists.begin(), bslists.end());
    return {ok, bslists};
}

pair<errorCode, beatmapSetAttribte> osuInstance::getBeatmapSetDetails(string bsid)
{
    string dbPath = (this->osuFolderPath / "osu!.db").generic_string();
    string command = "stbDbReaderCore --details " + bsid + " " + dbPath;
    pair<errorCode, string> res = executeCommand(command);
    if (res.first == ok)
    {
        string result=res.second;
        vector<string> tmp;
        string tmps;
        for (auto kv : result)
        {
            if (kv == '\n')
            {
                tmp.push_back(tmps);
                tmps = "";
                continue;
            }
            tmps += kv;
        }
        return {ok,beatmapSetAttribte{bsid,tmp[0],tmp[1],tmp[2]}};
    }
    else
        return {res.first,beatmapSetAttribte()};
}