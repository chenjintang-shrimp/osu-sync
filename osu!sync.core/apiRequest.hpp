//
// Created by chenjintang on 25-7-9.
//
#pragma once
#include <3rdpartyInclude/nlohmann/json.hpp>
#include <string>
#include "beatmapSetAttribute.hpp"
#include "errcodes.hpp"

class apiRequest //using osu!api v1
{
private:
    std::string apiKey;
public:
    apiRequest(std::string apiKey) {
        this->apiKey = apiKey;
    }

    std::pair<errorCode, beatmapSetAttribte> getBeatmapSetInfo(const std::string &bsid);
};




