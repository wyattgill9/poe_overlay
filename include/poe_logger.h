#pragma once

#include <fstream>
#include <string>
#include <string_view>
#include <thread>

#include "SPSCQueue.h"

#include "base.hpp"
#include "poe_overlay.h"

static constexpr size_type QUEUE_CAPACITY = 100;

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

struct LogContext {
    rigtorp::SPSCQueue<LogEntry> queue;
    std::ofstream                file;
    std::atomic<bool>            stop_flag;
};

class POE2OverlayLogger {
public: 
    explicit POE2OverlayLogger(const std::string& filename);
    ~POE2OverlayLogger();
       
    void log(LogLevel level, const std::string& message);

private:   
    LogContext  ctx;
    std::thread log_thread;
};

namespace log_ops {
    constexpr std::string_view level_to_string(LogLevel level);

    void write_entry(std::ofstream& file, const LogEntry& entry);
    void log_thread_func(LogContext& ctx);
}
