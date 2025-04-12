#pragma once
#include<zlib-ng.h>
#include<fstream>
#include<string>
#include<filesystem>
#include<vector>
namespace fs = std::filesystem;
using namespace std;

//参数要求：formattedOszNames中的每一个格式化后名称（谱面id + 空格 + 谱面罗马化名称）必须与pathToOSZs中的每一个谱面一一对应
int importOSZs(fs::path osuFolder, vector<fs::path> pathToOSZs, vector<string> formattedOszNames);
vector<string> getFormattedOszNamesFromList(fs::path beatmapListsFile);