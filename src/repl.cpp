#include <iostream>
#include <string>
#include <vector>

#include "tnyvec.h"

int main() {
  tnyvec::Env env;

  std::string in;
  while (true) {
    std::cout << ">>> ";
    std::getline(std::cin, in);
    std::vector<std::string> toks = tnyvec::lex(in);
    tnyvec::print(tnyvec::exec(tnyvec::parse(toks), env));
    std::cout << std::endl;
  };
}