#pragma once
#include <iostream>
#include <fstream>
#include <string>

// 日志优先级
enum class LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_IMPORTANT = 2,
    LOG_WARN = 3,
    LOG_ERROR = 4
};      

class Log {
public:
    static void setLogFile(const std::string& filename);
    static void setConsoleLevel(LogLevel level);
    static void setFileLevel(LogLevel level);

    static void log(LogLevel level, const std::string& msg);

    static void debug(const std::string& msg);
    static void info(const std::string& msg);
    static void important(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);

private:
    static LogLevel consoleLevel;
    static LogLevel fileLevel;
    static std::ofstream fileStream;
}; 