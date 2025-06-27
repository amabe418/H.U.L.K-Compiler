#pragma once
#include <memory>
#include "regular_expressions.hpp"
using REPtr = std::shared_ptr<RE>;

class RegexParser {
    const std::string input;
    size_t pos = 0;

public:
    RegexParser(const std::string& pat): input(pat) {}

    REPtr parse() {
        auto r = parse_union();
        if (pos != input.size())  return nullptr;//throw std::runtime_error("Unexpected char at end");
        return r;
    }

private:
    // jerarquía: union (lowest), concat, closure (+ * ?), atom
    REPtr parse_union();
    REPtr parse_concat();
    REPtr parse_closure();
    REPtr parse_atom();
    
    char peek() const { return pos < input.size() ? input[pos] : '\0'; }
    char get() { return pos < input.size() ? input[pos++] : '\0'; }
    bool eat(char c) { if (peek()==c) { pos++; return true; } return false; }
};