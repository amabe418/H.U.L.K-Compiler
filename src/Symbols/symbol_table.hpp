// symbol_table.hpp
#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Types/type_info.hpp"
#include "symbol.hpp"

class SymbolTable
{
   public:
    static SymbolTable&
    getInstance()
    {
        static SymbolTable instance;
        return instance;
    }

    SymbolTable() : currentScope(0)
    {
        // Initialize the global scope
        scopes.push_back(std::unordered_map<std::string, Symbol>());
    }
    ~SymbolTable() = default;

    void
    enterScope()
    {
        std::cout << "Entro al scope desde" << std::endl;
        for (auto& [name, symbol] : scopes[currentScope])
        {
            std::cout << name << " " << symbol.name << std::endl;
        }
        currentScope++;
        scopes.push_back(std::unordered_map<std::string, Symbol>());
    }

    void
    exitScope()
    {
        if (currentScope > 0)
        {
            currentScope--;
        }
    }

    void
    define(const std::string& name, const Symbol& symbol)
    {
        if (currentScope >= 0 && currentScope < scopes.size())
        {
            scopes[currentScope][name] = symbol;
        }
    }

    std::optional<Symbol>
    lookup(const std::string& name) const
    {
        // Search from current scope up to global scope
        for (int i = currentScope; i >= 0; i--)
        {
            auto found = scopes[i].find(name);
            if (found != scopes[i].end())
            {
                return found->second;
            }
        }
        return std::nullopt;
    }

    std::optional<Symbol>
    lookupInCurrentScope(const std::string& name) const
    {
        if (currentScope >= 0 && currentScope < scopes.size())
        {
            auto found = scopes[currentScope].find(name);
            if (found != scopes[currentScope].end())
            {
                return found->second;
            }
        }
        return std::nullopt;
    }

    void
    update(const std::string& name, const Symbol& symbol)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto found = it->find(name);
            if (found != it->end())
            {
                found->second = symbol;
                return;
            }
        }
    }

    bool
    insert(const std::string& name, const Symbol& symbol)
    {
        if (scopes.empty())
        {
            return false;
        }
        auto& currentScope = scopes.back();
        if (currentScope.find(name) != currentScope.end())
        {
            return false;  // Símbolo ya existe en este scope
        }
        currentScope[name] = symbol;
        return true;
    }

    // Método para imprimir la tabla de símbolos
    void
    print() const
    {
        std::cout << "\nCurrent Scope: " << currentScope << std::endl;
        std::cout << "scopes size: " << scopes.size() << std::endl;

        // Print all scopes
        for (size_t i = 0; i < scopes.size(); i++)
        {
            std::cout << "\nScope " << i << ":" << std::endl;
            std::cout << "Symbols:" << std::endl;

            const auto& scope = scopes[i];
            if (!scope.empty())
            {
                for (const auto& [name, symbol] : scope)
                {
                    std::cout << "  " << name << " (" << symbolKindToString(symbol.kind) << ")";
                    if (symbol.type)
                    {
                        std::cout << " : " << symbol.type->toString();
                    }
                    std::cout << std::endl;
                }
            }
            else
            {
                std::cout << "  No symbols in this scope" << std::endl;
            }
        }
    }

    int
    getCurrentScope() const
    {
        return currentScope;
    }

   private:
    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    int currentScope;

    std::string
    symbolKindToString(SymbolKind kind) const
    {
        switch (kind)
        {
            case SymbolKind::Variable:
                return "Variable";
            case SymbolKind::Function:
                return "Function";
            case SymbolKind::Parameter:
                return "Parameter";
            case SymbolKind::Type:
                return "Type";
            case SymbolKind::TypeParameter:
                return "TypeParameter";
            case SymbolKind::Method:
                return "Method";
            case SymbolKind::Attribute:
                return "Attribute";
            default:
                return "Unknown";
        }
    }
};
