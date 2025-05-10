#include"network.utils.h"
#include"curl/curl.h"
#include<string>
#include"nlohmann/json.hpp"
using namespace nlohmann;

using namespace std;
using namespace httplib;

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
