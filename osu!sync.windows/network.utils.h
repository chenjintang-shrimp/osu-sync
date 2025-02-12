#pragma once
#include"vcpkg_installed/x64-windows/include/curl/curl.h"
#include<filesystem>
namespace fs = std::filesystem;

void uploadFile(fs::path jsonPath,std::string userName,std::string url);
