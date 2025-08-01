#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

enum class Level
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    explicit Logger(const fs::path& logDir);

    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    
    // 设置日志级别
    void setLogLevel(Level level) { minLevel_ = level; }
    
    // 检查是否应该记录指定级别的日志
    bool shouldLog(Level level) const { return level >= minLevel_; }

private:
    void log(Level level, const std::string& message);
    fs::path ensureLogDirectory();
    std::string levelToString(Level level);

    fs::path logDir_;
    fs::path currentLogFile_;
    Level minLevel_ = Level::DEBUG;  // 默认记录所有级别的日志
};