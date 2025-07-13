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

errorCode apiRequest::refreshToken()
{
    if (chrono::system_clock::now()<=expiredTime)
        return ok;
    httplib::Client cli("http://120.26.51.57",8080);
    httplib::Params parm={
        {"refresh_token",refresh_Token}
    };
    auto res = cli.Post("/refresh-token", parm);
    if (res->status == 200)
    {
        json j=json::parse(res->body);
        token=j["access_token"];
        expiredTime = chrono::system_clock::now() + chrono::seconds(j["expire_in"].get<int>());
        refresh_Token=j["refresh_token"];
        return ok;
    }
    else
        return networkError;
}

pair<errorCode, string> apiRequest::requestApi(std::string endpoint, httplib::Params params)
{
    refreshToken();
    httplib::Client cli("http://osu.ppy.sh");
    httplib::Headers header={
        {"Authorization","Bearer "+token}
    };
    auto res=cli.Get(endpoint,params,header);
    if (res&&res->status==200)
    {
        logger_.info("apiRequest","成功通过api"+endpoint+"获取数据");
        return {ok,res->body};
    }
    return {networkError,string()};
}

errorCode apiRequest::getToken()
{
    httplib::Client cli("http://120.26.51.57", 8080);
    auto res = cli.Get("/start-auth");
    if (res&&res->status == 200)
    {
        string oauthUrl = res->body;
        logger_.info("apiRequest", "请打开以下网址授权oauth，并将浏览器最后显示的字符全部复制下来并输入");
        cout << oauthUrl << endl;
        string jsonCode;
        getline(cin, jsonCode, (char)EOF);
        json j = json::parse(jsonCode);
        if (j.contains("access_token"))
        {
            token = j["access_token"];
            expiredTime = chrono::system_clock::now() + chrono::seconds(j["expire_in"].get<int>());
            refresh_Token = j["refresh_token"];
            return ok;
        }
    }
    else
        return networkError;
}

pair<errorCode,beatmapSetAttribte> apiRequest::getBeatmapSetDetails(const std::string &bsid)
{
    string endpoint="/api/v2/beatmapsets"+bsid;
    auto result =requestApi(endpoint);
    if (result.first==ok)
    {
        beatmapSetAttribte ret;
        json bsj=json::parse(result.second);
        ret.title=bsj["title"];
        ret.artist=bsj["artist"];
        ret.beatmapSetId=bsj["id"];
        ret.titleUnicode=bsj["title_unicode"];
        return make_pair(ok,ret);
    }
    else
        return make_pair(result.first,beatmapSetAttribte());
}