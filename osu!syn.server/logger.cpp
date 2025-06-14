#include "logger.hpp"
#include <fstream>
#include <iomanip>
#include <chrono>
#include <iostream>

Logger::Logger(const fs::path& logDir) : logDir_(logDir) {
    currentLogFile_ = ensureLogDirectory() / "server.log";
}

void Logger::debug(const std::string& message) {
    log(Level::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(Level::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(Level::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(Level::ERROR, message);
}

void Logger::log(Level level, const std::string& message) {
    std::ofstream logFile(currentLogFile_, std::ios::app);
    if (!logFile) {
        std::cerr << "无法打开日志文件: " << currentLogFile_ << std::endl;
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    
    logFile << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S")
            << " [" << levelToString(level) << "] "
            << message << std::endl;
            
    // 同时输出到控制台
    std::cout << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S")
              << " [" << levelToString(level) << "] "
              << message << std::endl;
}

fs::path Logger::ensureLogDirectory() {
    try {
        if (!fs::exists(logDir_)) {
            fs::create_directories(logDir_);
        }
        return logDir_;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "创建日志目录失败: " << e.what() << std::endl;
        return fs::path(".");
    }
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARN";
        case Level::ERROR:   return "ERROR";
        default:            return "UNKNOWN";
    }
}
