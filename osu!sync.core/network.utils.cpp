#include"network.utils.h"
#include"3rdpartyInclude/nlohmann/json.hpp"
using namespace nlohmann;

using namespace std;
using namespace httplib;
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
        cerr << "���ļ�ʧ�ܣ�" << endl;
        exit(0);
    }
    json jList = json::parse(bmList);
    for (auto kv : jList)
    {
        cout << kv["onlineId"] << endl;
    }
}
