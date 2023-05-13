#include <variant.hh>
#include <iostream>

using namespace variflect;

int main() {
  Variant v;

  VariantMap& vmap = v.AsMap();

  vmap.emplace("3.0", 2);
  vmap.emplace("2", "hi");
  vmap.emplace("hello", 1000.);
  vmap.emplace("list", Variant());

  VariantList& vlist = vmap.at("list").AsList();
  vlist.emplace_back(3.0);
  vlist.emplace_back("hello");

  vlist.push_back(Variant());
  VariantMap& vmap2 = vlist.back().AsMap();
  
  vmap2.emplace("pi", 3.14);

  const std::string data = v.ToJSON();
  std::cout << data << std::endl;

  
  Variant converted = Variant::FromJSONString(data);
  std::cout << " === After === " << std::endl;
  std::cout << converted.ToJSON() << std::endl;

}
