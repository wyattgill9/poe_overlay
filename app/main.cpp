// #include "passive_tree.h"
#include "base.hpp"
#include <print>
#include "poe2_client.h"
#include "poe_logger.h"


auto main() -> int {
    auto logger = POE2OverlayLogger("log.txt");
    
    POE2OverlayHTTPClient client = POE2OverlayHTTPClient("lightning-arrow-farmer-fubgun", logger);
    auto node_ids = client.fetch_nodes();
    if(!node_ids.has_value()) {
        return 0;
    }

    for(const auto& node_id : node_ids.value()) {
        io::println(node_id);
    }

    return 0;
}
