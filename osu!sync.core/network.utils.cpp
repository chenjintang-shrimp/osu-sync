#include"network.utils.h"

using namespace std;
using namespace httplib;
int uploadData(fs::path filename, std::string username, std::string remoteURL)
{
    httplib::Client cli(remoteURL);

    // ���ش��ϴ��ļ�·����Windows��ʽ��
    // �ͻ����ṩ�����·��
    std::string relativePath = username+"/"+filename.string();

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "�޷����ļ�: " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string fileData = buffer.str();
    ifs.close();

    // ���� multipart/form-data ����
    httplib::MultipartFormDataItems items = {
        { "filepath", relativePath, "", "text/plain" },
        { "file", fileData, "file.txt", "application/octet-stream" }
    };

    auto res = cli.Post("/upload", items);
    if (res && res->status == 200) {
        std::cout << "��Ӧ: " << res->body << std::endl;
    }
    else {
        std::cerr << "�ϴ�ʧ�ܣ�״̬��: "
            << (res ? std::to_string(res->status) : "����Ӧ") << std::endl;
    }
    return 0;
}
