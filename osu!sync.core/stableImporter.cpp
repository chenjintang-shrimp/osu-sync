#include"stableImporter.h"
#include<zlib-ng.h>

int importOSZs(fs::path osuFolder, vector<fs::path> pathToOSZs, vector<string> formattedOszNames)
{
	osuFolder /= "songs";
	for (int i = 0;i < pathToOSZs.size();i++)
	{
		ifstream in(pathToOSZs[i], ios::binary);
		string compressedOszData((istreambuf_iterator<char>(in)), {});
		zng_stream stream{};
		zng_inflateInit(&stream);
		vector<Bytef> decompressed(compressedOszData.size() * 10);
		stream.next_in = reinterpret_cast<Bytef*>(compressedOszData.data());
		stream.avail_in = compressedOszData.size();
		stream.next_out = decompressed.data();
		stream.avail_out = decompressed.size();

		int ret = zng_inflate(&stream, Z_FINISH);
		if (ret != Z_STREAM_END)
		{
			return -1;
		}
		ofstream out(osuFolder / formattedOszNames[i], ios::binary);
		out.write(reinterpret_cast<char*>(decompressed.data()), stream.total_out);
		zng_inflateEnd(&stream);
		return 0;
	}
}