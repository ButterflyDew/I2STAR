#include "../include/Log.hpp"

LogLevel Log::consoleLevel = LogLevel::LOG_DEBUG;
LogLevel Log::fileLevel = LogLevel::LOG_INFO;
std::ofstream Log::fileStream;

void Log::setLogFile(const std::string& filename) {
    fileStream.open(filename, std::ios::out | std::ios::trunc);
}

void Log::setConsoleLevel(LogLevel level) {
    consoleLevel = level;
}

void Log::setFileLevel(LogLevel level) {
    fileLevel = level;
}

void Log::log(LogLevel level, const std::string& msg) {
#ifdef DEBUG
    if (level >= consoleLevel && level != LogLevel::LOG_IMPORTANT) {
        std::cout << msg << std::endl;
    }
    if (fileStream.is_open() && level >= fileLevel) {
        fileStream << msg << std::endl;
    }
#endif
}

void Log::debug(const std::string& msg) { log(LogLevel::LOG_DEBUG, "[DEBUG] " + msg); }
void Log::info(const std::string& msg)  { log(LogLevel::LOG_INFO,  "[INFO] "  + msg); }
void Log::important(const std::string& msg) { log(LogLevel::LOG_IMPORTANT, "[IMPORTANT] " + msg); }
void Log::warn(const std::string& msg)  { log(LogLevel::LOG_WARN,  "[WARN] "  + msg); }
void Log::error(const std::string& msg) { log(LogLevel::LOG_ERROR, "[ERROR] " + msg); } 