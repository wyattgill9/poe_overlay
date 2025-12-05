#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "poe_overlay.h"

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
// this should be generated at compile time technically
// TODO: (everything below this)

static std::unordered_map<NodeId, PassiveNode> node_id_map;

struct POE2PassiveTree {
    std::vector<NodeId> nodes;        // each node's ID
    std::vector<size_type> neighbors; // flattened adjacency list
    std::vector<size_type> offsets;   // offsets[i] = start of node i's neighbors
};

namespace tree_ops {
    std::vector<PassiveNode> id_to_nodes(std::vector<NodeId> node_ids);
        
    void save_graph(const std::string& path, POE2PassiveTree& graph);
    void load_graph(const std::string&, POE2PassiveTree& graph);
};

// size_t index_from_id(const Graph& g, NodeId id) {
//     // linear search (slow if large, usually you'd have a hash map)
//     for (size_t i = 0; i < g.nodes.size(); ++i)
//         if (g.nodes[i] == id)
//             return i;
//     throw std::runtime_error("Node ID not found");
// }

// std::span<const NodeId> get_neighbors_by_id(const Graph& g, NodeId id);
// std::span<const size_type> get_neighbors(const Graph& g, size_t n) {
//     const auto start = g.offsets[n];
//     const auto end   = g.offsets[n + 1];
//     return std::span<const size_type>(g.neighbors.data() + start, end - start);
// }
