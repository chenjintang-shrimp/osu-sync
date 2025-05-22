#include<iostream>
#include<fstream>
#include<sstream>
#include<chrono>
#include<iomanip>
#include<filesystem>
#include<locale>
#include"3rdparty/httplib.h"
namespace fs = std::filesystem;
using namespace std;

const fs::path baseUploadDir = "uploads";

void write_log(fs::path logFile, string msg)
{
	ofstream logFileHandler(logFile, ios::app);
	if (logFileHandler)
	{
		auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
		logFileHandler << put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " "
			<< msg << endl;
	}
}

bool isSafePath(fs::path path, string& errBuf)
{
	if (path.is_absolute())
	{
		errBuf = "尝试以绝对路径访问文件，已禁止";
		return false;
	}
	for (auto& compoment : path)
	{
		if (compoment == "..")
		{
			errBuf = "试图访问上一级目录，已禁止";
			return false;
		}
	}
	return true;
}

int main(int argc,char* argv[])
{

	httplib::Server svr;
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale("C.UTF-8"));
	svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res)
		{
			if (!req.has_file("file"))
			{
				res.status = 400;
				res.set_content("文件呢？我问你文件呢？？？？", "text/plain");
				write_log("./logs/log.txt","出现未上传成功的文件");
				return;
			}
			auto file = req.get_file_value("file");
			fs::path savePath;
			if (req.has_param("filepath"))
				savePath = req.get_param_value("filepath");
			else
				savePath = file.filename;
			string errMsg;
			if (!isSafePath(savePath, errMsg))
			{
				res.status = 400;
				res.set_content("非法文件路径", "text/plain");
				cerr << errMsg << endl;
			}
			fs::path tgtPath = baseUploadDir / savePath;
			try
			{
				fs::create_directories(tgtPath.parent_path());
			}
			catch (const fs::filesystem_error& e)
			{
				res.status = 500;
				string errorMsg = "目录" + string(e.what()) + "创建失败";
				std::cerr << errorMsg << endl;
				return;
			}

			ofstream ofs(tgtPath, ios::binary);
			if (!ofs)
			{
				res.status = 500;
				std::string error_msg = "打开文件写入失败: " + tgtPath.string();
				res.set_content(error_msg, "text/plain");
				cerr << error_msg << endl;
				return;
			}
			ofs.write(file.content.data(), file.content.size());
			ofs.close();

			std::string success_msg = "文件上传成功: " + tgtPath.string();
			res.set_content("文件上传成功: " + tgtPath.string(), "text/plain; charset=utf-8");
			auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

			cout << put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " " << success_msg << endl;
		});
	cout << "Server started." << endl;
	svr.listen("0.0.0.0", 8080);
}