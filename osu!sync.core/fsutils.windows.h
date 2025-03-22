#pragma once
#include<iostream>
#include<filesystem>
#include<vector>
#include<string>
using namespace std::filesystem;

std::vector<std::string> getBeatmapFolderNames(std::filesystem::path& songsFolderPath);
void writeBeatmapDataToFile(std::vector<beatmapMetaData> data, fs::path filePath);