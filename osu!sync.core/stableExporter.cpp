#include <fstream>
#include <algorithm>
#include "stableExporter.h"
#include "httplib.h"
#include <sstream>

bool BeatmapExporter::isNumber(const std::string& str) {
    return !str.empty() && 
           std::all_of(str.begin(), str.end(), ::isdigit);
}

BeatmapInfo BeatmapExporter::parseFolderName(const std::string& folderName) {
    BeatmapInfo info;
    
    // 查找第一个空格的位置
    size_t spacePos = folderName.find_first_of(" ");
    if (spacePos == std::string::npos) {
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

std::vector<BeatmapInfo> BeatmapExporter::exportBeatmaps(const fs::path& songsPath) {
    std::vector<BeatmapInfo> beatmaps;
    
    if (!fs::exists(songsPath) || !fs::is_directory(songsPath)) {
        return beatmaps;
    }
    
    // 遍历Songs文件夹下的所有子文件夹
    for (const auto& entry : fs::directory_iterator(songsPath)) {
        if (!entry.is_directory()) {
            continue;
        }
        
        // 解析文件夹名称
        BeatmapInfo info = parseFolderName(entry.path().filename().string());
        beatmaps.push_back(info);
    }
    
    return beatmaps;
}

void BeatmapExporter::saveBeatmapsToJson(const std::vector<BeatmapInfo>& beatmaps, 
                                        const fs::path& outputPath) {
    // 创建JSON对象
    json j = beatmaps;
    
    // 打开输出文件
    std::ofstream outFile(outputPath);
    if (!outFile) {
        throw std::runtime_error("无法创建输出文件：" + outputPath.string());
    }
    
    // 写入JSON（使用4个空格缩进）
    outFile << j.dump(4, ' ') << std::endl;
}
        httplib::Client cli(serverUrl);
        
        // 构建下载路径
        std::string path = "/download/" + username + "/" + username + ".json";
        
        // 发送GET请求
        auto res = cli.Get(path);
        
        if (!res) {
            throw std::runtime_error("无法连接到服务器");
        }
        
        if (res->status != 200) {
            throw std::runtime_error("下载失败，状态码: " + std::to_string(res->status));
        }
        
        // 解析JSON响应
        auto j = json::parse(res->body);
        beatmaps = j.get<std::vector<BeatmapInfo>>();
        
        std::cout << "成功从服务器获取谱面列表，共 " << beatmaps.size() << " 个谱面" << std::endl;
        
    } catch (const json::exception& e) {
        throw std::runtime_error("解析服务器响应失败: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw std::runtime_error("下载谱面列表失败: " + std::string(e.what()));
    }
    
    return beatmaps;
}





