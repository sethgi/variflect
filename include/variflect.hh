#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <clang-c/Index.h>

namespace variflect {

struct Variflect {};

class VariflectGen {
 public:
  VariflectGen() = default;
  VariflectGen(const std::string& source);
  VariflectGen(const std::vector<std::string>& sources);

  void GenerateCode();

 private:
  using TypeMap = std::unordered_map<std::string, CXCursor>;
  
  std::vector<std::string> sources_;

  TypeMap GetTypes();
};


}