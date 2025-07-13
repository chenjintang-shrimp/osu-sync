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
    if (chrono::system_clock::now() <= expiredTime)
        return ok;

    logger_.info("apiRequest", "正在尝试连接到认证服务器x");
    httplib::Client cli("120.26.51.57", 8080);
    cli.set_connection_timeout(10);
    cli.set_read_timeout(10);
    cli.set_write_timeout(10);

    httplib::Params parm = {
        {"refresh_token", refresh_Token}
    };

    auto res = cli.Post("/refresh-token", parm);

    if (!res)
    {
        logger_.error("apiRequest", "[ERROR] 刷新令牌失败，连接错误: " + httplib::to_string(res.error()));
        logger_.error("apiRequest", "- 错误代码: " + httplib::to_string(res.error()));

        return networkError;
    }

    if (res->status == 200)
    {
        try
        {
            json j = json::parse(res->body);
            token = j["access_token"];
            expiredTime = chrono::system_clock::now() + chrono::seconds(j["expire_in"].get<int>());
            refresh_Token = j["refresh_token"];
            return ok;
        }
        catch (const json::parse_error&)
        {
            logger_.error("apiRequest", "JSON解析失败，响应内容: " + res->body);
            return networkError;
        }
    }
    else
    {
        logger_.error("apiRequest", "HTTP请求失败，状态码: " + to_string(res->status));
        return networkError;
    }
}

pair<errorCode, string> apiRequest::requestApi(std::string endpoint, httplib::Params params)
{
    refreshToken();

    logger_.info("apiRequest", "正在连接到 osu.ppy.sh");
    httplib::Client cli("osu.ppy.sh");
    cli.set_connection_timeout(10);
    cli.set_read_timeout(10);
    cli.set_write_timeout(10);
    cli.set_follow_location(true);

    httplib::Headers header = {
        {"Authorization", "Bearer " + token},
        {"User-Agent", "osu!sync.core/1.0"}
    };

    auto res = cli.Get(endpoint, params, header);

    if (!res)
    {
        logger_.error("apiRequest", "API请求失败，连接错误: " + httplib::to_string(res.error()));
        logger_.error("apiRequest", "- 错误代码: " + httplib::to_string(res.error()));
        return {networkError, string()};
    }

    if (res->status == 200)
    {
        logger_.info("apiRequest", "成功通过API获取数据: " + endpoint);
        return {ok, res->body};
    }

    logger_.error("apiRequest", " 非预期的HTTP响应，状态码: " + to_string(res->status));
    return {networkError, string()};
}

errorCode apiRequest::getToken()
{
    logger_.info("apiRequest", "开始获取新令牌，连接到认证服务器");
    httplib::Client cli("120.26.51.57", 8080);
    cli.set_connection_timeout(10);
    cli.set_read_timeout(10);
    cli.set_write_timeout(10);

    auto res = cli.Get("/start-auth");

    if (!res)
    {
        logger_.error("apiRequest", " 获取令牌失败，连接错误: " + httplib::to_string(res.error()));
        logger_.error("apiRequest", "- 错误代码: " + httplib::to_string(res.error()));

        return networkError;
    }

    if (res->status == 200)
    {
        string oauthUrl = res->body;
        logger_.info("apiRequest", "请打开以下网址授权oauth，并将浏览器最后显示的字符全部复制下来并输入");
        cout << oauthUrl << endl;
        string jsonCode;
        getline(cin, jsonCode, (char)EOF);
        try
        {
            json j = json::parse(jsonCode);
            if (j.contains("access_token"))
            {
                token = j["access_token"];
                expiredTime = chrono::system_clock::now() + chrono::seconds(j["expire_in"].get<int>());
                refresh_Token = j["refresh_token"];
                return ok;
            }
        }
        catch (const json::parse_error&)
        {
            logger_.info("apiRequest", "JSON解析失败，请检查输入格式");
            return networkError;
        }
    }
    else
    {
        logger_.error("apiRequest", " HTTP请求失败，状态码: " + to_string(res->status));
        return networkError;
    }
}

pair<errorCode, beatmapSetAttribte> apiRequest::getBeatmapSetDetails(const std::string& bsid)
{
    string endpoint = "/api/v2/beatmapsets" + bsid;
    auto result = requestApi(endpoint);
    if (result.first == ok)
    {
        beatmapSetAttribte ret;
        json bsj = json::parse(result.second);
        ret.title = bsj["title"];
        ret.artist = bsj["artist"];
        ret.beatmapSetId = bsj["id"];
        ret.titleUnicode = bsj["title_unicode"];
        return make_pair(ok, ret);
    }
    else
        return make_pair(result.first, beatmapSetAttribte());
}
