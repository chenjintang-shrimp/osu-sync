#include "fsutils.windows.h"
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
