// #include <fstream>

// #include "base.hpp"
#include "poe_overlay.h"
#include "poe2_client.h"
#include "poe_logger.h"

auto main() -> int {
    auto logger = POE2OverlayLogger("log.txt");
    auto nodes  = fetch_passive_tree_nodes("lightning-arrow-farmer-fubgun", logger);
                  
    if(!nodes.has_value()) {
        return 0;
    }

    for(const auto& node_id : nodes.value()) {
        io::println(node_id);
        logger.log(LogLevel::INFO, std::to_string(node_id));
    }

    // std::ifstream f("tree.json");   
    // rapidjson::IStreamWrapper isw(f);

    // Document data;
    // data.ParseStream(isw);

    // std::string s = stringify(data);
    // io::println(s);
    
    return 0;
}
