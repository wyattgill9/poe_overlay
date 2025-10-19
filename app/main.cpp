#include "passive_tree.h"
#include "poe2_client.h"
#include "poe_logger.h"

#include <memory>

auto main() -> int {
    auto logger = std::make_shared<POE2OverlayLogger>("log.txt");
    PassiveTree tree;
    
    auto client = POE2OverlayHTTPClient("lightning-arrow-farmer-fubgun", logger);
    auto body = client.fetch_content();

    if(!body.has_value()) {
        return 0;
    }

    client.parse_content(body.value());
          
    return 0;
}
