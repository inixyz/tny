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
struct Fn {
    std::vector<std::string> params; Vec ast;
};
struct Data {
    enum {VEC, BUILTIN, FN, SYMBOL, NUM, STR} type;
    std::variant<Vec, Builtin, Fn, std::string, double> val;

    bool operator==(const Data& rhs) const {
        if (type != rhs.type) return false;

        switch(type) {
        case Data::BUILTIN:
            return std::get<Builtin>(val).target_type().name() ==
                std::get<Builtin>(rhs.val).target_type().name();

        case Data::FN: {
            const std::vector<std::string>& lhs_params =
                std::get<Fn>(val).params;
            const std::vector<std::string>& rhs_params =
                std::get<Fn>(rhs.val).params;
            const Vec& lhs_ast = std::get<Fn>(val).ast;
            const Vec& rhs_ast = std::get<Fn>(rhs.val).ast;
            if (lhs_params.size() != rhs_params.size() ||
                lhs_ast.size() != rhs_ast.size()) return false;
            else {
                for (size_t i = 0; i < lhs_params.size(); i++)
                    if (lhs_params[i] != rhs_params[i]) return false;
                for (size_t i = 0; i < lhs_ast.size(); i++)
                    if (!(lhs_ast[i] == rhs_ast[i])) return false;
                return true;
            }
        }

        case Data::SYMBOL: case Data::STR:
            return std::get<std::string>(val) ==
                std::get<std::string>(rhs.val);

        case Data::NUM:
            return std::get<double>(val) == std::get<double>(rhs.val);

        case Data::VEC: {
            const Vec& lhs_vec = std::get<Vec>(val);
            const Vec& rhs_vec = std::get<Vec>(rhs.val);
            if (lhs_vec.size() != rhs_vec.size()) return false;
            else {
                for (size_t i = 0; i < lhs_vec.size(); i++)
                    if (!(lhs_vec[i] == rhs_vec[i])) return false;
                return true;
            }
        } break;

        default:
            throw std::runtime_error("unknown data type in Data.operator==");
        }
    }

    operator bool() const {
        switch (type) {
        case Data::VEC: return !std::get<Vec>(val).empty();
        case Data::BUILTIN: return (bool)std::get<Builtin>(val);
        case Data::FN: return !std::get<Fn>(val).ast.empty();
        case Data::SYMBOL: case Data::STR:
            return !std::get<std::string>(val).empty();
        case Data::NUM: return std::get<double>(val);
        default:
            throw std::runtime_error("unknown data type in Data.operatorbool");
    }
}
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
        if (tok.front() == '"' && tok.back() != '"') {
            std::string str;
            std::getline(in_stream, str, '"');
            tok += str + '"';
        }
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

Data exec(const Vec& ast, Env& env);

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
        else if (fn.type == Data::FN) {
            const std::vector<std::string>& params =
                std::get<Fn>(fn.val).params;
            const Vec& ast = std::get<Fn>(fn.val).ast;

            if (params.size() != vec.size())
                throw std::runtime_error("invalid number of params in fn call");

            Scope scope;
            for (size_t i = 0; i < params.size(); i++)
                scope[params[i]] = eval(vec[i], env);

            env.local_scope.push(scope);
            const Data result = exec(ast, env);
            env.local_scope.pop();
            return result;
        } else throw std::runtime_error("unexpected data type in call");
    } break;
    default: throw std::runtime_error("unknown data type in eval");
    }
}

Data exec(const Vec& ast, Env& env) {
    if (ast.empty()) throw std::runtime_error("can't exec empty ast");
    Data result;
    for (const auto& expr : ast) result = eval(expr, env);
    return result;
}

