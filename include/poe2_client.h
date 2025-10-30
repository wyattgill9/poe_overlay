#pragma once

#include <expected>
#include <vector>
#include <string>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "poe_logger.h"
#include "poe2_passive_tree.h"

enum class POE2OverlayHTTPError : u8 {
    RESPONSE_UNAVAILABLE = 0,
    JSON_PARSE_ERROR = 1,
    RESPONSE_INVALID = 2,
    RESPONSE_MISSING_REQUIRED_FIELD = 3,
};

struct HTTPRequestData {
    std::string      body;
    httplib::Headers headers;
};

namespace http_ops {
    void configure_client(httplib::SSLClient& client);
    
    HTTPRequestData create_request(const std::string& build_name);
    
    std::expected<Document, POE2OverlayHTTPError> 
    send_request(
        httplib::SSLClient& client,
        const HTTPRequestData& request,
        POE2OverlayLogger& logger
    );
    
    std::expected<std::vector<NodeId>, POE2OverlayHTTPError>
    extract_passive_nodes(const Document& res_body, POE2OverlayLogger& logger);
}

std::expected<std::vector<NodeId>, POE2OverlayHTTPError>
fetch_passive_tree_nodes(
    const std::string& build_name,
    POE2OverlayLogger& logger
);
