#include"network.utils.h"

using namespace std;
using namespace httplib;
int uploadData(fs::path filename, std::string username, std::string remoteURL)
{
    httplib::Client cli(remoteURL);

    // 本地待上传文件路径（Windows格式）
    // 客户端提供的相对路径
    std::string relativePath = username+"/"+filename.string();

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string fileData = buffer.str();
    ifs.close();

    // 构造 multipart/form-data 数据
    httplib::MultipartFormDataItems items = {
        { "filepath", relativePath, "", "text/plain" },
        { "file", fileData, "file.txt", "application/octet-stream" }
    };

    auto res = cli.Post("/upload", items);
    if (res && res->status == 200) {
        std::cout << "响应: " << res->body << std::endl;
    }
    else {
        std::cerr << "上传失败，状态码: "
            << (res ? std::to_string(res->status) : "无响应") << std::endl;
    }
    return 0;
}
