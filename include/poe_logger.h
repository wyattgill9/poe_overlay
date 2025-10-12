#include <fstream>
#include <string>

#include "base.hpp"
using namespace base::types;

struct LogMessage {
    u8          level;
    std::string data;

    LogMessage(std::string data, u8 level)
        : level(level), data(data) {}

    std::string level_to_string() const;
};

class POE2OverlayLogger {
private:
    base::containers::BoundedPointerSPSC<LogMessage*> queue;
    std::ofstream file;

public:
    POE2OverlayLogger(const std::string& filename);

    ~POE2OverlayLogger();

    void log(u8 level, const std::string& msg);

    void debug(const std::string& msg);
    void info (const std::string& msg);
    void warn (const std::string& msg);
    void error(const std::string& msg);
};
