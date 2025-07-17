#include "osuInstance.hpp"
#include "systemUtils.hpp"
#include <cstdlib>
#include <algorithm>
#include<thread>
#include <mutex>
#include <queue>

namespace fs = std::filesystem;
using namespace std;

inline bool isNumber(const string& str)
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

pair<errorCode, vector<string>> osuInstance::getAllBeatmapSets(const readMethod method) const
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
            for (const auto& entry : fs::directory_iterator(songsFolderPath))
            {
                if (entry.is_directory())
                {
                    string bsFolderName = entry.path().filename().string();
                    // 找到第一个空格
                    string bsid = bsFolderName.substr(0, bsFolderName.find(' '));
                    if (isNumber(bsid))
                        bslists.push_back(bsid);
                }
            }
            break;
        }

    default:
        break;
    }
    ranges::sort(bslists);
    return {ok, bslists};
}

static pair<errorCode, string> downloadBeatmapSet(const string& bsid, const fs::path& downloadPath)
{
    const string command = "wget -P " + downloadPath.generic_string() +
        "https://txy1.sayobot.cn/beatmaps/download/novideo/" + bsid + "?server=auto";
    auto res = executeCommand(command);
}

errorCode osuInstance::downloadBeatmaps(const std::vector<beatmapSetAttribte>& beatmaps)
{
    std::mutex queue_mutex;
    std::queue<std::string> bsid_queue;

    // 将 beatmap ID 加入队列
    for (const auto& beatmap : beatmaps)
    {
        bsid_queue.push(beatmap.beatmapSetId);
    }

    // 下载路径
    const fs::path downloadPath = this->osuFolderPath / "Songs";

    // 线程函数
    auto download_worker = [&]()
    {
        while (true)
        {
            std::string bsid;

            // 获取一个 beatmap ID
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (bsid_queue.empty()) break;
                bsid = bsid_queue.front();
                bsid_queue.pop();
            }

            // 下载该 beatmap
            downloadBeatmapSet(bsid, downloadPath);
        }
    };

    // 创建 4 个线程
    const int thread_count = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < thread_count; ++i)
    {
        threads.emplace_back(download_worker);
    }

    // 等待所有线程完成
    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    return ok;
}

pair<errorCode, beatmapSetAttribte> osuInstance::getBeatmapSetDetails(const string& bsid) const
{
    const string dbPath = (this->osuFolderPath / "osu!.db").generic_string();
    const string command = "stbDbReaderCore --details " + bsid + " " + dbPath;
    auto [fst, snd] = executeCommand(command);
    if (fst == ok)
    {
        const string result = snd;
        vector<string> tmp;
        string tmps;
        for (const auto kv : result)
        {
            if (kv == '\n')
            {
                tmp.push_back(tmps);
                tmps = "";
                continue;
            }
            tmps += kv;
        }
        return {ok, beatmapSetAttribte{bsid, tmp[0], tmp[1], tmp[2]}};
    }
    else
        return {fst, beatmapSetAttribte()};
}
