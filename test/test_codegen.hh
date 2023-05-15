#pragma once 

#include <string>
#include <map>
#include <vector>

#include "variflect.hh"

namespace variflect{

class PrimitiveStruct : public Variflect
{
  int x;
  float z;
  bool w;
};

struct NestedStruct : public Variflect
{
  PrimitiveStruct primitive;
  std::map<std::string, PrimitiveStruct> map;
  std::vector<PrimitiveStruct> list;
};



}
