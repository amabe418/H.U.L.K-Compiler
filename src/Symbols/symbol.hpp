// symbol.hpp
#pragma once

#include <memory>
#include <string>
#include <variant>

#include "../Types/type_info.hpp"

enum class SymbolKind
{
    Variable,
    Function,
    Type,
    Method,
    TypeParameter,
    Attribute,
    Parameter
};

struct Symbol
{
    std::string name;
    SymbolKind kind;
    std::shared_ptr<TypeInfo> type;
    bool isMutable;

    Symbol() : isMutable(false) {}
    Symbol(const std::string& n, SymbolKind k, std::shared_ptr<TypeInfo> t = nullptr,
           bool m = false)
        : name(n), kind(k), type(t), isMutable(m)
    {
    }
};
