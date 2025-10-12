
#include "poe_logger.h"

std::string LogMessage::level_to_string() const {
    switch (level) {
        case 0: return "DEBUG";
        case 1: return "INFO";
        case 2: return "WARN";
        case 3: return "ERROR";
    }
    return "UNKNOWN";
}

POE2OverlayLogger::POE2OverlayLogger(const std::string& filename) : file(filename, std::ios::app) {
    queue.start_consumer([this](LogMessage* msg) {
        if (file.is_open()) {
            file << "[" << msg->level_to_string() << "] " 
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

void POE2OverlayLogger::log(u8 level, const std::string& msg) {
    queue.push(new LogMessage(msg, level));
}

void POE2OverlayLogger::debug(const std::string& msg) { log(0, msg); }
void POE2OverlayLogger::info(const std::string& msg)  { log(1, msg); }
void POE2OverlayLogger::warn(const std::string& msg)  { log(2, msg); }
void POE2OverlayLogger::error(const std::string& msg) { log(3, msg); }
