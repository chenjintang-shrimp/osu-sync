//
// Created by chenjintang on 25-7-9.
//

#include "apiRequest.hpp"
#include <map>
#include "errcodes.hpp"
#include <3rdpartyInclude/nlohmann/json.hpp>
#include <3rdpartyInclude/httplib.h>
using namespace nlohmann;
using namespace std;
using namespace httplib;

pair<errorCode, beatmapSetAttribte> apiRequest::getBeatmapSetInfo(const std::string &bsid)
{
    Client cli("http://osu.ppy.sh");
    auto res=cli.Get("/api/get_beatmaps?k="+this->apiKey+"&s="+bsid);
    if (res->status!=httplib::OK_200)
        return {networkError,beatmapSetAttribte()};
    else
    {
        beatmapSetAttribte beatmap;
        string response=res->body;
        json data=json::parse(response);
        for (auto kv:data) {
            beatmap.artist=kv["artist"];
            beatmap.beatmapSetId=kv["beatmapSetId"];

        }
    }
}

