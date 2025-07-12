//
// Created by chenjintang on 25-7-9.
//
#pragma once
#include "3rdpartyInclude/nlohmann/json.hpp"
#include <string>
#include "beatmapSetAttribute.hpp"
#include "errcodes.hpp"
#include <chrono>

class apiRequest //using osu!api v2(oauth)
{
private:
    std::string token;
    std::chrono::system_clock::time_point expiredTime;
public:
    errorCode getToken();
    std::pair<errorCode,beatmapSetAttribte> getBeatmapSetDetails(const std::string &bsid) const;
};




