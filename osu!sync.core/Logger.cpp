//
// Created by jimmy on 25-7-12.
//

#include "Logger.h"
#include <iostream>
#include <fstream>
using namespace std;

string Logger::logLevelToString(logLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "\033[47m\033[97m[DEBUG]\033[0m";
    case logLevel::INFO:
        return "\033[42m[INFO]\033[0m";
    case logLevel::WARNING:
        return "\033[103m[WARNING]\033[0m";
    case logLevel::ERROR:
        return "\033[101m[ERROR]\033[0m";
    }
}


void Logger::log(const logLevel level, const std::string& module, const std::string& msg) const
{
    const string lvlString = Logger::logLevelToString(level);
    cout << lvlString << " " << module << " : " << msg << endl;
    if (!(this->logFile.empty()))
    {
        ofstream logFile;
        if (exists(this->logFile))
            logFile.open(this->logFile, ios::out);
        else
            logFile.open(this->logFile, ios::app);
        logFile << lvlString << " " << module << " : " << msg << endl;
    }
}

void Logger::debug(const std::string& module, const std::string& msg)
{
    this->log(DEBUG, module, msg);
}

void Logger::info(const std::string& module, const std::string& msg)
{
    this->log(INFO, module, msg);
}

void Logger::warning(const std::string& module, const std::string& msg)
{
    this->log(WARNING, module, msg);
}

void Logger::error(const std::string& module, const std::string& msg)
{
    this->log(ERROR, module, msg);
}
