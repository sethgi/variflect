#pragma once

#include <vector>
#include <map>
#include <stdexcept>

#include "types.hh"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace variflect {


class Variant {

 public:
  Variant();
  Variant(const VariantTypeEnum& target_type);
  Variant(const PrimitiveType& primitive_val);
  Variant(const VariantList& list);
  Variant(const VariantMap& map);

  static Variant FromJSONString(const std::string& data);
  static Variant FromJSONFile(const std::string& filename);

  template <variant_type T>
  T& As();

  VariantMap& AsMap();

  VariantList& AsList();

  IntType& AsInt();

  FloatType& AsFloat();

  StringType& AsString();

  BoolType& AsBool();

  inline const std::string& GetTypeName() const {return VariantTypeNames[static_cast<int>(type_)];};

  inline const VariantTypeEnum GetType() const {return type_;};

  const std::string ToJSON() const;
  bool ToJSON(const std::string& filename) const;


 private:

  
  // TODO: Can this be a union? Technically a bad implementation could set many of these.
  // Will ignore for now for simplicity.
  struct DataType {
    PrimitiveType value;
    VariantList list;
    VariantMap map;
  } data_;

  VariantTypeEnum type_;

  const std::string FormatError(const VariantTypeEnum& expected) const;
  const std::string FormatError(const VariantTypeEnum& expected, const VariantTypeEnum& found) const;

  template <variant_type T>
  static VariantTypeEnum constexpr GetEnumByType();

  // Gets rid of everything that isn't active type
  void Cleanup();

  json ToJSONHelper(const Variant& variant) const;
};

// TODO: Define a concept for T
template <typename T>
Variant ToVariant(const T&);


template <typename T>
T FromVariant(const Variant&);


}