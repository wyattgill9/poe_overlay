#include <fstream>
#include <ranges>
#include <rapidjson/reader.h>
#include <vector>

#include "poe2_passive_tree.h"
#include "poe2_passive_tree.h"
#include "poe_overlay.h"

namespace tree_ops {

std::vector<PassiveNode> id_to_nodes(std::vector<NodeId> node_ids) {
    return node_ids 
        | rv::transform([](NodeId id){
            return node_id_map[id];
        })
        |std::ranges::to<std::vector<PassiveNode>>();
}


void save_graph(const std::string& path, POE2PassiveTree& graph) {
    std::vector<std::byte> data {};

    std::ofstream file(path);
    if(!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
}

void load_graph(const std::string&, POE2PassiveTree& graph) {
    
}

}; // tree_opts

