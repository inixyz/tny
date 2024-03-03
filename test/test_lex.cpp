#include <iostream>
#include <list>

#include "lex.hpp"

int main() {
    std::list<Tok> toks = Lex("let str = \"boss\"").get_toks();
    for (auto it = toks.begin(); it != toks.end(); it++) {
         std::cout << it->type << " ";

        if (it->type == Tok::IDENT || it->type == Tok::STR) {
            std::cout << std::get<std::string>(it->literal);
        }
        std::cout << std::endl;
    }
}