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

    auto nodes_json = body.value()["data"]["game"]["documents"]["userGeneratedDocumentBySlug"]
                ["data"]["data"]["buildVariants"]["values"][0]["passiveTree"]
                ["mainTree"]["selectedSlugs"];

    if(!nodes_json.is_array()) {
        return 0;
    }

    std::vector<std::string> unparsed_nodes = nodes_json.get<std::vector<std::string>>();

    std::vector<NodeId> node_ids;
    std::unordered_map<NodeId, PassiveNode> node_map;
              
    for(auto node : unparsed_nodes) {
        auto pos = node.find("-");
        node_ids.push_back(std::stoi(node.substr(pos + 1)));         
    }

    for(auto node_id : node_ids) {
        auto _ = tree.add_node(node_id);
        io::println(node_id);
    }
        
    return 0;
}
