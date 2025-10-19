#pragma once

#include <expected>
#include <vector>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <memory>
#include <string_view>

#include "poe_logger.h"
#include "poe_overlay.h"
#include "passive_tree.h"

class POE2OverlayHTTPClient {
public:
    POE2OverlayHTTPClient(std::string_view build_name, std::shared_ptr<POE2OverlayLogger> logger);

    std::expected<json, POE2OverlayError> fetch_content(); 
    std::expected<std::vector<NodeId>, POE2OverlayError> parse_content(json json_body);

private:
    std::shared_ptr<POE2OverlayLogger> logger_;
    std::string build_name_;
    mutable httplib::SSLClient client_;
};

// core ff
void set_client_config(httplib::SSLClient& c);

std::pair<std::string, httplib::Headers>
get_request_data(std::string& build_name);
