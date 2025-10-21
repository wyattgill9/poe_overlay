#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "json.hpp"
using json = nlohmann::json;

#include "base.hpp"
using namespace base;

namespace poe {};
