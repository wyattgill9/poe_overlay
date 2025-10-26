#include <vector>

#include "passive_tree.h"

namespace tree_opts {

std::vector<PassiveNode> id_to_nodes(std::vector<NodeId> node_ids) {
    std::vector<PassiveNode> passive_node;

    for(auto& node_id : node_ids) {
        passive_node.push_back(passive_node_id_map[node_id]);
    }

    return passive_node;
}

}; // tree_opts
