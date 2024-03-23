#include <iostream>
#include <any>
#include <vector>
#include <string>
#include <regex>
#include <functional>
#include <list>
#include <cctype>
#include <forward_list>

std::vector<std::any> lex(std::string& in) {
    in = std::regex_replace(in, std::regex("[(]"), " ( ");
    in = std::regex_replace(in, std::regex("[)]"), " ) ");

    std::istringstream in_stream(in);
    std::string tok; std::vector<std::any> toks;
    while (in_stream >> tok) {
        if (tok.empty()) continue;
        if (std::isdigit(tok[0])) toks.push_back(std::stod(tok));
        else toks.push_back(tok);
    }

    return toks;
}

std::any parse(std::vector<std::any>& toks,
    std::vector<std::any> ast = std::vector<std::any>()) {

    if (toks.empty()) return ast.back();
    std::any tok = toks.front();
    toks.erase(toks.begin());

    if (tok.type() == typeid(std::string)) {
        std::string tok_string = std::any_cast<std::string>(tok);
        if (tok_string == "(") {
            ast.push_back(parse(toks, std::vector<std::any>()));
            return parse(toks, ast);
        } else if (tok_string == ")") return ast;
        else {
            ast.push_back(tok);
            return parse(toks, ast);
        }
    } else if (tok.type() == typeid(double)) {
        ast.push_back(tok);
        return parse(toks, ast);
    } else {
        std::cerr << "Invalid tok encountered while parsing" << std::endl;
        std::terminate();
    }
}

void print_any(const std::any& any) {
    if (any.type() == typeid(double))
        std::cout << std::any_cast<double>(any);
    else if (any.type() == typeid(std::string))
        std::cout << std::any_cast<std::string>(any);
    else if (any.type() == typeid(std::vector<std::any>)) {
        const auto& vec = std::any_cast<std::vector<std::any>>(any);
        std::cout << "(";
        for (size_t i = 0; i < vec.size(); i++) {
            print_any(vec[i]);
            if (i != vec.size() - 1) std::cout << " ";
        }
        std::cout << ")";

    } else {
        std::cerr << "Invalid value encountered while printing" << std::endl;
        std::terminate();
    }
}

std::unordered_map<std::string, std::any> global_scope;

std::any eval(const std::any any) {
    if (any.type() == typeid(double))
        return std::any_cast<double>(any);
    else if (any.type() == typeid(std::string)) {
        std::string any_str = std::any_cast<std::string>(any);
        if (any_str.front() == '"' && any_str.back() == '"') return any_str;

        if (global_scope.find(any_str) == global_scope.end()) {
            std::cerr << "Invalid symbol: " + any_str << std::endl;
            std::terminate();
        }

        return global_scope.at(any_str);
    }
    else if (any.type() == typeid(std::vector<std::any>)) {
        auto vec = std::any_cast<std::vector<std::any>>(any);

        if (vec.empty()) {
            std::cerr << "Cannot eval empty list" << std::endl;
            std::terminate();
        }

        auto func = std::any_cast<std::function<std::any(const std::vector<std::any>)>>(eval(vec[0]));
        vec.erase(vec.begin());

        return func(vec);
    }
    else {
        std::cerr << "Invalid value encountered while evaluating" << std::endl;
        std::terminate();
    }
}

std::any func_sum(const std::vector<std::any> params) {
    if (params.size() < 2) {
        std::cerr << "Not enougth parameters for sum" << std::endl;
        std::terminate();
    }

    double result = std::any_cast<double>(eval(params[0]));
    for (size_t i = 1; i < params.size(); i++) {
        result += std::any_cast<double>(eval(params[i]));
    }
    return result;
}

int main() {
    std::function<std::any(const std::vector<std::any>)> func_func_sum = func_sum;
    global_scope["+"] = func_func_sum;
    global_scope["x"] = double(5);

    std::cout << sizeof(std::forward_list<int>) << std::endl;


    std::string in;
    std::vector<std::any> toks;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, in);
        toks = lex(in);
        print_any(eval(parse(toks))); std::cout << std::endl;
    }
}
