#include "json_parser.hh"

#include <nlohmann/detail/input/json_sax.hpp>

namespace variflect {

JSONSaxInterface::JSONSaxInterface(Variant* variant)
  : root_data_(variant),
    key_valid_(false),
    state_stack_{{variant, ParseType::EMPTY}}
{
}

bool JSONSaxInterface::key(json::string_t& val) {
  ParseType& parse_type = get_parse_type();
  if(parse_type == ParseType::EMPTY) {
    parse_type = ParseType::MAP;
  } else if(parse_type == ParseType::LIST) {
    throw std::runtime_error("Can't parse key in list. JSON appears invalid.");
  }

  active_key_ = static_cast<StringType>(val);
  key_valid_ = true;

  return true;
}

Variant* JSONSaxInterface::get_current_location() {
  return state_stack_.back().first;
}

JSONSaxInterface::ParseType& JSONSaxInterface::get_parse_type() {
  return state_stack_.back().second;
}

Variant* JSONSaxInterface::get_insert_location() {
  Variant* current_location = get_current_location();

  auto& parse_type = get_parse_type();
  if(current_location->GetType() == VariantTypeEnum::LIST) {
      VariantList& vlist = current_location->AsList();
      vlist.emplace_back();
      current_location = &vlist.back();
  }

  if(key_valid_) {
    VariantMap& vmap = current_location->AsMap();
    vmap.emplace(active_key_, Variant());
    key_valid_ = false;
    return &vmap.at(active_key_);
  } else {
    const bool can_insert_primitive = 
      (parse_type == ParseType::EMPTY || parse_type == ParseType::LIST)
      || (parse_type == ParseType::MAP && key_valid_);

    if(can_insert_primitive) {
      return current_location;
    } else {
      throw std::runtime_error("Tried to insert object at an invalid location");
    }
  }
}


bool JSONSaxInterface::start_array(std::size_t) {
  Variant* active_location = get_insert_location();
  *active_location = Variant(VariantTypeEnum::LIST);
  state_stack_.emplace_back(active_location, ParseType::LIST);
  return true;
}

bool JSONSaxInterface::end_array() {
  state_stack_.pop_back();
  return true;
}

bool JSONSaxInterface::start_object(std::size_t) {
  Variant* active_location = get_insert_location();
  *active_location = Variant(VariantTypeEnum::MAP);
  state_stack_.emplace_back(active_location, ParseType::MAP);
  return true;
}

bool JSONSaxInterface::end_object() {
  state_stack_.pop_back();
  return true;
}

bool JSONSaxInterface::parse_error(std::size_t, const std::string&, const nlohmann::detail::exception& ex)
{
  throw ex;
}

template <typename JsonType, typename OutputType>
bool JSONSaxInterface::set_field(const JsonType& data) {
  Variant* active_location = get_insert_location();
  active_location->As<OutputType>() = static_cast<OutputType>(data);
  return true;
}

bool JSONSaxInterface::boolean(bool val) {
  return set_field<BoolType, BoolType>(val);
}

bool JSONSaxInterface::number_integer(json::number_integer_t val) {
  return set_field<json::number_integer_t, IntType>(val);
}

bool JSONSaxInterface::number_unsigned(json::number_unsigned_t val) {
  return set_field<json::number_unsigned_t, IntType>(val);
}

bool JSONSaxInterface::number_float(json::number_float_t val, const std::string&) {
  return set_field<json::number_float_t, FloatType>(val);
}

bool JSONSaxInterface::string(json::string_t& val) {
  return set_field<json::string_t, StringType>(val);
}

bool JSONSaxInterface::null() {
  throw std::runtime_error("Null Value Encountered");
}

bool JSONSaxInterface::binary(json::binary_t&) {
  throw std::runtime_error("Binary not supported");
}





}