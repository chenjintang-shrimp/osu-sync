#pragma once
#include <string>

struct beatmapSetAttribte
{
    std::string beatmapSetId;
    std::string title;
    std::string titleUnicode;
    std::string artist;
};

bool operator<(beatmapSetAttribte a, beatmapSetAttribte b)
{
    return a.beatmapSetId < b.beatmapSetId;
}