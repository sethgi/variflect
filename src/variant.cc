#include "variant.hh"

#include <iostream>
#include <fstream>

#include "json_parser.hh"

namespace variflect {

Variant::Variant() : type_(VariantTypeEnum::EMPTY) {}

Variant::Variant(const VariantTypeEnum& type): type_(type) {};

Variant::Variant(const PrimitiveType& primitive_val) {
  std::visit(var_overloaded{
    [this](const IntType& int_val) {
      data_.value = int_val;
      type_ = VariantTypeEnum::INT;
    },
    [this](const FloatType& float_val) {
      data_.value = float_val;
      type_ = VariantTypeEnum::FLOAT;
    },
    [this](const StringType& str_val) {
      data_.value = str_val;
      type_ = VariantTypeEnum::STRING;
    },
    [this](const BoolType& bool_val) {
      data_.value = bool_val;
      type_ = VariantTypeEnum::BOOL;
    }
  }, primitive_val);
}

Variant Variant::FromJSONString(const std::string& data) {
  Variant result;
  JSONSaxInterface sax_parser(&result);
  json::sax_parse(data, &sax_parser);
  return result;
}

Variant Variant::FromJSONFile(const std::string& fname) {
  Variant result;

  std::ifstream input_file(fname);
  
  if(!input_file.is_open()) {
    throw std::runtime_error("Failed to open file " + fname);
  }
  std::string data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

  JSONSaxInterface sax_parser(&result);
  json::sax_parse(data, &sax_parser);
  return result;
}

Variant::Variant(const VariantList& list)
  : type_(VariantTypeEnum::LIST)
{
  data_.list = list;
}


Variant::Variant(const VariantMap& map)
  : type_(VariantTypeEnum::MAP)
{
  data_.map = map;
}

json Variant::ToJSONHelper(const Variant& variant) const {
  if(variant.type_ == VariantTypeEnum::STRING) {
    return json(std::get<StringType>(variant.data_.value));
  } else if(variant.type_ == VariantTypeEnum::FLOAT) {
    return json(std::get<FloatType>(variant.data_.value));
  } else if(variant.type_ == VariantTypeEnum::INT) {
    return json(std::get<IntType>(variant.data_.value));
  } else if(variant.type_ == VariantTypeEnum::LIST) {
    std::vector<json> entries;
    std::transform(variant.data_.list.cbegin(), variant.data_.list.cend(), std::back_inserter(entries),
      [&](const Variant& v){
        json entry = ToJSONHelper(v);
        return entry;
      });
    return json(entries);
  } else if(variant.type_ == VariantTypeEnum::MAP) {
    json local_map;
    for(const auto& [key, value] : variant.data_.map) {
      local_map[key] = ToJSONHelper(value);
    }
    return local_map;
  } else {
    throw std::runtime_error("Invalid type in enum");
  } 
}

const std::string Variant::ToJSON() const {
  json data = ToJSONHelper(*this);
  return data.dump(2);
}

bool Variant::ToJSON(const std::string& filename) const {
  const std::string& json_data = ToJSON();

  std::ofstream out_file(filename);

  if(!out_file.is_open()) {
    return false;
  }

  out_file << json_data;

  return true;
}


const std::string Variant::FormatError(const VariantTypeEnum& expected) const {
  return FormatError(expected, type_);
};

template <variant_type T>
VariantTypeEnum constexpr Variant::GetEnumByType()
{
  if constexpr(std::is_same_v<T, IntType>) {
    return VariantTypeEnum::INT;
  } else if constexpr(std::is_same_v<T, FloatType>) {
    return VariantTypeEnum::FLOAT;
  } else if constexpr(std::is_same_v<T, StringType>) {
    return VariantTypeEnum::STRING;
  } else if constexpr(std::is_same_v<T, BoolType>) {
    return VariantTypeEnum::BOOL;
  } else if constexpr(std::is_same_v<T, VariantList>) {
    return VariantTypeEnum::LIST;
  } else if constexpr(std::is_same_v<T, VariantMap>) {
    return VariantTypeEnum::MAP;
  } else {
    static_assert(variant_type<T>);
  }
}

const std::string Variant::FormatError(const VariantTypeEnum& expected, const VariantTypeEnum& found) const {
  std::string result = "Tried to fetch ";
  result += VariantTypeNames[static_cast<int>(expected)];
  result += ", but Variant contains ";
  result += VariantTypeNames[static_cast<int>(found)];
  return result;
};


template <>
VariantList& Variant::As<VariantList>() {
  if (type_ == VariantTypeEnum::EMPTY) {
    type_ = VariantTypeEnum::LIST;
    return data_.list;
  };
  if(type_ != VariantTypeEnum::LIST) {
    throw std::runtime_error(FormatError(VariantTypeEnum::LIST));
  }
  
  return data_.list;
} 
  
template <>
VariantMap& Variant::As<VariantMap>() { 
  if(type_ == VariantTypeEnum::EMPTY) {
    type_ = VariantTypeEnum::MAP;
    return data_.map;
  };
  if(type_ != VariantTypeEnum::MAP) {
    throw std::runtime_error(FormatError(VariantTypeEnum::MAP));
  }
  return data_.map;
}

void Variant::Cleanup() {
  if(type_ == VariantTypeEnum::EMPTY) {
    data_.map.clear();
    data_.list.clear();
    data_.value = PrimitiveType();
  } else if (type_ == VariantTypeEnum::LIST) {
    data_.map.clear();
    data_.value = PrimitiveType();
  } else if (type_ == VariantTypeEnum::MAP) {
    data_.list.clear();
    data_.value = PrimitiveType();
  } else {
    data_.list.clear();
    data_.map.clear();
  }
}

template <variant_type T>
T& Variant::As() {
  const VariantTypeEnum enum_type = GetEnumByType<T>();
  if(type_ == VariantTypeEnum::EMPTY) {
    type_ = enum_type;
    data_.value = T();
    return std::get<T>(data_.value);
  };

  if(type_ != enum_type) {
    throw std::runtime_error(FormatError(enum_type));
  }
  return std::get<T>(data_.value);
}

StringType& Variant::AsString() {
  return As<StringType>();
}

FloatType& Variant::AsFloat() {
  return As<FloatType>();
}

IntType& Variant::AsInt() {
  return As<IntType>();
}

BoolType& Variant::AsBool() {
  return As<BoolType>();
}

VariantList& Variant::AsList() {
  return As<VariantList>();
}

VariantMap& Variant::AsMap() {
  return As<VariantMap>();
}



};