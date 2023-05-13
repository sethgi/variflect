#pragma once

#include <nlohmann/json.hpp>

#include <variant.hh>

using json = nlohmann::json;

namespace variflect {

// Note: This class uses snake case functions to remain consistent with 
// requirement of inheritance. Sucks, but is what it is.
class JSONSaxInterface : public json::json_sax_t {

 public:
  JSONSaxInterface() = delete;
  JSONSaxInterface(Variant* variant);

  bool null();
  bool boolean(bool val);
  bool number_integer(json::number_integer_t val);
  bool number_unsigned(json::number_unsigned_t val);
  bool number_float(json::number_float_t val, const std::string& s);
  bool string(json::string_t& val);
  bool binary(json::binary_t& val);

  bool start_object(std::size_t elements);
  bool end_object();

  bool start_array(std::size_t elements);
  bool end_array();

  bool key(json::string_t& val);

  bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::detail::exception& ex);

 private:

  enum class ParseType {
    EMPTY,
    MAP,
    LIST
  };

  using ParseState = std::pair<Variant*, ParseType>;

  Variant* get_current_location();
  Variant* get_insert_location();

  ParseType& get_parse_type();

  std::vector<ParseState> state_stack_;

  Variant* root_data_;

  std::string active_key_;
  bool key_valid_;

  template <typename JsonType, typename OutputType>
  bool set_field(const JsonType& data);
};


}