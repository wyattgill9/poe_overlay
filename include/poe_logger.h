#pragma once

#include <fstream>
#include <string>

#include "poe_overlay.h"

enum class LogLevel : u8 {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    ERROR = 3, 
};

struct LogMessage {
    LogLevel    level;
    std::string data;

    LogMessage(std::string data, LogLevel level);
};

class POE2OverlayLogger {
public:
    POE2OverlayLogger(const std::string& filename);
    ~POE2OverlayLogger();

    void debug(const std::string& msg);
    void info (const std::string& msg);
    void warn (const std::string& msg);
    void error(const std::string& msg);

private:
    void log(LogLevel level, const std::string& msg);

    base::containers::BoundedPointerSPSC<LogMessage*> queue;
    std::ofstream file;
};
