#pragma once
#include <string>
#include <vector>

using NodeId = int;

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

    PassiveNode(NodeId id, std::string name, NodeType type = NodeType::Normal)
        : id(id), name(std::move(name)), type(type) {}
};

class PassiveTree {
private:
    std::vector<PassiveNode> graph_;

public:
    PassiveTree() = default;

    void add_node(NodeId id, const std::string& name, NodeType type = NodeType::Normal);
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
