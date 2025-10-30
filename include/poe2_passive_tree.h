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

// struct POE2PassiveTree {
    // std::vector<NodeId> nodes;        // each node's ID
    // std::vector<size_type> neighbors; // flattened adjacency list
    // std::vector<size_type> offsets;   // offsets[i] = start of node i's neighbors    
// }
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

struct POE2PassiveNodeTree {
public:
    POE2PassiveNodeTree();
    POE2PassiveNodeTree(const std::string& bin_path);

    // for the first pass (without .bin file)
    // returns the position of id in graph vec
    size_type add_node(NodeId id);
    void add_edge(size_type a_loc, size_type b_loc);

private:
    std::vector<NodeId>                 nodes;
    std::vector<std::vector<size_type>> adj_list; // adj[i] = list of nodes[i] neighbors
};

namespace tree_opts {
    std::vector<PassiveNode> id_to_nodes(std::vector<NodeId> node_ids);

    std::vector<std::byte> graph_to_bytes(
        const std::vector<NodeId>& nodes,
        const std::vector<std::vector<size_t>>& adj_list
    );
        
    void save_graph(
        const std::string& path,
        const std::vector<NodeId>& graph,
        const std::vector<std::vector<size_t>>& adj_list
    );

    void load_graph(
        const std::string&,
        std::vector<NodeId> nodes,
        std::vector<std::vector<size_type>> adj_list
    );
};

