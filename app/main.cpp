#include <iostream>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "json.hpp"
using json = nlohmann::json;

// #include "absl/container/flat_hash_map.h"

#include "base.hpp"
using namespace base;

#include "passive_tree.h"

int main() {
    PassiveTree tree;
    tree.add_node(0, "Hi");
     
    httplib::SSLClient cli("mobalytics.gg");
    cli.set_connection_timeout(10, 0); 
    cli.set_read_timeout(10, 0);
    cli.set_write_timeout(10, 0);
    
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
        {"slug", "lightning-arrow-farmer-fubgun"},
        {"type", "builds"},
        {"widgetsOverride", json::array()}
      }}
    };
    
    std::string json_body = j.dump();
    
    httplib::Headers headers = {
        { "Content-Type", "application/json" },
        { "x-apollo-operation-name", "Poe2PassiveTreeQuery" },
        { "Origin", "https://mobalytics.gg" },
        { "Referer", "https://mobalytics.gg/poe-2/builds/lightning-arrow-farmer-fubgun" },
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36" }
    };
    
    cli.enable_server_certificate_verification(false);
    
    auto res = cli.Post("/api/poe-2/v1/graphql/query", headers, json_body, "application/json");
    
    if(!res) {        
        auto err = res.error();
        io::println("HTTP request failed!");
        io::println("Error: " + httplib::to_string(err));
        return 0;
    }

    json body = json::parse(res->body);

    // todo count number of build varients
    io::println(body["data"]["game"]["documents"]["userGeneratedDocumentBySlug"]
                ["data"]["data"]["buildVariants"]["values"][0]["passiveTree"]);

    return 0;
}
