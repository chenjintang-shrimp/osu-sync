#pragma once
#include<string>
#include<map>
#include<string>
#include<vector>
#include<filesystem>

namespace fs=std::filesystem;

/*
ModuleManager.h: 模块加载器，管理和加载模块，同时负责模块的执行
*/

struct Module
{
    std::string moduleCommand;
    std::string moduleName;
    std::string moduleDescription;
    fs::path baseFilePath;
    bool enabled=true;
};

class ModuleManager
{
private:
    std::map<std::string,Module> moduleMap;
    std::map<std::string,std::string> commandName2ModuleNameMap;
public:
    int loadModules(fs::path moduleDirectory);
    int printCommandHelp(std::string moduleName);
    int executeModule(std::string command);
    void listModule();
    //TODO: 未来加入启用/禁用特定模块的功能
};