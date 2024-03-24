#pragma once

#include <unordered_map>
#include <string>
#include <stack>
#include <vector>
#include <functional>
#include <variant>
#include <sstream>
#include <stdexcept>

namespace tnyvec {

struct Data;

using Scope = std::unordered_map<std::string, Data>;
struct Env {
    Scope global_scope; std::stack<Scope> local_scope;
    Env();
};

using Vec = std::vector<Data>;
using Builtin = std::function<Data(const Vec& args, Env& env)>;
struct Data {
    enum {VEC, BUILTIN, SYMBOL, NUM, STR} type;
    std::variant<Vec, Builtin, std::string, double> val;
};

std::vector<std::string> lex(const std::string& in) {
    std::string in_modified = in;

    auto replace = [](std::string& in, const std::string& pattern,
        const std::string& result) {

        size_t pos = 0;
        while ((pos = in.find(pattern, pos)) != std::string::npos) {
            in.replace(pos, pattern.size(), result);
            pos += result.size();
        }
    };

    replace(in_modified, "(", " ( ");
    replace(in_modified, ")", " ) ");

    std::istringstream in_stream(in_modified);
    std::string tok; std::vector<std::string> toks;

    while (in_stream >> tok) {
        if (tok.empty()) continue;
        toks.push_back(tok);
    }
    return toks;
}

Vec parse(std::vector<std::string>& toks, Vec ast = Vec()) {
    if (toks.empty()) return ast;
    std::string tok = toks.front(); toks.erase(toks.begin());

    if (tok == "(") {
        ast.push_back({Data::VEC, parse(toks, Vec())});
        return parse(toks, ast);
    } else if (tok == ")") return ast;
    else {
        double num;
        try {num = std::stod(tok);}
        catch (...) {
            if (tok.front() == '"' && tok.back() == '"') {
                tok.erase(tok.begin()); tok.pop_back();
                ast.push_back({Data::STR, tok});
            } else ast.push_back({Data::SYMBOL, tok});
            return parse(toks, ast);
        }
        ast.push_back({Data::NUM, num});
        return parse(toks, ast);
    }
}

Data eval(const Data& data, Env& env) {
    switch (data.type) {
    case Data::BUILTIN: case Data::NUM: case Data::STR: return data; break;
    case Data::SYMBOL: {
        const std::string ident = std::get<std::string>(data.val);

        if (!env.local_scope.empty() &&
            env.local_scope.top().find(ident) != env.local_scope.top().end()) {

            return env.local_scope.top().at(ident);
        } else if (env.global_scope.find(ident) != env.global_scope.end())
            return env.global_scope.at(ident);
        else throw std::runtime_error("undefined symbol " + ident);
    } break;
    case Data::VEC: {
        auto vec = std::get<Vec>(data.val);
        if (vec.empty()) throw std::runtime_error("can't call empty vector");

        Data fn = eval(vec[0], env); vec.erase(vec.begin());
        if (fn.type == Data::BUILTIN)
            return std::get<Builtin>(fn.val)(vec, env);
        // todo else if user function
        else throw std::runtime_error("unexpected data type in call");
    } break;
    default: throw std::runtime_error("unknown data type in eval");
    }
}

Data exec(const Vec& ast, Env& env) {
    if (ast.empty()) throw std::runtime_error("can't run empty ast");
    Data result;
    for (const auto& expr : ast) result = eval(expr, env);
    return result;
}

void print(const Data& data, std::ostream& out = std::cout) {
    switch (data.type) {
    case Data::BUILTIN:
        out << "BUILTIN:" << std::get<Builtin>(data.val).target_type().name();
        break;
    case Data::SYMBOL: out << std::get<std::string>(data.val); break;
    case Data::NUM: out << std::get<double>(data.val); break;
    case Data::STR: out << '"' << std::get<std::string>(data.val) << '"'; break;
    case Data::VEC: {
        const auto& vec = std::get<Vec>(data.val);
        out << "(";
        for (auto it = vec.begin(); it != vec.end(); it++) {
            print(*it);
            if (it != vec.end() - 1) out << " ";
        }
        out << ")";
    } break;
    default: throw std::runtime_error("unknown data type in print");
    }
}

bool is_true(const Data& data) {
    switch (data.type) {
    case Data::VEC: return !std::get<Vec>(data.val).empty();
    case Data::BUILTIN: return (bool)std::get<Builtin>(data.val);
    case Data::SYMBOL: case Data::STR:
        return !std::get<std::string>(data.val).empty();
    case Data::NUM: return std::get<double>(data.val);
    default: throw std::runtime_error("unknown data type in is_true");
    }
}

namespace builtin {

#define ARITHMETIC_OPERATION(FN_NAME, OPERATION) \
Data FN_NAME(const Vec& args, Env& env) { \
    if (args.size() < 2) \
        throw std::runtime_error("not enough args for " \
            + std::string(#FN_NAME)); \
    Data data = eval(args[0], env); \
    if (data.type != Data::NUM) \
        throw std::runtime_error("invalid argument for " \
            + std::string(#FN_NAME)); \
    double result = std::get<double>(data.val); \
    for (auto it = args.begin() + 1; it != args.end(); it++) { \
        if ((data = eval(*it, env)).type != Data::NUM) \
            throw std::runtime_error("invalid argument for " \
                + std::string(#FN_NAME)); \
        result OPERATION std::get<double>(data.val); \
    } \
    return {Data::NUM, result}; \
}

ARITHMETIC_OPERATION(sum, +=)
ARITHMETIC_OPERATION(sub, -=)
ARITHMETIC_OPERATION(mul, *=)
ARITHMETIC_OPERATION(div, /=)

#define LOGICAL_OPERATION(FN_NAME, OPERATION) \
Data FN_NAME(const Vec& args, Env& env) { \
    if (args.size() != 2) \
        throw std::runtime_error("invalid number of args for " \
            + std::string(#FN_NAME)); \
    const Data left = eval(args[0], env); \
    if (left.type != Data::NUM) \
        throw std::runtime_error("invalid left argument for " \
            + std::string(#FN_NAME)); \
    const double left_val = std::get<double>(left.val); \
    const Data right = eval(args[1], env); \
    if (right.type != Data::NUM) \
        throw std::runtime_error("invalid right argument for " \
            + std::string(#FN_NAME)); \
    const double right_val = std::get<double>(right.val); \
    return {Data::NUM, double(left_val OPERATION right_val)}; \
}

LOGICAL_OPERATION(lt, <)
LOGICAL_OPERATION(gt, >)
LOGICAL_OPERATION(lteq, <=)
LOGICAL_OPERATION(gteq, >=)
LOGICAL_OPERATION(land, &&)
LOGICAL_OPERATION(lor, ||)

Data cond_if(const Vec& args, Env& env) {
    if (args.size() != 3)
        throw std::runtime_error("invalid number of args for cond_if +");

    Data condition = eval(args[0], env);
    if (is_true(condition)) return eval(args[1], env);
    else return eval(args[2], env);
}

} // namespace builtin

Env::Env() {
    global_scope["+"] = {Data::BUILTIN, builtin::sum};
    global_scope["-"] = {Data::BUILTIN, builtin::sub};
    global_scope["*"] = {Data::BUILTIN, builtin::mul};
    global_scope["/"] = {Data::BUILTIN, builtin::div};

    global_scope["<"] = {Data::BUILTIN, builtin::lt};
    global_scope[">"] = {Data::BUILTIN, builtin::gt};
    global_scope["<="] = {Data::BUILTIN, builtin::lteq};
    global_scope[">="] = {Data::BUILTIN, builtin::gteq};
    global_scope["&&"] = {Data::BUILTIN, builtin::land};
    global_scope["||"] = {Data::BUILTIN, builtin::lor};

    global_scope["if"] = {Data::BUILTIN, builtin::cond_if};
}

} // namespace tnyvec