#include "passive_tree.h"

void PassiveTree::add_node(NodeId id, const std::string& name, NodeType type) {
    graph_.emplace_back(id, name, type);
}

void PassiveTree::connect_nodes(NodeId a, NodeId b) {
    graph_[a].neighbors.push_back(b);
    graph_[b].neighbors.push_back(a);
}
