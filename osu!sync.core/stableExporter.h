#pragma once
#include<string>
#include<vector>
#include<map>
#include<filesystem>
namespace fs = std::filesystem;

typedef std::pair<std::string, std::string> beatmapMetaData;

std::vector<beatmapMetaData> getAllBeatmapData(std::vector<std::string> beatmapNames);
void writeBeatmapDataToFile(std::vector<beatmapMetaData> data, fs::path filePath);