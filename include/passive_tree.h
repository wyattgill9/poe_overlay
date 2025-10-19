#pragma once

#include <expected>
#include <string>
#include <vector>

#include "poe_overlay.h"

// namespace poe {   

enum class POE2OverlayError : u8 {
    PASSIVE_TREE_UNKNOWN_NODE_ID = 0,
    JSON_RESPONSE_ERROR          = 1,
};

using NodeId = i32;

enum class NodeType {
    Normal,
    Notable,
    Keystone,
    JewelSocket
};

struct PassiveNode {
    NodeId      id;
    std::string name;
    NodeType    type { NodeType::Normal };

    std::vector<NodeId> neighbors;
};

class PassiveTree {
public:
    std::expected<void, POE2OverlayError> add_node(NodeId id);
    void connect_nodes(NodeId a, NodeId b);

private:
    std::vector<PassiveNode> graph_;
    static inline std::unordered_map<NodeId, PassiveNode> global_node_map_;
};

// };
