#pragma once

#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <cstdio>
#include "errcodes.hpp"
using namespace std;

pair<errorCode, string> executeCommand(string command)
{
    try
    {
        string result;
        array<char, 1024> buf;
        FILE *pipe = _popen(command.c_str(), "r");
        if (!pipe)
            throw runtime_error("命令执行失败");
        while (fgets(buf.data(), buf.size(), pipe) != nullptr)
            result += buf.data();
        _pclose(pipe); // 关闭管道
        return {ok, result};
    }
    catch (const runtime_error &e)
    {
        return {commandFail, e.what()};
    }
}