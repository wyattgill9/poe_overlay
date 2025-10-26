#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "poe_overlay.h"

using NodeId = i32; // int

enum class NodeType : u8 {
    Normal      = 0,
    Notable     = 1,
    Keystone    = 2,
    JewelSocket = 3,
};

struct PassiveNode {
    NodeId      id;
    std::string name;
    NodeType    type { NodeType::Normal };
};

// todo make this constexpr by loading it from a file in the form
// { node_id : (id, name, NodeType) } or smt like that
static std::unordered_map<NodeId, PassiveNode> passive_node_id_map;


// this should be generated at compile time technically
class PassiveNodeTree {
public:

private:
    std::vector<PassiveNode>         graph;
    std::vector<std::vector<NodeId>> neighbors; // neighbors[graph[i]] = list of graph's neighbors
};

namespace tree_opts {
    std::vector<PassiveNode> id_to_nodes(std::vector<NodeId> node_ids);
};
