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

    // ���ش��ϴ��ļ�·����Windows��ʽ��
    // �ͻ����ṩ�����·��
    string relativePath = username + "/" + filename.generic_string(); // ʹ��ͨ�ø�ʽ·��

    ifstream ifs(filename, ios::binary);
    if (!ifs) {
        cerr << "�޷����ļ�: " << filename << endl;
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
        cout << "��Ӧ: " << res->body << endl;
    }
    else {
        cerr << "�ϴ�ʧ�ܣ�״̬��: "
            << (res ? to_string(res->status) : "����Ӧ") << endl;
    }
    return 0;
}