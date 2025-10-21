#include "base.hpp"
#include <expected>
#include <openssl/err.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "passive_tree.h"
#include "poe2_client.h"
#include "poe_overlay.h"

namespace http_ops {

void configure_client(httplib::SSLClient& c) {
    c.set_follow_location(true);
    c.set_connection_timeout(50, 0); 
    c.set_read_timeout(30, 0);
    c.set_write_timeout(30, 0);
}

HTTPRequestData create_request(const std::string& build_name) { 
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

    return HTTPRequestData { j.dump(), headers };
}

std::expected<json, POE2OverlayHTTPError>
send_request(HTTPClientContext& ctx, POE2OverlayLogger& logger) {
    configure_client(ctx.client);

    HTTPRequestData req = create_request(ctx.build_name);

    auto res = ctx.client.Post(
        "/api/poe-2/v1/graphql/query",
        req.headers,
        req.body,
        "application/json"
    );

    if (!res) {
        return std::unexpected(POE2OverlayHTTPError::RESPONSE_UNAVAILABLE);
    }

    try {
        return json::parse(res->body);
    } catch (...) {
        return std::unexpected(POE2OverlayHTTPError::JSON_PARSE_ERROR);
    }
}

std::expected<std::vector<NodeId>, POE2OverlayHTTPError>
extract_passive_nodes(const json& res_body, POE2OverlayLogger& logger) {
    // if(!res_body) {}
    // io::println("Hell");

    const auto& nodes_json = res_body["data"]["game"]["documents"]["userGeneratedDocumentBySlug"]
                                     ["data"]["data"]["buildVariants"]["values"][0]["passiveTree"]
                                     ["mainTree"]["selectedSlugs"];
    if(!nodes_json.is_array()) {
        return std::unexpected(POE2OverlayHTTPError::RESPONSE_MISSING_REQUIRED_FIELD);
    }

    std::vector<std::string> unparsed_node_ids = nodes_json.get<std::vector<std::string>>();
    std::vector<NodeId> node_ids;
              
    for(const auto& node : unparsed_node_ids) {
        auto pos = node.find("-"); // "node-xxx"
        node_ids.push_back(std::stoi(node.substr(pos + 1)));
    }

    return node_ids;
}

};

HTTPClientContext::HTTPClientContext(std::string build_name_)
    : build_name(build_name_), client(httplib::SSLClient("mobalytics.gg", 443)) {}

POE2OverlayHTTPClient::POE2OverlayHTTPClient(std::string build_name_, POE2OverlayLogger& logger_)
    : ctx(HTTPClientContext(build_name_)), logger(logger_) {}

std::expected<std::vector<NodeId>, POE2OverlayHTTPError>
POE2OverlayHTTPClient::fetch_nodes() {
    auto json_res = http_ops::send_request(ctx, logger);

    if (!json_res.has_value()) {
        return std::unexpected(json_res.error()); // percalate the error up
    }
    
    return http_ops::extract_passive_nodes(json_res.value(), logger);
}
