#pragma once

#include <minbase/crossplat.h>

THIRDPARTY_INCLUDES_BEGIN
#include <json-cpp/value.h>
THIRDPARTY_INCLUDES_END

namespace vi {

Json::Value json_from_string(std::string const& str);

Json::Value json_from_file(std::string const& filename);

void json_to_file(std::string const& path, Json::Value const& value);

void validate_json(Json::Value const& root, Json::Value const& schema_js);

} //ns vi