#include<iostream>
#include<fstream>
#include<filesystem>
#include"3rdpartyInclude/httplib.h"
#define NOSSL

namespace fs = std::filesystem;

void uploadData(fs::path filename, std::string username, std::string remoteURL);
