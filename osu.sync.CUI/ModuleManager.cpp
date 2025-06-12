#include "ModuleManager.h"
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
using namespace std;
using namespace nlohmann;

void from_json(const json &j, Module &m)
{
    j.at("name").get_to(m.moduleName);
    j.at("command").get_to(m.moduleCommand);
    j.at("description").get_to(m.moduleDescription);
    j.at("baseFilePath").get_to(m.baseFilePath);
}

int ModuleManager::loadModules(fs::path moduleDirectory)
{
    int loadedCount = 0;

    if (!fs::exists(moduleDirectory))
    {
        return 0; // 如果目录不存在,返回0
    }

    // 遍历插件目录下的所有文件
    for (const auto &entry : fs::directory_iterator(moduleDirectory))
    {
        if (entry.is_regular_file())
        { // 只处理普通文件
            // 获取文件的完整路径
            fs::path pluginFilePath = entry.path();
            if (pluginFilePath.extension() == ".json")
            {
                try
                {
                    ifstream pluginFile(pluginFilePath);
                    if (!pluginFile.is_open())
                    {
                        cerr << "无法打开插件描述文件" << pluginFilePath << endl;
                        return 1;
                    }
                    json j;
                    pluginFile >> j;
                    Module curModule = j.get<Module>();
                    this->moduleMap[curModule.moduleCommand] = curModule;
                    cout << "加载插件" << curModule.moduleName << endl;
                }
                catch (const json::exception &e)
                {
                    cerr << "JSON解析错误:" << e.what() << endl;
                    return 2;
                }
            }
        }
    }
    return 0;
}

int ModuleManager::printModuleHelp(string moduleName)
{
    if (!moduleMap.count(moduleName))
    {
        cerr << "未找到该模块。你是不是搞错了？" << endl;
        return 3;
    }
    cout << moduleMap[moduleName].moduleDescription << endl;
    return 0;
}

int ModuleManager::executeModule(string command)
{
    string commandName = command.substr(0, command.find_first_of(" "));
    string commandArgs = command.substr(command.find_first_of(" ") + 1);
    if (!this->moduleMap.count(commandName))
    {
        cerr << "未找到对应的命令。你是不是搞错了？" << endl;
        return 3;
    }

    array<char, 1024> buf;
    string result;
    fs::path moduleFile = moduleMap[commandName].baseFilePath;
    string finalCommand = moduleFile.generic_string() + " " + commandArgs;
    FILE *pipe = popen(finalCommand.c_str(), "r");
    if (!pipe)
    {
        throw std::runtime_error("命令执行失败");
    }

    while (fgets(buf.data(), buf.size(), pipe) != nullptr)
        result += buf.data();
    cout << buf.data() << endl;
    cout.flush();
    return 0;
}

void ModuleManager::listModule()
{
    cout << "已加载的模块：" << endl;
    for (auto kv : this->moduleMap)
        cout << kv.first << endl;
    return;
}