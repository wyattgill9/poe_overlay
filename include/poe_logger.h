#pragma once

#include <fstream>
#include <string>
#include <string_view>
#include <thread>

#include "SPSCQueue.h"

#include "base.hpp"

static constexpr size_t QUEUE_CAPACITY = 100;

enum class LogLevel : u8 {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    ERROR = 3, 
};

struct LogEntry {
    LogLevel    level;
    std::string msg;
};

namespace log_ops {
    constexpr std::string_view level_to_string(LogLevel level);

    void write_entry(std::ofstream& file, const LogEntry& entry);
    void log(rigtorp::SPSCQueue<LogEntry>& queue, std::ofstream& file, LogLevel level, std::string_view msg);
}

// Façade
class POE2OverlayLogger {
public:
    explicit POE2OverlayLogger(const std::string& filename);
    ~POE2OverlayLogger();
    
    POE2OverlayLogger(const POE2OverlayLogger&) = delete;
    POE2OverlayLogger& operator=(const POE2OverlayLogger&) = delete;
    
    void debug(std::string_view msg);
    void info (std::string_view msg);
    void warn (std::string_view msg);
    void error(std::string_view msg);

private:
    void log_thread_func();
    
    rigtorp::SPSCQueue<LogEntry> queue;

    std::ofstream file;

    std::thread log_thread;
    std::atomic<bool> stop_flag;
};
