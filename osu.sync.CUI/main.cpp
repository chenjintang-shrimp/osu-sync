#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;
namespace fs = filesystem;

struct command
{
    string name;           // 命令名称
    string developer;      // 开发者名称
    string description;    // 命令描述
    fs::path baseFilePath; // 插件模块位置路径
    vector<string> args;           // 参数
};

bool operator<(const command &a, const command &b)
{
    return a.name < b.name; // 按名称排序
}

void from_json(const json &j, command &cmd)
{
    j.at("name").get_to(cmd.name);
    j.at("developer").get_to(cmd.developer);
    j.at("description").get_to(cmd.description);
    j.at("baseFilePath").get_to(cmd.baseFilePath);
    j.at("argsCnt").get_to(cmd.args);
}

void printVersion()
{
    const string verStr = "osu!sync.CUI, v0.0.1(b20250610),api v1";
    for (int i = 1; i <= verStr.size() + 2; i++)
        cout << "*";
    cout << endl
         << "*" << verStr << "*" << endl;
    for (int i = 1; i <= verStr.size() + 2; i++)
        cout << "*";
    cout << endl;
}

int main()
{
    printVersion();
    cout<<"开始解析 JSON 插件清单...\n";
    map<string,command> cmdMap;
    try
    {
        vector<command> commands;
        ifstream file("plugins.json");
        if(!file.is_open())
        {
            cerr<<"无法打开清单文件......"<<endl;
            return 1;
        }

        json j;
        file>>j;
        commands=j.get<vector<command>>();

        for(auto& kv:commands)
        {
            cout<<"已发现注册第三方命令："<<kv.name<<" by "<<kv.developer<<endl;
            cmdMap[kv.name]=kv;
        }

    }
    catch(const json::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}