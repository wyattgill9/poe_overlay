#pragma once
#include <string>
#include <vector>

#include "base.hpp"
using namespace base::types;

using NodeId = i32;

enum class NodeType {
    Normal,
    Notable,
    Keystone,
    JewelSocket
};

struct PassiveNode {
    NodeId id;
    std::string name;
    NodeType type;
    std::vector<NodeId> neighbors;

    // PassiveNode(NodeId id, std::string name, NodeType type = NodeType::Normal)
        // : id(id), name(std::move(name)), type(type) {}
};

// todo make global node_map
static std::unordered_map<NodeId, PassiveNode> global_node_map;

class PassiveTree {
private:
    std::vector<PassiveNode> graph_;

public:
    PassiveTree() = default;

    void add_node(NodeId id);
    void connect_nodes(NodeId a, NodeId b);
};

class PassiveTreePath {
private:
    std::vector<NodeId> path_;

public:
    explicit PassiveTreePath(std::vector<NodeId> path)
        : path_(std::move(path)) {}

    [[nodiscard]] const std::vector<NodeId>& nodes() const noexcept { return path_; }
};
