#include<iostream>
#include<filesystem>
#include"httplib.h"
#include"nlohmann/json.hpp"
#include"globalUsed.h"

namespace fs = std::filesystem;
using namespace nlohmann;

void from_json(const json& j, beatmapMetadata bmd)
{
    j.at("onlineId").get_to(bmd.onlineID);
    j.at("fullName").get_to(bmd.beatmapName);
}

int uploadData(fs::path filename, string username, string remoteURL)
{
    httplib::Client cli(remoteURL);

    // 构建上传文件路径（使用通用格式）
    // 客户端提供相对路径
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

    // ���� multipart/form-data ����
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