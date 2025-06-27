#pragma once
#include <memory>
#include <set>
#include <string>
#include "Token.hpp"
#include "NFA.hpp"
enum class REKind {
    Empty, Epsilon, Symbol, Union, Concat, Clousure,Range,Any
};

class RE {
public:
    TokenType type;
    std::set<char> alphabet;

    RE(std::set<char> alphabet) : alphabet(std::move(alphabet)) {}
    virtual ~RE() = default;

    virtual bool is_valid(const std::string& inp) const = 0;
    virtual NFA ConvertToNFA() const = 0;
};

using REPtr = std::shared_ptr<RE>;

class EmptyRE : public RE {
public:
    EmptyRE() : RE({}) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::emptyRE();
    }
};

class EpsilonRE : public RE {
public:
    EpsilonRE() : RE({'$'}) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::epsilonRE();
    }
};

class AnyRE : public RE {
public:
    AnyRE() : RE({'~'}) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::anyRE();
    }
};


class SymbolRE : public RE {
    char symbol;
public:
    SymbolRE(char symbol) : RE({symbol}), symbol(symbol) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::symbolRE(symbol);
    }
};

class UnionRE : public RE {
    REPtr left, right;
public:
    UnionRE(REPtr l, REPtr r) : RE(l->alphabet) {
        left = std::move(l);
        right = std::move(r);
        alphabet.insert(right->alphabet.begin(), right->alphabet.end());
    }
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::UnionRE(left->ConvertToNFA(), right->ConvertToNFA());
    }
};

class ConcatenationRE : public RE {
    REPtr left, right;
public:
    ConcatenationRE(REPtr l, REPtr r) : RE(l->alphabet) {
        left = std::move(l);
        right = std::move(r);
        alphabet.insert(right->alphabet.begin(), right->alphabet.end());
    }
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::ConcatenationRE(left->ConvertToNFA(), right->ConvertToNFA());
    }
};

class ClousureRE : public RE {
    REPtr value;
public:
    ClousureRE(REPtr val) : RE(val->alphabet), value(std::move(val)) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::ClousureRE(value->ConvertToNFA());
    }
};


class RangeRE : public RE {
    char from;
    char to;


public:
    RangeRE(char from, char to) : RE({}), from(from), to(to) {
        for (char c = from; c <= to; ++c)
            alphabet.insert(c);
    }

    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evaluate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::RangeRE(from,to);
    }
};
