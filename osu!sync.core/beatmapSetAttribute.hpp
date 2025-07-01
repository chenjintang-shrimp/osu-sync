#pragma once
#include <string>
#include "3rdparty/nlohmann/json.hpp"

struct beatmapSetAttribte
{
    std::string beatmapSetId;
    std::string title;
    std::string titleUnicode;
    std::string artist;
};

void to_json(nlohmann::json& j, const beatmapSetAttribte& bsa) {
    j = nlohmann::json{
        {"beatmapSetId", bsa.beatmapSetId},
        {"title", bsa.title},
        {"titleUnicode", bsa.titleUnicode},
        {"artist", bsa.artist}
    };
}

bool operator<(beatmapSetAttribte a, beatmapSetAttribte b)
{
    return a.beatmapSetId < b.beatmapSetId;
}