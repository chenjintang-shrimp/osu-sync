#pragma once
#include <string>
#include "nlohmann/json.hpp"

struct beatmapSetAttribte
{
    std::string beatmapSetId;
    std::string title;
    std::string titleUnicode;
    std::string artist;
};

inline void to_json(nlohmann::json& j, const beatmapSetAttribte& bsa) {
    j = nlohmann::json{
        {"beatmapSetId", bsa.beatmapSetId},
        {"title", bsa.title},
        {"titleUnicode", bsa.titleUnicode},
        {"artist", bsa.artist}
    };
}

inline bool operator<(beatmapSetAttribte a, beatmapSetAttribte b)
{
    return a.beatmapSetId < b.beatmapSetId;
}