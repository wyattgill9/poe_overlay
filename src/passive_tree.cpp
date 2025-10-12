#include "passive_tree.h"

void PassiveTree::add_node(NodeId id) {
    auto node = global_node_map[id];

    graph_.emplace_back(node);
}

void PassiveTree::connect_nodes(NodeId a, NodeId b) {
    graph_[a].neighbors.push_back(b);
    graph_[b].neighbors.push_back(a);
}
