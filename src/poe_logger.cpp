#include "poe_logger.h"
#include <type_traits>

LogMessage::LogMessage(std::string data, LogLevel level)
    : level(level), data(data) {}

POE2OverlayLogger::POE2OverlayLogger(const std::string& filename)
    : file(("../log/" + filename), std::ios::app)
{
    queue.start_consumer([this](LogMessage* msg) {
        if (file.is_open()) {
            file << "[" << level_to_string(*msg) << "] " 
                 << msg->data << "\n";
        }
        delete msg;
    });
}

POE2OverlayLogger::~POE2OverlayLogger() {
    queue.stop();
    queue.join();
    if (file.is_open()) {
        file.close();
    }
}

void POE2OverlayLogger::log(LogLevel level, const std::string& msg) {
    queue.push(new LogMessage(msg, level));
}

void POE2OverlayLogger::debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
void POE2OverlayLogger::info(const std::string& msg)  { log(LogLevel::INFO, msg); }
void POE2OverlayLogger::warn(const std::string& msg)  { log(LogLevel::WARN, msg); }
void POE2OverlayLogger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }

std::string level_to_string(LogMessage msg) {
    switch (std::underlying_type_t<LogLevel>(msg.level)) {
        case 0: return "DEBUG";
        case 1: return "INFO";
        case 2: return "WARN";
        case 3: return "ERROR";
    }
    return "UNKNOWN";
}
