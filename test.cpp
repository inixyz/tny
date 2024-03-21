#include <iostream>
#include <any>
#include <vector>
#include <string>
#include <regex>

std::vector<std::string> split_string(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<std::string> lex(const std::string& in) {
    std::string in_modfied = std::regex_replace(in, std::regex("[(]"), " ( ");
    in_modfied = std::regex_replace(in_modfied, std::regex("[)]"), " ) ");

    return split_string(in_modfied);
}

std::any parse(std::vector<std::string>& toks,
    std::vector<std::any>& ast = *(new std::vector<std::any>)) {

    // wtf
    if (toks.empty()) {
        return ast.back();
    }

    // extract one token
    std::string tok = toks.front();
    toks.erase(toks.begin());

    if (tok == "(") {
        // wtf x2
        ast.push_back(parse(toks, *(new std::vector<std::any>)));
        return parse(toks, ast);
    } else if (tok == ")") {
        return ast;
    } else {
        ast.push_back(tok);
        return parse(toks, ast);
    }
}

void print_any(const std::any any) {
    if (any.type() == typeid(std::string))
        std::cout << "\"" << std::any_cast<std::string>(any) << "\"";
    else if (any.type() == typeid(std::vector<std::any>)) {
        std::vector<std::any> vec = std::any_cast<std::vector<std::any>>(any);
        std::cout << "[";
        for (size_t i = 0; i < vec.size(); i++) {
            print_any(vec[i]);
            if (i != vec.size() - 1) std::cout << ", ";
        }
        std::cout << "]";
    }
}

int main() {
    std::string in = "(+ (+ 5 (quote 10)) 2)";
    std::cout << in << std::endl;

    std::vector<std::string> toks = lex(in);
    for (size_t i = 0; i < toks.size(); i++) {
        std::cout << toks[i] << ", ";
    }
    std::cout << std::endl;

    print_any(parse(toks));
}
