//
// Created by chenjintang on 25-7-9.
//
#pragma once
#include "3rdpartyInclude/nlohmann/json.hpp"
#include <string>
#include "beatmapSetAttribute.hpp"
#include "errcodes.hpp"
#include <chrono>
#include "Logger.h"
#include "httplib.h"

class apiRequest //using osu!api v2(oauth)
{
private:
    std::string token;
    std::string refresh_Token;
    std::chrono::system_clock::time_point expiredTime;
    errorCode refreshToken();
    std::pair<errorCode, std::string> requestApi(std::string endpoint, httplib::Params params = {});
    Logger logger_;
public:
    apiRequest(std::string logPath)
    {
        logger_ = Logger(logPath);
    }
    apiRequest(){};
    errorCode getToken();
    std::pair<errorCode,beatmapSetAttribte> getBeatmapSetDetails(const std::string &bsid);

};




