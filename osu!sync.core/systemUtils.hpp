#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
using namespace std;

string executeCommand(string command)
{
    array<char, 1024> buf;
    string result;
    FILE *pipe = _popen(command.c_str(), "r");
    if (!pipe)
    {
        throw runtime_error("命令执行失败");
    }

    while (fgets(buf.data(), buf.size(), pipe) != nullptr)
        result += buf.data();
    return result;
}