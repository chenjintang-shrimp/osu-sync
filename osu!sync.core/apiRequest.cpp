//
// Created by chenjintang on 25-7-9.
//

#include "apiRequest.hpp"
#include <map>
#include "errcodes.hpp"
#include <3rdpartyInclude/nlohmann/json.hpp>
#include <3rdpartyInclude/httplib.h>
#include <chrono>
#include "Logger.h"
using namespace nlohmann;
using namespace std;

errorCode apiRequest::getToken()
{
    httplib::Client cli("http://120.26.51.57", 8080);
    auto res = cli.Get("/start-auth");
    if (res->status == 200)
    {
        string oauthUrl = res->body;
        Logger log("");
        log.info("apiRequest", "请打开以下网址授权oauth，并将浏览器最后显示的字符全部复制下来并输入");
        cout << oauthUrl << endl;
        string jsonCode;
        getline(cin, jsonCode, (char)EOF);
        json j = json::parse(jsonCode);
        if (j.contains("access_token"))
        {
            token = j["access_token"];
            expiredTime=chrono::system_clock::now()+chrono::seconds(j["expire_in"].get<int>());
            return ok;
        }
    }
}
