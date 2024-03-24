#include <string>
#include <iostream>
#include <vector>

#include "tnyvec.hpp"

int main() {
    tnyvec::Env env;

    std::string in;
    while (true) {
        std::cout << ">>> "; std::getline(std::cin, in);
        std::vector<std::string> toks = tnyvec::lex(in);
        tnyvec::print(tnyvec::run(tnyvec::parse(toks), env));
        std::cout << std::endl;
    };
}