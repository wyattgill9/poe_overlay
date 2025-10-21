#include "passive_tree.h"

void PassiveTree::connect_nodes(NodeId a, NodeId b) {
    if (a < graph_.size() && b < graph_.size()) {
        graph_[a].neighbors.push_back(b);
        graph_[b].neighbors.push_back(a);
    }
}

std::expected<void, PassiveTreeError> PassiveTree::add_node(NodeId id) {
    if (auto it = global_node_map_.find(id); it != global_node_map_.end()) {
        graph_.push_back(it->second);
    } else {
        return std::unexpected(PassiveTreeError::PASSIVE_TREE_UNKNOWN_NODE_ID);
    }
    return {};
}
