#pragma once
#include<zlib-ng.h>
#include<fstream>
#include<string>
#include<filesystem>
#include<vector>
namespace fs = std::filesystem;
using namespace std;

//����Ҫ��formattedOszNames�е�ÿһ����ʽ�������ƣ�����id + �ո� + �����������ƣ�������pathToOSZs�е�ÿһ������һһ��Ӧ
int importOSZs(fs::path osuFolder, vector<fs::path> pathToOSZs, vector<string> formattedOszNames);
vector<string> getFormattedOszNamesFromList(fs::path beatmapListsFile);