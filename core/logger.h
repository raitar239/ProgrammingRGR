#pragma once
#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel { INFO, WARNING, ERROR };

class Logger {
public:
    static Logger& instance();

    void set_file(const std::string& path);
    void log(LogLevel level, const std::string& message);

    void info   (const std::string& msg) { log(LogLevel::INFO,    msg); }
    void warning(const std::string& msg) { log(LogLevel::WARNING, msg); }
    void error  (const std::string& msg) { log(LogLevel::ERROR,   msg); }

private:
    Logger() = default;
    std::ofstream file_;
    std::mutex    mutex_;

    static std::string level_str(LogLevel level);
    static std::string timestamp();
};

#define LOG_INFO(msg)    Logger::instance().info(msg)
#define LOG_WARNING(msg) Logger::instance().warning(msg)
#define LOG_ERROR(msg)   Logger::instance().error(msg)
