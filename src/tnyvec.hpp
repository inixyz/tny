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

Data run(const Vec& ast, Env& env) {
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

namespace builtin {

Data sum(const Vec& args, Env& env) {
    if (args.size() < 2) throw std::runtime_error("not enough args for +");

    Data data = eval(args[0], env);
    if (data.type != Data::NUM)
        throw std::runtime_error("invalid argument for +");
    double result = std::get<double>(data.val);

    for (auto it = args.begin() + 1; it != args.end(); it++) {
        if ((data = eval(*it, env)).type != Data::NUM)
            throw std::runtime_error("invalid argument for +");
        result += std::get<double>(data.val);
    }

    return {Data::NUM, result};
}

} // namespace builtin

Env::Env() {
    global_scope["+"] = {Data::BUILTIN, builtin::sum};
}

} // namespace tnyvec