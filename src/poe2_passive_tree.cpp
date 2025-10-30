#include <array>
#include <bit>
#include <expected>
#include <fstream>
#include <ranges>
#include <rapidjson/reader.h>
#include <type_traits>
#include <vector>
#include <filesystem>

#include "poe2_passive_tree.h"
#include "poe_overlay.h"

namespace tree_opts {

std::vector<PassiveNode> id_to_nodes(std::vector<NodeId> node_ids) {
    std::vector<PassiveNode> passive_node;

    for(auto& node_id : node_ids) {
        passive_node.push_back(node_id_map[node_id]);
    }

    return passive_node;
}

template <typename T>
std::array<std::byte, sizeof(T)> to_bytes(const T& t) {
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(t);
}

std::vector<std::byte> graph_to_bytes(
    const std::vector<NodeId>& nodes,
    const std::vector<std::vector<size_t>>& adj_list
) {
    std::vector<std::byte> res;

    res.append_range(to_bytes(nodes.size()));

    auto node_bytes =
        nodes
        | rv::transform([](const std::remove_cvref_t<decltype(nodes)>::value_type elem) {
              return to_bytes(elem);
          })
        | rv::join
        | std::ranges::to<std::vector<std::byte>>();

    res.append_range(node_bytes);

    res.append_range(to_bytes(adj_list.size()));

    for(const auto& neighbors : adj_list) {
        res.append_range(to_bytes(neighbors.size()));

        for(size_type neightbor : neighbors) {
            res.append_range(to_bytes(neightbor));
        }
    }

    return res;
}    

// load entire tree from .bin file:w TODO
// POE2PassiveNodeTree(const std::string& file_path)
//     : passive_node_id_map(), nodes(0), adj_list(0) {}
void save_graph(
    const std::string& path,
    const std::vector<NodeId>& nodes,
    const std::vector<std::vector<size_t>>& adj_list
) {
    std::vector<std::byte> data = tree_opts::graph_to_bytes(nodes, adj_list);
    
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
}

void load_graph(
    const std::string& path,
    std::vector<NodeId> nodes,
    std::vector<std::vector<size_type>> adj_list
) {
    if(std::filesystem::exists(path)) {
        // load the file from the path
        
    } else {
        // return
        
    }
}

}; // tree_opts

POE2PassiveNodeTree::POE2PassiveNodeTree() {
    POE2PassiveNodeTree("graph.bin"); 
}

POE2PassiveNodeTree::POE2PassiveNodeTree(const std::string& bin_path)
    : nodes(0), adj_list(0)
{
    tree_opts::load_graph(bin_path, nodes, adj_list);
}

// for the first pass (without .bin file)
size_type POE2PassiveNodeTree::add_node(NodeId id) {
    nodes.push_back(id);
    return nodes.size() - 1; // returns the position of id in nodes vec
}

// adj[i] = list of nodes[i] neighbors
// adj[i] = vec<NodeId>
void POE2PassiveNodeTree::add_edge(size_type a_loc, size_type b_loc) {
    NodeId first  = nodes[a_loc];
    NodeId second = nodes[b_loc];

    adj_list[a_loc].push_back(second);
    adj_list[b_loc].push_back(first);
}
