#pragma once

#include <memory>
#include <string_view>

#include "poe_logger.h"
#include "poe_overlay.h"
#include "passive_tree.h"

class POE2OverlayHTTPClient {
public:
    POE2OverlayHTTPClient(std::string_view build_name, std::shared_ptr<POE2OverlayLogger> logger);
    std::expected<json, POE2OverlayError> fetch_content(); 

private:
    std::shared_ptr<POE2OverlayLogger> logger_;
    std::string build_name_;
    mutable httplib::Client client_;
};

// core ff
using jason_string = std::string;

void set_client_config(httplib::Client& c);

std::pair<jason_string, httplib::Headers>
get_request_data(std::string& build_name);
