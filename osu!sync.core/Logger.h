//
// Created by jimmy on 25-7-12.
//
#include <filesystem>
namespace fs=std::filesystem;


class Logger {
public:
    enum logLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };
    explicit Logger(const fs::path& logFilePath)
    {
        this->logFile=logFilePath;
    }

    void debug(const std::string& module, const std::string& msg);
    void info(const std::string& module, const std::string& msg);
    void warning(const std::string& module, const std::string& msg);
    void error(const std::string& module, const std::string& msg);

private:
    fs::path logFile;
    static std::string logLevelToString(logLevel level);
    void log(logLevel level, const std::string& module, const std::string& msg) const;
};
