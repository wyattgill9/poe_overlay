#include <openssl/err.h>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "passive_tree.h"
#include "poe2_client.h"

namespace http_ops {

void configure_client(httplib::SSLClient& c) {
    c.set_follow_location(true);
    c.set_connection_timeout(50, 0); 
    c.set_read_timeout(30, 0);
    c.set_write_timeout(30, 0);
}

std::string to_string(const Document& o) {
	StringBuffer sb;
	Writer<StringBuffer> writer(sb);
	o.Accept(writer);
	return sb.GetString();
}


HTTPRequestData create_request(const std::string& build_name) { 
    const char* query = R"(query Poe2PassiveTreeQuery($input: Poe2UserGeneratedDocumentInputBySlug!) {
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

    // Construct the actual JSON body
    Document json_body;
    json_body.SetObject();
    Document::AllocatorType& allocator = json_body.GetAllocator();

    json_body.AddMember("query", Value(query, allocator), allocator);

    Value variables(kObjectType);
    Value input(kObjectType);

    input.AddMember("slug", Value(build_name.c_str(), allocator), allocator);
    input.AddMember("type", "builds", allocator);

    Value widgetsOverride(kArrayType);
    input.AddMember("widgetsOverride", widgetsOverride, allocator);

    variables.AddMember("input", input, allocator);
    json_body.AddMember("variables", variables, allocator);

    // JSON headers 
    static httplib::Headers headers = {
        { "Content-Type", "application/json" },
        { "x-apollo-operation-name", "Poe2PassiveTreeQuery" },
        { "Origin", "https://mobalytics.gg" },
        { "Referer", "https://mobalytics.gg/poe-2/builds/" + build_name },
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36" }
    };

    return HTTPRequestData { to_string(json_body), headers };
}

std::expected<Document, POE2OverlayHTTPError>
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
        // return json::parse(res->body);
        Document json_res;
        json_res.Parse(res->body.c_str());
        return json_res;

    } catch (...) {
        return std::unexpected(POE2OverlayHTTPError::JSON_PARSE_ERROR);
    }
}

std::expected<std::vector<NodeId>, POE2OverlayHTTPError>
extract_passive_nodes(const Document& res_body, POE2OverlayLogger& logger) {
    const auto& nodes_json = res_body["data"]["game"]["documents"]["userGeneratedDocumentBySlug"]
                                     ["data"]["data"]["buildVariants"]["values"][0]["passiveTree"]
                                     ["mainTree"]["selectedSlugs"];
    if(!nodes_json.IsArray()) {
        return std::unexpected(POE2OverlayHTTPError::RESPONSE_MISSING_REQUIRED_FIELD);
    }

    auto view =
        nodes_json.GetArray()
        | rv::filter   ([](const auto& elem) { return elem.IsString();               })
        | rv::transform([](const auto& elem) { return std::string(elem.GetString()); })
        | rv::transform([](const std::string& s){
              auto pos = s.find('-');
              return static_cast<NodeId>(std::stoi(s.substr(pos + 1)));
          });

    std::vector<NodeId> node_ids(view.begin(), view.end());    

    return node_ids;
}

}; // http_client_ops

HTTPClientContext::HTTPClientContext(std::string build_name_)
    : build_name(build_name_), client(httplib::SSLClient("mobalytics.gg", 443)) {}

POE2OverlayHTTPClient::POE2OverlayHTTPClient(std::string build_name_, POE2OverlayLogger& logger_)
    : ctx(HTTPClientContext(build_name_)), logger(logger_) {}

std::expected<std::vector<NodeId>, POE2OverlayHTTPError>
POE2OverlayHTTPClient::fetch_nodes()
{
    auto json_res = http_ops::send_request(ctx, logger);

    if (!json_res.has_value()) {
        return std::unexpected(json_res.error()); // percalate the error up
    }
    
    return http_ops::extract_passive_nodes(json_res.value(), logger);
}
