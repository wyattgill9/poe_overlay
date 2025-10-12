#include <unordered_map>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;

#include "base.hpp"
using namespace base;

#include "passive_tree.h"

// very rough, will refactor completly later;
auto main() -> i32 {
    PassiveTree tree;
    
    std::string build_name = "lightning-arrow-farmer-fubgun"; 
    httplib::SSLClient cli("mobalytics.gg", 443);

    cli.set_follow_location(true);
    cli.set_connection_timeout(50, 0); 
    cli.set_read_timeout(30, 0);
    cli.set_write_timeout(30, 0);
    cli.enable_server_certificate_verification(false);

    std::string query = R"(query Poe2PassiveTreeQuery($input: Poe2UserGeneratedDocumentInputBySlug!) {
      game: poe2 {
        documents {
          userGeneratedDocumentBySlug(input: $input) {
            data {
              data {
                buildVariants {
                  values {
                    passiveTree {
                      mainTree {
                        selectedSlugs
                        priorityList {
                          slug
                          name
                          type
                          iconURL
                          description
                        }
                      }
                      ascendancyTree {
                        selectedSlugs
                        priorityList {
                          slug
                          name
                          type
                          iconURL
                          description
                        }
                      }
                      attributeNodes { attribute nodeSlug }
                    }
                  }
                }
              }
            }
          }
        }
      }
    })";
    
    json j;
    j["query"] = query;
    j["variables"] = {
      {"input", {
        // {"slug", "lightning-arrow-farmer-fubgun"},
        {"slug", build_name},
        {"type", "builds"},
        {"widgetsOverride", json::array()}
      }}
    };
    
    std::string json_body = j.dump();
    
    httplib::Headers headers = {
        { "Content-Type", "application/json" },
        { "x-apollo-operation-name", "Poe2PassiveTreeQuery" },
        { "Origin", "https://mobalytics.gg" },
        { "Referer", "https://mobalytics.gg/poe-2/builds/" + build_name },
        // { "Referer", "https://mobalytics.gg/poe-2/builds/lightning-arrow-farmer-fubgun" },
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36" }
    };
        
    auto res = cli.Post("/api/poe-2/v1/graphql/query", headers, json_body, "application/json");

    
    if(!res) {        
        auto err = res.error();
        io::println("HTTP request failed!");
        io::println("Error: " + httplib::to_string(err));
        return 0;
    }

    json body = json::parse(res->body);

    // todo count number of build varients
    auto nodes_json = body["data"]["game"]["documents"]["userGeneratedDocumentBySlug"]
                ["data"]["data"]["buildVariants"]["values"][0]["passiveTree"]
                ["mainTree"]["selectedSlugs"];

    if(!nodes_json.is_array()) {
        return 0;
    }

    std::vector<std::string> unparsed_nodes = nodes_json.get<std::vector<std::string>>();

    std::vector<NodeId> node_ids;
    std::unordered_map<NodeId, PassiveNode> node_map;
              
    for(auto node : unparsed_nodes) {
        auto pos = node.find("-");
        node_ids.push_back(std::stoi(node.substr(pos + 1)));         
    }

    for(auto node_id : node_ids) {
        tree.add_node(node_id);
    }
        
    return 0;
}
