#include "../include/variflect.hh"

#include <stdexcept>

namespace variflect {

const char* const kCommandLineArgs[] = {"-std=c++20", "-I../include/", "-I/usr/include/c++/12"};
constexpr int kNumCommandLineArgs = 3;

VariflectGen::VariflectGen(const std::string& source)
  : sources_{source}
{}


VariflectGen::VariflectGen(const std::vector<std::string>& sources)
  : sources_(sources)
{}

void VariflectGen::GenerateCode() {
  TypeMap types = GetTypes();
}

VariflectGen::TypeMap VariflectGen::GetTypes() {
  TypeMap result;

  for(const std::string& source : sources_) {
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
      index, source.c_str(), kCommandLineArgs, kNumCommandLineArgs,
      nullptr, 0, CXTranslationUnit_None);

    if(unit == nullptr) {
      throw std::runtime_error("Can't process translation unit");
    }
  }

  return result;
}


}

int main() {}