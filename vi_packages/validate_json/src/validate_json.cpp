#include <validate_json/validate_json.h>

THIRDPARTY_INCLUDES_BEGIN
#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>
THIRDPARTY_INCLUDES_END

#include <fstream>

namespace vi {

Json::Value json_from_string(std::string const& str)
{
  Json::Reader reader;
  std::stringstream schema_stream(str);
  Json::Value doc;
  if (!reader.parse(schema_stream, doc, false))
    throw std::runtime_error("Unable to parse JSON from string: "
                             + reader.getFormattedErrorMessages());
  return doc;
}

Json::Value json_from_file(std::string const& filename)
{
  Json::Reader reader;
  std::ifstream stream(filename, std::ios_base::in);
  if (!stream.is_open()) {
    throw std::runtime_error("vi::json_from_file(" + filename + "): cannot open file for reading");
  }
  Json::Value doc;
  if (!reader.parse(stream, doc, false))
    throw std::runtime_error("Unable to parse JSON from file " + filename + ": "
                             + reader.getFormattedErrorMessages());
  return doc;
}

void json_to_file(std::string const& path, Json::Value const& value)
{
  try
  {
    Json::StyledWriter writer;
    std::ofstream ofs(path);
    ofs << writer.write(value);
  }
  catch (std::exception const& e)
  {
    throw std::runtime_error("write_json: failed to write json to file: " + path +
                             ": " + e.what());
  }
}

void validate_json(Json::Value const& root, Json::Value const& schema_js)
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::ValidationResults;
  using valijson::adapters::JsonCppAdapter;

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
    throw std::runtime_error(err_oss.str());
  }
}

} //ns vi