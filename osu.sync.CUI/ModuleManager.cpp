#include "ModuleManager.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include "nlohmann/json.hpp"
using namespace std;
using namespace nlohmann;

void from_json(const json &j, Command &cmd) {
    j.at("name").get_to(cmd.name);
    j.at("description").get_to(cmd.description);
}

void from_json(const json &j, Module &m) {
    j.at("name").get_to(m.moduleName);
    j.at("commands").get_to(m.commands);
    j.at("baseFilePath").get_to(m.baseFilePath);
    j.at("enabled").get_to(m.enabled);
}

ModuleError ModuleManager::loadModules(fs::path moduleDirectory) {
    if (!fs::exists(moduleDirectory)) {
        return ModuleError::Success;
    }

    for (const auto &entry : fs::directory_iterator(moduleDirectory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            try {
                ifstream pluginFile(entry.path());
                if (!pluginFile.is_open()) {
                    cerr << "无法打开插件描述文件" << entry.path() << endl;
                    return ModuleError::FileOpenFailed;
                }

                json j;
                pluginFile >> j;
                Module curModule = j.get<Module>();
                this->moduleMap[curModule.moduleName] = curModule;

                for (const auto &cmd : curModule.commands) {
                    this->commandName2ModuleNameMap[cmd.name] = curModule.moduleName;
                }

                cout << "加载插件" << curModule.moduleName << endl;
            } catch (const json::exception &e) {
                cerr << "JSON解析错误:" << e.what() << endl;
                return ModuleError::JsonParseFailed;
            }
        }
    }
    return ModuleError::Success;
}

ModuleError ModuleManager::printCommandHelp(string commandName) {
    if (!commandName2ModuleNameMap.count(commandName)) {
        cerr << "未找到该命令。你是不是搞错了？" << endl;
        return ModuleError::CommandNotFound;
    }

    string moduleName = commandName2ModuleNameMap[commandName];
    for (const auto &cmd : moduleMap[moduleName].commands) {
        if (cmd.name == commandName) {
            cout << cmd.description << endl;
            return ModuleError::Success;
        }
    }
    return ModuleError::CommandHelpNotFound;
}

ModuleError ModuleManager::executeModule(string command) {
    string commandName = command.substr(0, command.find_first_of(" "));
    string commandArgs = command.substr(command.find_first_of(" ") + 1);

    if (!commandName2ModuleNameMap.count(commandName)) {
        cerr << "未找到对应的命令。你是不是搞错了？" << endl;
        return ModuleError::CommandNotFound;
    }

    fs::path moduleFile = moduleMap[commandName2ModuleNameMap[commandName]].baseFilePath;
    string finalCommand = moduleFile.generic_string() + " " + commandArgs;

    try {
        array<char, 1024> buf;
        string result;
        FILE *pipe = _popen(finalCommand.c_str(), "r");
        if (!pipe) {
            throw runtime_error("命令执行失败");
        }

        while (fgets(buf.data(), buf.size(), pipe) != nullptr)
            result += buf.data();
        cout << result << endl;
        return ModuleError::Success;
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return ModuleError::CommandExecutionFailed;
    }
}

void ModuleManager::listModule() {
    cout << "已加载的模块和命令：" << endl;
    for (const auto &mod : moduleMap) {
        cout << mod.first << ":" << endl;
        for (const auto &cmd : mod.second.commands) {
            cout << "  " << cmd.name << " - " << cmd.description << endl;
        }
    }
}
