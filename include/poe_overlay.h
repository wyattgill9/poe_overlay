#pragma once

#include <ranges>
namespace rv = std::views;

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
using namespace rapidjson;

#include "base.hpp"

namespace poe {};
