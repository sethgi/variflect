#pragma once


#include <string>
#include <variant>


namespace variflect {

class Variant;

using StringType = std::string;
using FloatType = double;
using IntType = long;
using BoolType = bool;

using PrimitiveType = std::variant<StringType, FloatType, IntType, BoolType>;


using VariantList = std::vector<Variant>;
using VariantMap = std::map<StringType, Variant>;

// Boilerplate, i.e. from https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Types>
struct var_overloaded : Types... { using Types::operator()...; };

template<class... Ts>
var_overloaded(Ts...) -> var_overloaded<Ts...>;


template <typename T, typename... U>
concept SameAsOneOf = (std::same_as<T, U> || ...);

template <typename T>
concept variant_type = SameAsOneOf<T, IntType, StringType, FloatType, VariantList, VariantMap, BoolType>;

enum class VariantTypeEnum {
  EMPTY = 0,
  INT = 1,
  FLOAT = 2,
  STRING = 3,
  BOOL = 4,
  MAP = 5,
  LIST = 6
};

static const std::string VariantTypeNames[] = {
  "EMPTY",
  "INT", 
  "FLOAT",
  "STRING",
  "BOOL",
  "MAP",
  "LIST"
};


}


