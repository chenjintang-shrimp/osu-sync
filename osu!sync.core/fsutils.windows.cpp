#include "fsutils.windows.h"
#include<fstream>
#include"stableExporter.h"
#include"nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

vector<string> getBeatmapFolderNames(fs::path& songsFolderPath)
{
	if (!fs::exists(songsFolderPath)||!fs::is_directory(songsFolderPath))
		return {"no data"};
	vector<string> beatmapFolderNames;
	for (auto& kv : fs::directory_iterator(songsFolderPath))
	{
		if (kv.is_directory())
			beatmapFolderNames.push_back(kv.path().filename().string());
	}
	return beatmapFolderNames;
}

void to_json(json& j, beatmapMetaData bmd)
{
	j = json{ {"onlineId",bmd.first},{"fullName",bmd.second} };
}

void writeBeatmapDataToFile(vector<beatmapMetaData> data, fs::path filePath)
{
	json tgt;
	for (auto kv : data)
	{
		json temp;
		to_json(temp, kv);
		tgt.push_back(temp);
	}
	std::ofstream file(filePath, std::ios::out);
	file << tgt.dump(4);
	file.close();
	return;
}