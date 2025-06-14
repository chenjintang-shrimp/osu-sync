#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <functional>
#include <locale>
#include <codecvt>
#include "stableExporter.hpp"
#include "beatmap_types.hpp"
#include "beatmap_importer.hpp"
#include "3rdpartyInclude/nlohmann/json.hpp"
#include "network.utils.hpp"

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
    UTF8Console::print("  download <用户名> <服务器地址>  从服务器下载谱面列表");    
    UTF8Console::print("  import <谱面列表> <保存路径> [osu路径] [并发数] [--mirror <镜像站>]");
    UTF8Console::print("                                 下载并导入谱面列表中的谱面");
    UTF8Console::print("  mirrors                        列出所有可用的镜像站");
    UTF8Console::print("");
    UTF8Console::print("镜像站选项:");
    UTF8Console::print("  --mirror <镜像站>              指定下载使用的镜像站");
    UTF8Console::print("                                可选值: sayobot, catboy, chimu, nerinyan, kitsu");
    UTF8Console::print("");
    UTF8Console::print("示例:");
    UTF8Console::print("  osu!sync export \"C:/Games/osu!\" beatmaps.json");
    UTF8Console::print("  osu!sync download player123 http://sync-server.com");
    UTF8Console::print("  osu!sync import beatmaps.json ./downloads \"C:/Games/osu!\" 16 --mirror sayobot");
    UTF8Console::print("  osu!sync import beatmaps.json ./downloads --mirror chimu       # 使用默认25个并发");
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
{
    if (args.size() != 2)
    {
        UTF8Console::error("错误: export命令需要osu路径和输出文件参数");
        return false;
    }

    const auto &[osuPath, outputPath] = std::tie(args[0], args[1]);

    try
    {
        fs::path songsPath = fs::path(osuPath) / "Songs";
        if (!fs::exists(songsPath))
        {
            UTF8Console::error("错误: Songs文件夹不存在: " + songsPath.string());
            return false;
        }

        UTF8Console::print("正在从 " + songsPath.string() + " 导出谱面信息...");
        auto beatmaps = BeatmapExporter::exportBeatmaps(songsPath);
        UTF8Console::print("找到 " + std::to_string(beatmaps.size()) + " 个谱面");

        saveToJson(beatmaps, outputPath);
        UTF8Console::print("谱面信息已保存到: " + outputPath);
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
    // if (args.size() != 2)
    // {
    //     UTF8Console::error("错误: download命令需要用户名和服务器地址参数");
    //     return false;
    // }

    // const auto &[username, serverUrl] = std::tie(args[0], args[1]);

    // try
    // {
    //     UTF8Console::print("正在从服务器下载谱面列表...");
    //     auto beatmaps = osu::CollectionDownloader::downloadBeatmapList(username, serverUrl);

    //     std::string outputFile = username + "_collection.json";
    //     saveToJson(beatmaps, outputFile);

    //     UTF8Console::print("谱面列表已保存到: " + outputFile);
    //     return true;
    // }    
    // catch (const std::exception &e)
    // {
    //     UTF8Console::error("下载谱面列表时发生错误: " + std::string(e.what()));
    //     return false;
    // }
    return true;
}

bool importBeatmaps(const std::vector<std::string> &args)
{    
    if (args.empty())
    {
        UTF8Console::error("错误: import命令需要谱面列表文件和保存路径参数");
        return false;
    }

    try
    {
        std::string jsonPath;
        std::string savePath;
        std::string osuPath;
        std::string mirror;
        size_t concurrent = 25;

        // 解析参数
        for (size_t i = 0; i < args.size(); i++) {
            if (args[i] == "--mirror") {
                if (i + 1 >= args.size()) {
                    UTF8Console::error("错误: --mirror 选项需要指定镜像站名称");
                    return false;
                }
                mirror = args[++i];
                continue;
            }
            
            // 处理位置参数
            if (jsonPath.empty()) {
                jsonPath = args[i];
            } else if (savePath.empty()) {
                savePath = args[i];
            } else if (osuPath.empty()) {
                osuPath = args[i];
            } else {
                // 尝试解析为并发数
                try {
                    concurrent = std::stoul(args[i]);
                    if (concurrent < 1) {
                        UTF8Console::error("警告: 并发数必须大于0，将使用默认值25");
                        concurrent = 25;
                    }
                } catch (const std::exception&) {
                    UTF8Console::error("警告: 未知参数 '" + args[i] + "'，将被忽略");
                }
            }
        }

        // 验证必需参数
        if (jsonPath.empty() || savePath.empty()) {
            UTF8Console::error("错误: 需要指定谱面列表文件和保存路径");
            return false;
        }

        // 创建导入器实例
        osu::BeatmapImporter importer(savePath, concurrent);
        
        // 设置镜像站
        if (!mirror.empty()) {
            importer.setMirror(mirror);
            UTF8Console::print("使用镜像站: " + mirror);
        }
        
        // 设置osu路径
        if (!osuPath.empty()) {
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

bool listMirrors()
{
    try {
        auto mirrors = osu::NetworkUtils::getMirrors();
        UTF8Console::print("\n可用的镜像站:");
        for (const auto& [key, mirror] : mirrors) {
            if (key != "custom") {  // 不显示自定义镜像
                UTF8Console::print("  " + mirror.name + " (" + key + ")");
                UTF8Console::print("    基础URL: " + mirror.baseUrl);
                if (mirror.requiresNoskip) {
                    UTF8Console::print("    需要noskip参数");
                }
                UTF8Console::print("");
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        UTF8Console::error("获取镜像站列表时发生错误: " + std::string(e.what()));
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

    if (command == "version")
    {
        printVersion();
        return 0;
    }

    if (command == "mirrors")
    {
        return listMirrors() ? 0 : 1;
    }

    std::vector<std::string> args(argv + 2, argv + argc);

    if (command == "export") {
        return exportBeatmaps(args) ? 0 : 1;
    } else if (command == "download") {
        return downloadCollection(args) ? 0 : 1;
    } else if (command == "import") {
        return importBeatmaps(args) ? 0 : 1;
    } else {
        UTF8Console::error("错误: 未知命令 '" + command + "'");
        printUsage();
        return 1;
    }
}