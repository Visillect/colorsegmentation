/*
Copyright (c) 2012-2018, Visillect Service LLC. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of copyright holders.
*/


#include "utils.h"
#include <i8r/types.h>
#include <cstdio>
#include <json-cpp/reader.h>
#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

namespace i8r {

std::string unsafe_fmt_number(int num, char const* fmt)
{
  char buf[22] = {0}; // 2**64 seems to be 19 digits + '-' + '\0' + one extra
  sprintf(buf, fmt, num);
  return buf;
}

void parse_json(Json::Value & doc, std::string const& str)
{
  if (str.empty())
    return;
  Json::Reader reader;
  if (!reader.parse(str, doc))
    throw Exception("Error parsing config: " + reader.getFormattedErrorMessages());
}

void validate_json(Json::Value const& root, std::string const& schema_str)
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::ValidationResults;
  using valijson::adapters::JsonCppAdapter;

  Json::Value schema_js;
  {
    Json::Reader reader;
    std::stringstream schema_stream(schema_str);
    if (!reader.parse(schema_stream, schema_js, false))
      throw Exception("Unable to parse the embedded schema: "
                               + reader.getFormattedErrorMessages());
  }

  JsonCppAdapter doc(root);
  JsonCppAdapter schema_doc(schema_js);

  SchemaParser parser(SchemaParser::kDraft4);
  Schema schema;
  parser.populateSchema(schema_doc, schema);
  Validator validator(schema);
  validator.setStrict(false);
  ValidationResults results;
  if (!validator.validate(doc, &results))
  {
    std::stringstream err_oss;
    err_oss << "Validation failed." << std::endl;
    ValidationResults::Error error;
    int error_num = 1;
    while (results.popError(error))
    {
      std::string context;
      std::vector<std::string>::iterator itr = error.context.begin();
      for (; itr != error.context.end(); itr++)
        context += *itr;

      err_oss << "Error #" << error_num << std::endl
              << "  context: " << context << std::endl
              << "  desc:    " << error.description << std::endl;
      ++error_num;
    }
    throw Exception(err_oss.str());
  }
}

void override_json_objects(Json::Value & out, Json::Value const& overrides)
{
  if (out.isNull() )
  {
    out = overrides;
  }
  else if (overrides.isNull())
  {
    // do nothing
  }
  else if (out.isObject() && overrides.isObject())
  {
    for (std::string const& key : overrides.getMemberNames())
    {
      if (out.isMember(key))
      {
        Json::Value & val_to_override = out[key];
        Json::Value const& overriding_val = overrides[key];
        if (val_to_override.isObject() && overriding_val.isObject())
          override_json_objects(val_to_override, overriding_val);
        else
          val_to_override = overriding_val;
      }
      else
      {
        out[key] = overrides[key];
      }
    }
  }
  else
  {
    throw Exception("Either one of documents is not an object or null");
  }
}

} // ns i8r
