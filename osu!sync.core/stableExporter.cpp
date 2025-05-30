#include <fstream>
#include <iostream>
#include <algorithm>
#include "stableExporter.h"

bool BeatmapExporter::isNumber(const std::string& str)
{
    return !str.empty() && 
           std::all_of(str.begin(), str.end(), ::isdigit);
}

BeatmapInfo BeatmapExporter::parseFolderName(const std::string& folderName)
{
    BeatmapInfo info;

    // 查找第一个空格的位置
    size_t spacePos = folderName.find_first_of(" ");
    if (spacePos == std::string::npos)
    {
        // 如果没有找到空格，整个文件夹名作为歌曲标题
        info.id = "-1";
        info.title = folderName;
        return info;
    }

    // 提取可能的在线ID
    std::string possibleId = folderName.substr(0, spacePos);
    info.id = isNumber(possibleId) ? possibleId : "-1";

    // 提取歌曲标题（空格后的所有内容）
    info.title = folderName.substr(spacePos + 1);

    return info;
}

std::vector<BeatmapInfo> BeatmapExporter::exportBeatmaps(const fs::path& songsPath)
{
    if (!fs::exists(songsPath) || !fs::is_directory(songsPath))
    {
        throw std::runtime_error("歌曲文件夹不存在或不是有效目录：" + songsPath.string());
    }

    std::vector<BeatmapInfo> beatmaps;

    // 遍历Songs文件夹下的所有子文件夹
    for (const auto& entry : fs::directory_iterator(songsPath))
    {
        if (entry.is_directory())
        {
            try 
            {
                // 解析文件夹名称
                BeatmapInfo info = parseFolderName(entry.path().filename().string());
                beatmaps.push_back(std::move(info));
            }
            catch (const std::exception& e)
            {
                // 记录错误但继续处理其他文件夹
                std::cerr << "警告: 处理文件夹 " << entry.path().filename().string() 
                        << " 时出错: " << e.what() << std::endl;
            }
        }
    }

    if (beatmaps.empty())
    {
        throw std::runtime_error("未找到任何谱面");
    }

    return beatmaps;
}

void BeatmapExporter::saveBeatmapsToJson(const std::vector<BeatmapInfo>& beatmaps, 
                                        const fs::path& outputPath)
{
    try
    {
        // 创建JSON对象
        json j = beatmaps;

        // 创建输出目录（如果不存在）
        const auto parentPath = outputPath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath))
        {
            fs::create_directories(parentPath);
        }

        // 打开并写入文件
        std::ofstream outFile(outputPath);
        if (!outFile)
        {
            throw std::runtime_error("无法创建输出文件：" + outputPath.string());
        }

        // 写入JSON（使用4个空格缩进）
        outFile << j.dump(4, ' ') << std::endl;

        if (!outFile)
        {
            throw std::runtime_error("写入文件时发生错误：" + outputPath.string());
        }
    }
    catch (const json::exception& e)
    {
        throw std::runtime_error("JSON序列化失败: " + std::string(e.what()));
    }
}