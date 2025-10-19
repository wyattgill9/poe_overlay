#include "passive_tree.h"
#include "poe2_client.h"
#include "poe_logger.h"

POE2OverlayHTTPClient::POE2OverlayHTTPClient(std::string_view build_name, std::shared_ptr<POE2OverlayLogger> logger)
    : build_name_(build_name), client_(httplib::Client("mobalytics.gg", 443)), logger_(logger) {}

std::expected<json, POE2OverlayError>
POE2OverlayHTTPClient::fetch_content() {
    set_client_config(client_);
    auto [json, headers] = get_request_data(build_name_);

    auto res = client_.Post("/api/poe-2/v1/graphql/query", headers, json, "application/json");

    if(!res) {        
        auto err = res.error();

        // io::println("HTTP request failed!");
        // io::println("Error: " + httplib::to_string(err));
        return std::unexpected(POE2OverlayError::JSON_RESPONSE_ERROR);
    }

    return json::parse(res->body);
}

void set_client_config(httplib::Client& c) {
    c.set_follow_location(true);
    c.set_connection_timeout(50, 0); 
    c.set_read_timeout(30, 0);
    c.set_write_timeout(30, 0);
}

std::pair<jason_string, httplib::Headers> get_request_data(std::string& build_name) { 
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
            {"slug", build_name},
            {"type", "builds"},
            {"widgetsOverride", json::array()}
        }}
    };

    static httplib::Headers headers = {
        { "Content-Type", "application/json" },
        { "x-apollo-operation-name", "Poe2PassiveTreeQuery" },
        { "Origin", "https://mobalytics.gg" },
        { "Referer", "https://mobalytics.gg/poe-2/builds/" + build_name },
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36" }
    };

    return { j.dump(), headers };
}
