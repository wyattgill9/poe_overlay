#include <atomic>
#include <filesystem>
#include <fstream>
#include <thread>
#include <utility>

#include "poe_logger.h"
#include "SPSCQueue.h"

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

void log_thread_func(LogContext& ctx) {   
    while (!ctx.stop_flag.load(std::memory_order_acquire)) {
        LogEntry* entry = ctx.queue.front();
        if (entry) {
            log_ops::write_entry(ctx.file, *entry);
            ctx.queue.pop();
        } else {
            std::this_thread::yield();
        }
    }

    LogEntry* entry;
    while ((entry = ctx.queue.front()) != nullptr) {
        log_ops::write_entry(ctx.file, *entry);
        ctx.queue.pop();
    }
   
    ctx.file.flush();
}

} // namespace log_ops

POE2OverlayLogger::POE2OverlayLogger(const std::string &filename)
    : ctx {
        .queue = rigtorp::SPSCQueue<LogEntry>(QUEUE_CAPACITY),
        .file = std::ofstream("../log/" + filename),
        .stop_flag = false,
      }
{
    log_thread = std::thread(log_ops::log_thread_func, std::ref(ctx));
}

POE2OverlayLogger::~POE2OverlayLogger() {
    ctx.stop_flag.store(true, std::memory_order_relaxed);

    if(log_thread.joinable()) {
        log_thread.join();
    }
}
   
void POE2OverlayLogger::log(LogLevel level, const std::string& msg) {
    LogEntry entry { .level = level, .msg = msg};
    log_ops::write_entry(ctx.file, entry);
}
