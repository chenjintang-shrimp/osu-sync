#include"network.utils.h"
#include"curl/curl.h"
#include<string>
#include"3rdpartyInclude/nlohmann/json.hpp"
using namespace nlohmann;

#define CURL_STATICLIB

using namespace std;
using namespace httplib;
int uploadData(fs::path filename, string username, string remoteURL)
{
    httplib::Client cli(remoteURL);

    // 本地待上传文件路径（Windows格式）
    // 客户端提供的相对路径
    string relativePath = username + "/" + filename.generic_string(); // 使用通用格式路径

    ifstream ifs(filename, ios::binary);
    if (!ifs) {
        cerr << "无法打开文件: " << filename << endl;
        return 1;
    }
    stringstream buffer;
    buffer << ifs.rdbuf();
    string fileData = buffer.str();
    ifs.close();

    // 构造 multipart/form-data 数据
    cout << relativePath << endl;
    httplib::MultipartFormDataItems items = {
        { "filepath", relativePath, "", "text/plain; charset=utf-8" },
        { "file", fileData, filename.generic_string(), "application/octet-stream"}
    };

    auto res = cli.Post("/upload", items);
    if (res && res->status == 200) {
        cout << "响应: " << res->body << endl;
    }
    else {
        cerr << "上传失败，状态码: "
            << (res ? to_string(res->status) : "无响应") << endl;
    }
    return 0;
}

void from_json(const json& j, beatmapMetaData bmd)
{
    j.at("onlineId").get_to(bmd.first);
    j.at("fullName").get_to(bmd.second);
}

string getSayobotMirrorURL(string beatmapId)
{
    string basePath = "beatmaps";
    string ret = basePath;
    if (beatmapId.size() <= 4)
        ret = ret + "/0" + "/" + beatmapId;
    else
        ret = ret + "/" + beatmapId.substr(0, beatmapId.size()-4) + "/" + beatmapId.substr(beatmapId.size() - 4);
    ret = ret + "/full%3ffilename=" + beatmapId;
    return ret;
}

static size_t writeOszToFile(void* data, size_t size, size_t nmenb, void* userp)
{
    ofstream* out_file = static_cast<ofstream*>(userp);
    if (out_file && out_file->is_open())
    {
        out_file->write(static_cast<char*>(data), size * nmenb);
        return size * nmenb;
    }
    return 0;
}

vector<string> downloadBeatmapFromList(fs::path pathToBeatmapList,fs::path downloadDir)
{
    vector<string> bmIdlist;
    ifstream bmList(pathToBeatmapList);
    if (!bmList.is_open())
    {
        cerr << "打开文件失败！" << endl;
        exit(-1);
    }
    json jList = json::parse(bmList);
    for (auto kv : jList)
    {
        CURL* curl = curl_easy_init();
        if (!curl)
        {
            cerr << "curl初始化失败" << endl;
            exit(-1);
        }
        //cout << kv << endl;
        string fullName = kv["fullName"];
        string onlineId = kv["onlineId"];
        if (onlineId == "-1") continue;
        ofstream out_file(downloadDir / (onlineId + ".osz"), ios::binary);
        if (!out_file.is_open())
        {
            cerr << "无法创建文件" << onlineId << ".osz" << endl;
            curl_easy_cleanup(curl);
            exit(-1);
        }
        //cout << "https://b10.sayobot.cn:25225" + getSayobotMirrorURL(onlineId) << endl;
        curl_easy_setopt(curl, CURLOPT_PORT, 25225);
        //curl_easy_setopt(curl, CURLOPT_URL, "https://b10.sayobot.cn:25225" + getSayobotMirrorURL(onlineId));
        //curl_easy_escape(curl, ("https://b10.sayobot.cn:25225" + getSayobotMirrorURL(onlineId)).c_str();
        string URLpath = "https://b10.sayobot.cn/"+getSayobotMirrorURL(onlineId);
        curl_easy_setopt(curl, CURLOPT_URL, URLpath);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeOszToFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  // 启用详细输出v
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            cerr << "下载失败：" << curl_easy_strerror(res) << endl;
        else
            cout << "谱面" << fullName << "下载成功" << endl;
        out_file.close();
        curl_easy_cleanup(curl);
    }
    return bmIdlist;
}
