#include"network.utils.h"

using namespace std;
using namespace httplib;
void uploadData(fs::path filename, std::string username, std::string remoteURL)
{
	httplib::Client cli(remoteURL, 80);
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs)
	{
		std::cerr << "error opening file";
		return;
	}
	string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
	httplib::MultipartFormDataItems items = {
		{"file_path",}
	}
}
