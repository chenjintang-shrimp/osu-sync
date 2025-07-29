#include<iostream>
#include<filesystem>
#include"osuInstance.hpp"
#include "apiRequest.hpp"
#include "archiveManager.hpp"
using namespace std;

int main()
{
    system("chcp 65001");
    
    // 检查路径是否存在，如果不存在请修改为正确的路径
    std::filesystem::path osuPath = "D:\\osustb";
    std::filesystem::path jsonPath = "C:\\users\\jimmy\\osu.json";
    std::filesystem::path downloadPath = "C:\\users\\jimmy\\test";
    
    if (!std::filesystem::exists(osuPath)) {
        cout << "警告: osu! 路径不存在: " << osuPath << endl;
        // 可以选择退出或使用默认路径
    }
    
    if (!std::filesystem::exists(jsonPath.parent_path())) {
        cout << "警告: JSON 文件目录不存在: " << jsonPath.parent_path() << endl;
    }
    
    if (!std::filesystem::exists(downloadPath)) {
        if (!std::filesystem::create_directories(downloadPath)) {
            cout << "警告: 无法创建下载目录: " << downloadPath << endl;
        }
    }
    
    osuInstance osu(osuPath);
    apiRequest api;
    api.getToken();
    archiveManager archive(jsonPath);
    
    // 修复未定义的变量folder，使用db或folder枚举值
    auto res = osu.getAllBeatmapSets(folder);
    
    if (res.first != ok) {
        cout << "获取Beatmap集失败，错误代码: " << res.first << endl;
        return res.first;
    }
    
    // 检查下载路径
    if (!std::filesystem::exists(downloadPath)) {
        cout << "下载路径不存在: " << downloadPath << endl;
        return -1;
    }
    
    osu.downloadBeatmaps(res.second, downloadPath);
    cout<<"下载成功"<<endl;
    return 0;
}