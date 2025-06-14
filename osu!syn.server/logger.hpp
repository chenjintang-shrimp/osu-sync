#pragma once
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    explicit Logger(const fs::path& logDir);

    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    void log(Level level, const std::string& message);
    fs::path ensureLogDirectory();
    std::string levelToString(Level level);
    
    fs::path logDir_;
    fs::path currentLogFile_;
};
