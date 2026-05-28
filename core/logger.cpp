#include "logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::set_file(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    file_.open(path, std::ios::app);
    if (!file_.is_open())
        std::cerr << "[Logger] Cannot open log file: " << path << "\n";
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string line = "[" + timestamp() + "] [" + level_str(level) + "] " + message + "\n";
    if (file_.is_open()) file_ << line << std::flush;
}

std::string Logger::level_str(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO   ";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR  ";
    }
    return "UNKNOWN";
}

std::string Logger::timestamp() {
    auto now  = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
