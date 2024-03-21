#include <iostream>
#include <any>
#include <vector>
#include <string>
#include <regex>
#include <functional>
#include <list>
#include <cctype>

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
        if (isdigit(tok[0])) ast.push_back(std::stod(tok));
        else ast.push_back(tok);
        return parse(toks, ast);
    }
}

void print_any(const std::any any) {
    if (any.type() == typeid(std::string))
        std::cout << "\"" << std::any_cast<std::string>(any) << "\"";
    else if (any.type() == typeid(double))
        std::cout << std::any_cast<double>(any);
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

std::unordered_map<std::string, std::any> context;

std::any eval(const std::any any) {
    if (any.type() == typeid(std::string)) return std::any_cast<std::string>(any);
    else if (any.type() == typeid(double)) return std::any_cast<double>(any);
    else if (any.type() == typeid(std::vector<std::any>)) {
        std::vector<std::any> vec = std::any_cast<std::vector<std::any>>(any);

        std::string oper = std::any_cast<std::string>(vec[0]);
        vec.erase(vec.begin());

        // wtf 3
        return std::any_cast<std::function<double(const std::vector<std::any>)>>(context[oper])(vec);

        // if ( std::any_cast<std::string>(vec[0]) == "+") {
        //     double result = std::any_cast<double>(eval(vec[1]));
        //     for (size_t i = 2; i < vec.size(); i++)
        //         result += std::any_cast<double>(eval(vec[i]));
        //     return result;
        // }
    }
}


double plus(const std::vector<std::any> values) {
    double result = std::any_cast<double>(eval(values[0]));
    for (size_t i = 1; i < values.size(); i++) {
        result += std::any_cast<double>(eval(values[i]));
    }
    return result;
}

int main() {

    std::function<double(const std::vector<std::any>)> plus_func = plus;
    context["+"] = plus_func;


    std::string in = "(+ (+ 3 4) 2)";
    std::vector<std::string> toks = lex(in);
    print_any(eval(parse(toks)));
    std::cout << std::endl;
}
