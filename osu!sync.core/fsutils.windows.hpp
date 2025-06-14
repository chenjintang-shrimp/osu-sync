#pragma once
#include "stableExporter.hpp"
#include "beatmap_types.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

std::vector<std::string> getBeatmapFolderNames(const fs::path& songsFolderPath);
void writeBeatmapDataToFile(const std::vector<osu::BeatmapInfo>& data, const fs::path& filePath);