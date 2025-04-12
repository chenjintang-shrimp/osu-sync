#include"network.utils.h"
#include"3rdpartyInclude/nlohmann/json.hpp"
using namespace nlohmann;

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

vector<string> downloadBeatmapFromList(fs::path pathToBeatmapList)
{
    vector<string> bmIdlist;
    ifstream bmList(pathToBeatmapList);
    if (!bmList.is_open())
    {
        cerr << "打开文件失败！" << endl;
        exit(0);
    }
    json jList = json::parse(bmList);
    for (auto kv : jList)
    {
        cout << kv["onlineId"] << endl;
    }
}