void print(const Data& data, std::ostream& out = std::cout) {
    switch (data.type) {
    case Data::BUILTIN:
        out << "BUILTIN:" << std::get<Builtin>(data.val).target_type().name();
        break;

    case Data::FN: {
        const Fn& fn = std::get<Fn>(data.val);
        const std::vector<std::string>& params = fn.params;
        out << "FN(";
        for (auto it = params.begin(); it != params.end(); it++) {
            out << *it;
            if (it != params.end() - 1) out << " ";
        }
        out << ")(";
        for (const auto& expr : fn.ast) print(expr);
        out << ")";
    } break;

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

namespace builtin {

Data fn(const Vec& args, Env& env) {
    if (args.size() < 2) throw std::runtime_error("not enough args for fn");

    Fn result;

    Data params = args[0];
    if (params.type != Data::VEC)
        throw std::runtime_error("params for fn is not a vec");
    for (const auto& data : std::get<Vec>(params.val))
        if (data.type != Data::SYMBOL)
            throw std::runtime_error("param is not symbol");
        else result.params.push_back(std::get<std::string>(data.val));
    for (auto it = args.begin() + 1; it != args.end(); it++)
        result.ast.push_back(*it);

    return {Data::FN, result};
}

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

#define RELATIONAL_OPERATION(FN_NAME, OPERATION) \
Data FN_NAME(const Vec& args, Env& env) { \
    if (args.size() != 2) \
        throw std::runtime_error("invalid number of args for " \
            + std::string(#FN_NAME)); \
    const Data lhs = eval(args[0], env); \
    if (lhs.type != Data::NUM) \
        throw std::runtime_error("invalid lhs argument for " \
            + std::string(#FN_NAME)); \
    const double lhs_val = std::get<double>(lhs.val); \
    const Data rhs = eval(args[1], env); \
    if (rhs.type != Data::NUM) \
        throw std::runtime_error("invalid rhs argument for " \
            + std::string(#FN_NAME)); \
    const double rhs_val = std::get<double>(rhs.val); \
    return {Data::NUM, double(lhs_val OPERATION rhs_val)}; \
}

RELATIONAL_OPERATION(lt, <)
RELATIONAL_OPERATION(gt, >)
RELATIONAL_OPERATION(lteq, <=)
RELATIONAL_OPERATION(gteq, >=)

Data eq(const Vec& args, Env& env) {
    if (args.size() != 2)
        throw std::runtime_error("invalid number of args for eq");
    const Data lhs = eval(args[0], env);
    const Data rhs = eval(args[1], env);
    return {Data::NUM, double(lhs == rhs)};
}

#define LOGICAL_OPERATION(FN_NAME, OPERATION) \
Data FN_NAME(const Vec& args, Env& env) { \
    if (args.size() != 2) \
        throw std::runtime_error("invalid number of args for " \
            + std::string(#FN_NAME)); \
    const Data lhs = eval(args[0], env); \
    const Data rhs = eval(args[1], env); \
    return {Data::NUM, double(bool(lhs) OPERATION bool(rhs))}; \
}

LOGICAL_OPERATION(land, &&)
LOGICAL_OPERATION(lor, ||)

Data lnot(const Vec& args, Env& env) {
    if (args.size() != 1)
        throw std::runtime_error("invalid number of args for lnot");
    const Data rhs = eval(args[0], env);
    if (rhs.type != Data::NUM)
        std::runtime_error("invalid argument for lnot");
    return {Data::NUM, double(!bool(rhs))};
}

Data cond_if(const Vec& args, Env& env) {
    if (args.size() != 3)
        throw std::runtime_error("invalid number of args for cond_if +");

    Data condition = eval(args[0], env);
    if (condition) return eval(args[1], env);
    else return eval(args[2], env);
}

} // namespace builtin

Env::Env() {
    // arithmetic
    global_scope["+"] = {Data::BUILTIN, builtin::sum};
    global_scope["-"] = {Data::BUILTIN, builtin::sub};
    global_scope["*"] = {Data::BUILTIN, builtin::mul};
    global_scope["/"] = {Data::BUILTIN, builtin::div};

    // relational
    global_scope["<"] = {Data::BUILTIN, builtin::lt};
    global_scope[">"] = {Data::BUILTIN, builtin::gt};
    global_scope["<="] = {Data::BUILTIN, builtin::lteq};
    global_scope[">="] = {Data::BUILTIN, builtin::gteq};
    global_scope["=="] = {Data::BUILTIN, builtin::eq};

    // logical
    global_scope["&&"] = {Data::BUILTIN, builtin::land};
    global_scope["||"] = {Data::BUILTIN, builtin::lor};
    global_scope["!"] = {Data::BUILTIN, builtin::lnot};

    global_scope["if"] = {Data::BUILTIN, builtin::cond_if};

    global_scope["fn"] = {Data::BUILTIN, builtin::fn};
}

} // namespace tnyvec