#include "beatmap_importer.h"
#include <fstream>
#include <iostream>

namespace osu {

BeatmapImporter::BeatmapImporter(const fs::path& savePath, size_t maxConcurrent)
    : savePath_(savePath)
    , currentMirror_("sayobot")
    , maxConcurrent_(maxConcurrent)
{
    validateSavePath();
}

ImportStatus BeatmapImporter::importFromJson(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        status_.errors.push_back("无法打开谱面列表文件: " + jsonPath);
        return status_;
    }
    
    std::string jsonContent((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    return importFromJsonString(jsonContent);
}

ImportStatus BeatmapImporter::importFromJsonString(const std::string& jsonContent) {
    try {
        // 重置状态
        status_ = ImportStatus{};
        
        // 解析JSON
        nlohmann::json j = nlohmann::json::parse(jsonContent);
        
        std::vector<BeatmapInfo> beatmaps;
        
        // 判断JSON格式（对象或数组）
        if (j.is_array()) {
            beatmaps = j.get<std::vector<BeatmapInfo>>();
            std::cout << "开始导入谱面列表" << std::endl;
        } else if (j.is_object()) {
            auto collection = j.get<BeatmapCollection>();
            beatmaps = collection.beatmaps;
            std::cout << "开始导入谱面合集: " << collection.name << std::endl;
            if (!collection.description.empty()) {
                std::cout << "描述: " << collection.description << std::endl;
            }
        } else {
            throw std::runtime_error("无效的JSON格式：必须是数组或对象");
        }
        
        status_.totalMaps = beatmaps.size();
        std::cout << "总计谱面数: " << status_.totalMaps << std::endl;
        
        // 验证并创建保存目录
        if (!validateSavePath()) {
            throw std::runtime_error("保存目录无效或无法创建");
        }

        // 收集需要下载的谱面ID
        std::vector<std::string> toDownload;
        std::vector<BeatmapInfo*> beatmapRefs;
        
        for (auto& beatmap : beatmaps) {
            // 构建保存路径
            fs::path beatmapPath = savePath_ / (beatmap.id + ".osz");
            
            // 检查是否已经下载
            if (fs::exists(beatmapPath) && fs::file_size(beatmapPath) > 0) {
                std::cout << "谱面已存在，跳过下载: " << beatmap.title 
                         << " (ID: " << beatmap.id << ")" << std::endl;
                beatmap.localPath = beatmapPath.string();
                beatmap.downloaded = true;
                status_.downloadedMaps++;
                continue;
            }
            
            toDownload.push_back(beatmap.id);
            beatmapRefs.push_back(&beatmap);
        }
        
        // 如果有需要下载的谱面
        if (!toDownload.empty()) {
            std::cout << "\n开始下载 " << toDownload.size() << " 个谱面..." << std::endl;
            
            // 配置下载选项
            DownloadOptions options;
            options.mirror = currentMirror_;
            options.concurrent = maxConcurrent_;
            
            // 批量下载所有谱面
            if (NetworkUtils::downloadFile(toDownload, savePath_, options)) {
                // 更新下载成功的谱面状态
                for (size_t i = 0; i < toDownload.size(); ++i) {
                    auto& beatmap = *beatmapRefs[i];
                    fs::path beatmapPath = savePath_ / (beatmap.id + ".osz");
                    
                    if (fs::exists(beatmapPath) && fs::file_size(beatmapPath) > 0) {
                        beatmap.localPath = beatmapPath.string();
                        beatmap.downloaded = true;
                        status_.downloadedMaps++;
                        
                        // 如果设置了osu安装目录，尝试导入
                        if (!osuPath_.empty()) {
                            if (importToOsuFolder(beatmap)) {
                                std::cout << "谱面已成功导入到osu!: " << beatmap.title << std::endl;
                            }
                        }
                    } else {
                        status_.failedMaps++;
                        status_.errors.push_back("下载失败: " + beatmap.title + " (ID: " + beatmap.id + ")");
                    }
                }
            } else {
                // 批量下载失败
                for (auto& beatmap : beatmapRefs) {
                    status_.failedMaps++;
                    status_.errors.push_back("下载失败: " + beatmap->title + " (ID: " + beatmap->id + ")");
                }
            }
        }
        
        // 更新总进度
        status_.currentProgress = 1.0;
        
    } catch (const std::exception& e) {
        status_.errors.push_back(std::string("解析谱面列表失败: ") + e.what());
    }
    
    return status_;
}

bool BeatmapImporter::validateSavePath() {
    try {
        if (!fs::exists(savePath_)) {
            fs::create_directories(savePath_);
        }
        return true;
    } catch (const std::exception& e) {
        status_.errors.push_back("创建保存目录失败: " + std::string(e.what()));
        return false;
    }
}

bool BeatmapImporter::importToOsuFolder(const BeatmapInfo& beatmap) {
    try {
        if (osuPath_.empty()) {
            throw std::runtime_error("未设置osu!安装目录");
        }

        // 检查谱面是否已下载
        if (!beatmap.downloaded || beatmap.localPath.empty()) {
            throw std::runtime_error("谱面未下载或下载失败");
        }

        // 构建目标路径（osu!/Songs/）
        fs::path songsPath = osuPath_ / "Songs";
        if (!fs::exists(songsPath)) {
            throw std::runtime_error("Songs文件夹不存在: " + songsPath.string());
        }

        // 获取源文件路径
        fs::path sourcePath = beatmap.localPath;
        if (!fs::exists(sourcePath)) {
            throw std::runtime_error("源文件不存在: " + sourcePath.string());
        }

        // 移动文件到Songs文件夹
        fs::path destPath = songsPath / sourcePath.filename();
        
        // 如果目标文件已存在，先删除
        if (fs::exists(destPath)) {
            fs::remove(destPath);
        }

        // 移动文件
        fs::rename(sourcePath, destPath);
        std::cout << "已导入谱面: " << sourcePath.filename().string() << std::endl;
        return true;

    } catch (const std::exception& e) {
        status_.errors.push_back("导入谱面失败: " + std::string(e.what()));
        return false;
    }
}

}