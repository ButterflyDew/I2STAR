#include "../include/Log.hpp"
#include <iostream>
using namespace std;

int main() {
    Log::setLogFile("log.txt");
    Log::setConsoleLevel(LogLevel::LOG_DEBUG);   // 控制台输出所有日志
    Log::setFileLevel(LogLevel::LOG_INFO);     // 文件输出INFO及以上

    Log::debug("This is a debug message (should only be in file)");
    Log::info("This is an info message (console and file)");
    Log::warn("This is a warning (console and file)");
    Log::error("This is an error (console and file)");

    cout << "Check console and log.txt for output." << endl;
    return 0;
}