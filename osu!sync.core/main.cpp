#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <functional>
#include <locale>
#include <codecvt>
#include "stableExporter.h"
#include "collection_downloader.h"
#include "beatmap_importer.h"
#include "3rdpartyInclude/nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

// 版本信息
constexpr auto VERSION = "v0.1.0";
constexpr auto CODENAME = "Shiroko";
constexpr auto COPYRIGHT_YEAR = "2024";

// UTF-8 输出工具类
class UTF8Console {
public:
    static void init() {
        #if defined(_WIN32) || defined(_WIN64)
            // Windows: 设置控制台代码页为 UTF-8
            system("chcp 65001 > nul");
        #else
            // Unix-like systems: 设置 locale
            std::locale::global(std::locale("en_US.UTF-8"));
        #endif
        
        // 为 cout 设置 UTF-8 locale
        std::cout.imbue(std::locale());
        std::cerr.imbue(std::locale());
    }

    static void print(const std::string& text) {
        std::cout << text << std::endl;
    }

    static void error(const std::string& text) {
        std::cerr << text << std::endl;
    }
};

void printUsage()
{
    UTF8Console::print("osu!sync - osu!谱面同步工具");
    UTF8Console::print("版本: " + std::string(VERSION) + " (Codename " + CODENAME + ")");
    UTF8Console::print("使用方法: osu!sync <命令> [参数...]");
    UTF8Console::print("");
    UTF8Console::print("可用命令:");
    UTF8Console::print("  export <osu路径> <输出文件>     从osu!导出谱面列表到JSON文件");
    UTF8Console::print("  download <用户名> <服务器地址>  从服务器下载谱面列表");    UTF8Console::print("  import <谱面列表> <保存路径> [osu路径] [并发数]");
    UTF8Console::print("                                 下载并导入谱面列表中的谱面");
    UTF8Console::print("");
    UTF8Console::print("示例:");
    UTF8Console::print("  osu!sync export \"C:/Games/osu!\" beatmaps.json");
    UTF8Console::print("  osu!sync download player123 http://sync-server.com");
    UTF8Console::print("  osu!sync import beatmaps.json ./downloads \"C:/Games/osu!\" 16    # 使用16个并发");
    UTF8Console::print("  osu!sync import beatmaps.json ./downloads                      # 使用默认25个并发");
}

// 保存JSON到文件的通用函数
template <typename T>
void saveToJson(const T &data, const std::string &outputPath)
{
    std::ofstream outFile(outputPath);
    if (!outFile)
    {
        throw std::runtime_error("无法创建输出文件：" + outputPath);
    }
    json j = data;
    outFile << j.dump(4, ' ') << std::endl;
}

bool exportBeatmaps(const std::vector<std::string> &args)
{    if (args.size() != 2)
    {
        UTF8Console::error("错误: export命令需要osu路径和输出文件参数");
        return false;
    }

    const auto &[osuPath, outputPath] = std::tie(args[0], args[1]);

    try
    {
        fs::path songsPath = fs::path(osuPath) / "Songs";
        if (!fs::exists(songsPath))
        {            UTF8Console::error("错误: Songs文件夹不存在: " + songsPath.string());
            return false;
        }

        UTF8Console::print("正在从 " + songsPath.string() + " 导出谱面信息...");
        auto beatmaps = BeatmapExporter::exportBeatmaps(songsPath);
        UTF8Console::print("找到 " + std::to_string(beatmaps.size()) + " 个谱面");

        saveToJson(beatmaps, outputPath);        UTF8Console::print("谱面信息已保存到: " + outputPath);
        return true;
    }
    catch (const std::exception &e)
    {
        UTF8Console::error("导出谱面时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool downloadCollection(const std::vector<std::string> &args)
{    
    if (args.size() != 2)
    {
        UTF8Console::error("错误: download命令需要用户名和服务器地址参数");
        return false;
    }

    const auto &[username, serverUrl] = std::tie(args[0], args[1]);

    try
    {        UTF8Console::print("正在从服务器下载谱面列表...");
        auto beatmaps = osu::CollectionDownloader::downloadBeatmapList(username, serverUrl);

        std::string outputFile = username + "_collection.json";
        saveToJson(beatmaps, outputFile);

        UTF8Console::print("谱面列表已保存到: " + outputFile);
        return true;
    }    
    catch (const std::exception &e)
    {
        UTF8Console::error("下载谱面列表时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool importBeatmaps(const std::vector<std::string> &args)
{    
    if (args.size() < 2 || args.size() > 4)
    {
        UTF8Console::error("错误: import命令需要谱面列表文件和保存路径参数，可选osu路径和并发数");
        return false;
    }

    try
    {
        const auto &jsonPath = args[0];
        const auto &savePath = args[1];
        const auto &osuPath = args.size() >= 3 ? args[2] : "";
        
        // 解析并发数参数，默认25
        size_t concurrent = 25;
        if (args.size() >= 4) {
            try {
                concurrent = std::stoul(args[3]);
                if (concurrent < 1) {
                    UTF8Console::error("警告: 并发数必须大于0，将使用默认值25");
                    concurrent = 25;
                }
            } catch (const std::exception&) {
                UTF8Console::error("警告: 无效的并发数参数，将使用默认值25");
            }
        }        osu::BeatmapImporter importer(savePath, concurrent);
        if (!osuPath.empty())
        {
            importer.setOsuPath(osuPath);
        }
        UTF8Console::print("开始导入谱面... (并发数: " + std::to_string(concurrent) + ")");
        auto status = importer.importFromJson(jsonPath);

        UTF8Console::print("导入完成！");
        UTF8Console::print("总计谱面: " + std::to_string(status.totalMaps));
        UTF8Console::print("成功下载: " + std::to_string(status.downloadedMaps));
        UTF8Console::print("下载失败: " + std::to_string(status.failedMaps));        
        if (!status.errors.empty())
        {
            UTF8Console::print("\n错误信息:");
            for (const auto& error : status.errors)
            {
                UTF8Console::print("- " + error);
            }
        }
        return status.failedMaps == 0;
    }
    catch (const std::exception &e)
    {
        UTF8Console::error("导入谱面时发生错误: " + std::string(e.what()));
        return false;
    }
}

void printVersion()
{
    UTF8Console::print("osu!sync(core) " + std::string(VERSION) + " (Codename " + CODENAME + ")");
    UTF8Console::print("Copyright " + std::string(COPYRIGHT_YEAR) + " chen-shrimp");
    UTF8Console::print("本软件基于MIT协议发布，引用/更改记得署名哦~");
}

int main(int argc, char *argv[])
{
    UTF8Console::init();

    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    const std::string command = argv[1];
    std::vector<std::string> args(argv + 2, argv + argc);

    // 命令处理映射
    const std::map<std::string, std::function<bool(const std::vector<std::string> &)>> commandHandlers = {
        {"export", exportBeatmaps},
        {"download", downloadCollection},
        {"import", importBeatmaps}};

    try
    {
        if (command == "version")
        {
            printVersion();
            return 0;
        }

        auto it = commandHandlers.find(command);
        if (it == commandHandlers.end())
        {            UTF8Console::error("错误: 未知命令 '" + command + "'");
            printUsage();
            return 1;
        }

        return it->second(args) ? 0 : 1;
    }
    catch (const std::exception &e)
    {
        UTF8Console::error("发生意外错误: " + std::string(e.what()));
        return 1;
    }
}