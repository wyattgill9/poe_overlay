#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"

#include "passive_tree.h"

using json = nlohmann::json;

int main() {
    PassiveTree tree;
    tree.add_node(0, "Hi");
     
    httplib::SSLClient cli("mobalytics.gg");

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
        {"widgetsOverride", json::array()} // widgetsOverride: {}
      }}
    };

    std::string body = j.dump();

    httplib::Headers headers = {
        { "Content-Type", "application/json" },
        { "x-apollo-operation-name", "Poe2PassiveTreeQuery" },
        { "Origin", "https://mobalytics.gg" },
        { "Referer", "https://mobalytics.gg/poe-2/builds/lightning-arrow-farmer-fubgun" },
        { "User-Agent", "poe_overlay_client" }
    };

    auto res = cli.Post("/api/poe-2/v1/graphql/query", headers, body, "application/json");

    if (res) {
        std::cout << res->body << std::endl;
    }

    return 0;
}
