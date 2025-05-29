#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "stableExporter.h"
#include "collection_downloader.h"
#include "beatmap_importer.h"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

void printUsage() {
	std::cout << "osu!sync - osu!谱面同步工具" << std::endl;
	std::cout << "版本: v0.1.0 (Codename Shiroko)" << std::endl;
    std::cout << "使用方法: osu!sync <命令> [参数...]" << std::endl;
    std::cout << std::endl;
    std::cout << "可用命令:" << std::endl;
    std::cout << "  export <osu路径> <输出文件>     从osu!导出谱面列表到JSON文件" << std::endl;
    std::cout << "  download <用户名> <服务器地址>  从服务器下载谱面列表" << std::endl;
    std::cout << "  import <谱面列表> <保存路径> [osu路径]" << std::endl;
    std::cout << "                                 下载并导入谱面列表中的谱面" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  osu!sync export \"C:/Games/osu!\" beatmaps.json" << std::endl;
    std::cout << "  osu!sync download player123 http://sync-server.com" << std::endl;
    std::cout << "  osu!sync import beatmaps.json ./downloads \"C:/Games/osu!\"" << std::endl;
}

bool exportBeatmaps(const std::string& osuPath, const std::string& outputPath) {
    try {
        fs::path songsPath = fs::path(osuPath) / "Songs";
        if (!fs::exists(songsPath)) {
            std::cerr << "错误: Songs文件夹不存在: " << songsPath << std::endl;
            return false;
        }

        std::cout << "正在从 " << songsPath << " 导出谱面信息..." << std::endl;
        auto beatmaps = BeatmapExporter::exportBeatmaps(songsPath);
        std::cout << "找到 " << beatmaps.size() << " 个谱面" << std::endl;

        BeatmapExporter::saveBeatmapsToJson(beatmaps, outputPath);
        std::cout << "谱面信息已保存到: " << outputPath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return false;
    }
}

bool downloadCollection(const std::string& username, const std::string& serverUrl) {
    try {
        std::cout << "正在从服务器下载谱面列表..." << std::endl;
        auto beatmaps = osu::CollectionDownloader::downloadBeatmapList(username, serverUrl);
        
        // 保存到本地文件
        std::string outputFile = username + "_collection.json";
        
        // 创建JSON对象并保存
        std::ofstream outFile(outputFile);
        if (!outFile) {
            throw std::runtime_error("无法创建输出文件：" + outputFile);
        }
        
        // 使用nlohmann::json直接序列化
        json j = beatmaps;
        outFile << j.dump(4, ' ') << std::endl;
        
        std::cout << "谱面列表已保存到: " << outputFile << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return false;
    }
}

bool importBeatmaps(const std::string& jsonPath, const std::string& savePath, 
                   const std::string& osuPath = "") {
    try {
        osu::BeatmapImporter importer(savePath);
        if (!osuPath.empty()) {
            importer.setOsuPath(osuPath);
        }

        std::cout << "开始导入谱面..." << std::endl;
        auto status = importer.importFromJson(jsonPath);

        std::cout << "导入完成！" << std::endl;
        std::cout << "总计谱面: " << status.totalMaps << std::endl;
        std::cout << "成功下载: " << status.downloadedMaps << std::endl;
        std::cout << "下载失败: " << status.failedMaps << std::endl;

        if (!status.errors.empty()) {
            std::cout << "\n错误信息:" << std::endl;
            for (const auto& error : status.errors) {
                std::cout << "- " << error << std::endl;
            }
        }

        return status.failedMaps == 0;

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];

    try {
        if (command == "export") {
            if (argc != 4) {
                std::cerr << "错误: export命令需要osu路径和输出文件参数" << std::endl;
                printUsage();
                return 1;
            }
            return exportBeatmaps(argv[2], argv[3]) ? 0 : 1;
        }
        else if (command == "download") {
            if (argc != 4) {
                std::cerr << "错误: download命令需要用户名和服务器地址参数" << std::endl;
                printUsage();
                return 1;
            }
            return downloadCollection(argv[2], argv[3]) ? 0 : 1;
        }
        else if (command == "import") {
            if (argc != 4 && argc != 5) {
                std::cerr << "错误: import命令需要谱面列表文件和保存路径参数，可选osu路径" << std::endl;
                printUsage();
                return 1;
            }
            return importBeatmaps(argv[2], argv[3], argc == 5 ? argv[4] : "") ? 0 : 1;
        }
		else if(command=="version")
		{
			std::cout<<"osu!sync(core) v0.1.0 (Codename Shrioko)"<<std::endl;
			std::cout<<"Copyright 2025 chen-shrimp"<<std::endl;
			std::cout<<"本软件基于MIT协议发布，引用/更改记得署名哦~"<<std::endl;
			return 0;

		}
        else {
            std::cerr << "错误: 未知命令 '" << command << "'" << std::endl;
            printUsage();
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "发生错误: " << e.what() << std::endl;
        return 1;
    }
}