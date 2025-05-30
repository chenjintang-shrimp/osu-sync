#include "collection_downloader.h"
#include "3rdpartyInclude/httplib.h"
#include <stdexcept>
#include <iostream>

namespace osu
{

    std::vector<BeatmapInfo> CollectionDownloader::downloadBeatmapList(
        const std::string &username,
        const std::string &serverUrl)
    {

        std::vector<BeatmapInfo> beatmaps;

        try
        {
            // 解析服务器URL
            httplib::Client cli(serverUrl);

            // 构建下载路径
            std::string path = "/download/" + username + "/" + username + ".json";

            // 发送GET请求
            auto res = cli.Get(path);

            if (!res)
            {
                throw std::runtime_error("无法连接到服务器");
            }

            validateServerResponse(res->status, path);

            // 解析JSON响应
            auto j = json::parse(res->body);
            beatmaps = j.get<std::vector<BeatmapInfo>>();

            std::cout << "成功从服务器获取谱面列表，共 " << beatmaps.size() << " 个谱面" << std::endl;
        }
        catch (const json::exception &e)
        {
            throw std::runtime_error("解析服务器响应失败: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("下载谱面列表失败: " + std::string(e.what()));
        }

        return beatmaps;
    }

    void CollectionDownloader::validateServerResponse(const int status, const std::string &path)
    {
        if (status != 200)
        {
            throw std::runtime_error("下载失败，状态码: " + std::to_string(status) + "，路径: " + path);
        }
    }
}