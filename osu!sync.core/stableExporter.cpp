#include<fstream>
#include<algorithm>
#include"stableExporter.h"
#include"fsutils.windows.h"
#include"nlohmann/json.hpp"
using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

bool isDigit(string tgt)
{
	return all_of(tgt.begin(), tgt.end(), ::isdigit);
}

vector<beatmapMetaData> getAllBeatmapData(vector<string> beatmapNames)
{
	vector<string> noData = { "no data" };
	if (beatmapNames == noData)
	{
		vector<beatmapMetaData> noData;
		beatmapMetaData temp;
		temp.first = "no data";
		temp.second = "no data";
		noData.push_back(temp);
		return noData;
	}
	vector<beatmapMetaData> data;
	for (auto kv : beatmapNames)
	{
		beatmapMetaData temp;
		int blankOffset = kv.find_first_of(" ");
		temp.first = kv.substr(0, blankOffset);
		if (!isDigit(temp.first))
			temp.first = "-1";//±ê¼ÇË½Æ×
		temp.second = kv.substr(blankOffset + 1, kv.size());
		data.push_back(temp);
	}
	vector<beatmapMetaData> tmpData = data;
	data.clear();
	data.push_back(tmpData[0]);
	for (auto kv : tmpData)
	{
		if (data.size() > 0)
		{
			if (kv.first != (data.end()-1)->first)
				data.push_back(kv);
		}
	}
	return data;
}





