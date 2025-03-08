#include<iostream>
#include<fstream>
#include<sstream>
#include<chrono>
#include<iomanip>
#include<filesystem>
#include"3rdparty/httplib.h"
namespace fs = std::filesystem;
using namespace std;

const fs::path baseUploadDir = "/home/uploads";

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
		errBuf = "�����Ծ���·�������ļ����ѽ�ֹ";
		return false;
	}
	for (auto& compoment : path)
	{
		if (compoment == "..")
		{
			errBuf = "��ͼ������һ��Ŀ¼���ѽ�ֹ";
			return false;
		}
	}
	return true;
}

int main(int argc,char* argv[])
{

	httplib::Server svr;
	svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res)
		{
			if (!req.has_file("file"))
			{
				res.status = 400;
				res.set_content("�ļ��أ��������ļ��أ�������", "text/plain");
				write_log("./logs/log.txt","����δ�ϴ��ɹ����ļ�");
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
				res.set_content("�Ƿ��ļ�·��", "text/plain");
				write_log("./logs/log.txt", errMsg);
			}
			fs::path tgtPath = baseUploadDir / savePath;
			try
			{
				fs::create_directories(tgtPath.parent_path());
			}
			catch (const fs::filesystem_error& e)
			{
				res.status = 500;
				string errorMsg = "Ŀ¼" + string(e.what()) + "����ʧ��";
				res.set_content(errorMsg, "text/plain");
				write_log("./logs/log.txt", errorMsg);
				return;
			}

			ofstream ofs(tgtPath, ios::binary);
			if (!ofs)
			{
				res.status = 500;
				std::string error_msg = "���ļ�д��ʧ��: " + tgtPath.string();
				res.set_content(error_msg, "text/plain");
				write_log("./logs/log.txt",error_msg);
				return;
			}
			ofs.write(file.content.data(), file.content.size());
			ofs.close();

			std::string success_msg = "�ļ��ϴ��ɹ�: " + tgtPath.string();
			res.set_content(success_msg, "text/plain");
			write_log("./logs/log.txt",success_msg);
		});
}