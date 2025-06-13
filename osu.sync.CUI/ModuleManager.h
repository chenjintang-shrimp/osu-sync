#pragma once
#include<string>
#include<map>
#include<vector>
#include<filesystem>

namespace fs=std::filesystem;

/*
ModuleManager.h: 模块加载器，管理和加载模块，支持一个模块注册多个命令
*/

struct Command {
    std::string name;
    std::string description;
};

struct Module
{
    std::string moduleName;
    std::vector<Command> commands;
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
