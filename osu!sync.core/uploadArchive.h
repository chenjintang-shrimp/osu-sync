#include<iostream>
#include<filesystem>
using namespace std;
namespace fs = std::filesystem;

int uploadData(fs::path archiveFileName, string remoteFileName, string remoteURL);