#include <filesystem>
#include <thread>
#include <utility>

#include "poe_logger.h"

namespace log_ops {

constexpr std::string_view level_to_string(LogLevel level) {
    switch(std::to_underlying(level)) {
        case 0:  return "DEBUG";
        case 1:  return "INFO";
        case 2:  return "WARN";
        case 3:  return "ERROR";
        default: return "UNKNOWN";
    }
}

void write_entry(std::ofstream& file, const LogEntry& entry) {
    file << "[ " << level_to_string(entry.level) << " ] " << entry.msg << "\n";
}

void log(rigtorp::SPSCQueue<LogEntry>& queue, std::ofstream& file, LogLevel level, std::string_view msg) {
    LogEntry entry { level, std::string(msg) };
    
    if (!queue.try_push(entry)) {
        write_entry(file, entry);
        file.flush();
    }
}

} // namespace log_ops

POE2OverlayLogger::POE2OverlayLogger(const std::string& filename)
    : queue(QUEUE_CAPACITY)
    , stop_flag(false)
{
    std::string log_path = "./log/";
    std::filesystem::create_directories(log_path);
    
    file.open(log_path + filename, std::ios::app);
    
    log_thread = std::thread(&POE2OverlayLogger::log_thread_func, this);
}

POE2OverlayLogger::~POE2OverlayLogger()
{
    stop_flag.store(true, std::memory_order_release);
    
    if (log_thread.joinable()) {
        log_thread.join();
    }
}

void POE2OverlayLogger::log_thread_func() {   
    while (!stop_flag.load(std::memory_order_acquire)) {
        LogEntry* entry = queue.front();
        if (entry) {
            log_ops::write_entry(file, *entry);
            queue.pop();
        } else {
            std::this_thread::yield();
        }
    }

    LogEntry* entry;
    while ((entry = queue.front()) != nullptr) {
        log_ops::write_entry(file, *entry);
        queue.pop();
    }
   
    file.flush();
}

void POE2OverlayLogger::debug(std::string_view msg) { log_ops::log(queue, file, LogLevel::DEBUG, msg); }
void POE2OverlayLogger::info (std::string_view msg) { log_ops::log(queue, file, LogLevel::INFO , msg); }
void POE2OverlayLogger::warn (std::string_view msg) { log_ops::log(queue, file, LogLevel::WARN , msg); }
void POE2OverlayLogger::error(std::string_view msg) { log_ops::log(queue, file, LogLevel::ERROR, msg); }
