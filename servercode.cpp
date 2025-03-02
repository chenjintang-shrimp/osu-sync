#include <iostream>
#include <httplib.h>
#include <filesystem>
#include <iomanip>

using namespace httplib;
using namespace std;

// 函数：生成安全的文件路径
void sanitize_filename(string& filename) {
    replace(filename.begin(), filename.end(), '\\', '/');
    replace(filename.begin(), filename.end(), '..', ""); // 防止路径穿越
}

int main() {
    Server svr;

    // 设置存储目录
    const string base_dir = "uploads";
    filesystem::create_directories(base_dir);

    // 处理 POST 请求
    svr.Post("/upload", [&](const Request& req, Response& res) {
        // 提取 multipart/form-data 数据
        MultipartFormData data = req.get_multipart_form_data();

        // 获取文件内容和文件名
        const MultipartFormDataItem* file_item = data["file"];

        if (!file_item) {
            res.set_content("Missing file part!", "text/plain");
            return;
        }

        string filename = file_item->filename;

        // 获取上传目录
        const MultipartFormDataItem* dir_item = data["upload_dir"];
        string upload_dir = dir_item ? dir_item->content : "";

        // 生成存储路径
        string final_path = base_dir + "/" + upload_dir + "/" + filename;

        // 清理路径
        sanitize_filename(final_path);

        // 创建目标目录
        filesystem::path dir_path = filesystem::path(final_path).parent_path();
        filesystem::create_directories(dir_path);

        // 保存文件内容
        ofstream outfile(final_path);
        if (outfile) {
            outfile << file_item->content;
            outfile.close();
            res.set_content("File uploaded successfully: " + final_path, "text/plain");
        }
        else {
            res.set_content("Failed to save file!", "text/plain");
        }
        });

    cout << "Server started at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}