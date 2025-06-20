#pragma once
#include "../Types/type_info.hpp"
#include "../AST/ast.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
/**
 * @brief Symbol information for variables and functions
 */
struct Symbol
{
    std::string name;
    TypeInfo type;
    bool is_mutable;
    int declaration_line;

    Symbol() : name(""), type(TypeInfo::Kind::Unknown), is_mutable(true), declaration_line(0) {}

    Symbol(const std::string &n, const TypeInfo &t, bool mut = true, int line = 0)
        : name(n), type(t), is_mutable(mut), declaration_line(line) {}
};

/**
 * @brief Function signature information
 */
struct FunctionSymbol
{
    std::string name;
    std::vector<TypeInfo> parameter_types;
    TypeInfo return_type;
    int declaration_line;

    FunctionSymbol(const std::string &n, const std::vector<TypeInfo> &params,
                   const TypeInfo &ret, int line = 0)
        : name(n), parameter_types(params), return_type(ret), declaration_line(line) {}
};

/**
 * @brief Type information including attributes and methods
 */
struct TypeSymbol
{
    std::string name;
    std::string base_type;
    std::map<std::string, Symbol> attributes;
    std::map<std::string, std::shared_ptr<FunctionSymbol>> methods;
    int declaration_line;

    TypeSymbol(const std::string &n, const std::string &base = "Object", int line = 0)
        : name(n), base_type(base), declaration_line(line) {}
};

/**
 * @brief Symbol table for managing variable and function scopes
 */
class SymbolTable
{
private:
    std::vector<std::map<std::string, Symbol>> variable_scopes_;
    std::map<std::string, std::shared_ptr<FunctionSymbol>> functions_;
    std::map<std::string, std::shared_ptr<TypeSymbol>> types_; // Track declared types with full info
    std::map<std::string, TypeDecl *> type_declarations_;      // Store type declarations for parameter checking

public:
    SymbolTable()
    {
        // Create global scope
        pushScope();

        // Add built-in functions
        addBuiltinFunctions();
    }

    /**
     * @brief Enter a new scope
     */
    void pushScope()
    {
        variable_scopes_.emplace_back();
    }

    /**
     * @brief Exit current scope
     */
    void popScope()
    {
        if (!variable_scopes_.empty())
        {
            variable_scopes_.pop_back();
        }
    }

    /**
     * @brief Enter a new scope (alias for pushScope for compatibility)
     */
    void enterScope()
    {
        pushScope();
    }

    /**
     * @brief Exit current scope (alias for popScope for compatibility)
     */
    void exitScope()
    {
        popScope();
    }

    /**
     * @brief Declare a variable in current scope
     */
    bool declareVariable(const std::string &name, const TypeInfo &type,
                         bool is_mutable = true, int line = 0)
    {
        if (variable_scopes_.empty())
        {
            return false;
        }

        auto &current_scope = variable_scopes_.back();

        // Check if variable already exists in current scope
        if (current_scope.find(name) != current_scope.end())
        {
            return false; // Variable already declared
        }

        std::cout << "[DEBUG] SymbolTable::declareVariable - storing variable " << name
                  << " with type: " << type.toString()
                  << " (typeName: '" << type.getTypeName() << "')" << std::endl;

        current_scope[name] = Symbol(name, type, is_mutable, line);

        // Verify what was actually stored
        auto &stored_symbol = current_scope[name];
        std::cout << "[DEBUG] SymbolTable::declareVariable - stored symbol has type: "
                  << stored_symbol.type.toString()
                  << " (typeName: '" << stored_symbol.type.getTypeName() << "')" << std::endl;

        return true;
    }

    /**
     * @brief Look up a variable in any scope (from current to global)
     */
    Symbol *lookupVariable(const std::string &name)
    {
        for (auto it = variable_scopes_.rbegin(); it != variable_scopes_.rend(); ++it)
        {
            auto found = it->find(name);
            if (found != it->end())
            {
                std::cout << "[DEBUG] SymbolTable::lookupVariable - found variable " << name
                          << " with type: " << found->second.type.toString()
                          << " (typeName: '" << found->second.type.getTypeName() << "')" << std::endl;
                return &found->second;
            }
        }
        return nullptr;
    }

    /**
     * @brief Declare a function
     */
    bool declareFunction(const std::string &name, const std::vector<TypeInfo> &params,
                         const TypeInfo &return_type, int line = 0)
    {
        // Check if function already exists
        if (functions_.find(name) != functions_.end())
        {
            return false;
        }

        functions_[name] = std::make_shared<FunctionSymbol>(name, params, return_type, line);
        return true;
    }

    /**
     * @brief Look up a function
     */
    std::shared_ptr<FunctionSymbol> lookupFunction(const std::string &name)
    {
        auto found = functions_.find(name);
        return (found != functions_.end()) ? found->second : nullptr;
    }

    /**
     * @brief Check if variable exists in current scope only
     */
    bool hasLocalVariable(const std::string &name) const
    {
        if (variable_scopes_.empty())
        {
            return false;
        }

        const auto &current_scope = variable_scopes_.back();
        return current_scope.find(name) != current_scope.end();
    }

    /**
     * @brief Check if variable is declared (compatibility method)
     */
    bool isVariableDeclared(const std::string &name) const
    {
        for (auto it = variable_scopes_.rbegin(); it != variable_scopes_.rend(); ++it)
        {
            if (it->find(name) != it->end())
            {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Get variable type (compatibility method)
     */
    TypeInfo getVariableType(const std::string &name) const
    {
        for (auto it = variable_scopes_.rbegin(); it != variable_scopes_.rend(); ++it)
        {
            auto found = it->find(name);
            if (found != it->end())
            {
                return found->second.type;
            }
        }
        return TypeInfo(TypeInfo::Kind::Unknown);
    }

    /**
     * @brief Check if function is declared
     */
    bool isFunctionDeclared(const std::string &name) const
    {
        return functions_.find(name) != functions_.end();
    }

    /**
     * @brief Get function parameters (simplified for compatibility)
     */
    std::vector<std::string> getFunctionParams(const std::string &name) const
    {
        auto found = functions_.find(name);
        if (found != functions_.end())
        {
            std::vector<std::string> params;
            for (size_t i = 0; i < found->second->parameter_types.size(); ++i)
            {
                params.push_back("param" + std::to_string(i));
            }
            return params;
        }
        return {};
    }

    /**
     * @brief Declare a function with parameter names (compatibility method)
     */
    void declareFunction(const std::string &name, const std::vector<std::string> &param_names)
    {
        std::vector<TypeInfo> param_types;
        for (size_t i = 0; i < param_names.size(); ++i)
        {
            param_types.push_back(TypeInfo(TypeInfo::Kind::Unknown));
        }
        declareFunction(name, param_types, TypeInfo(TypeInfo::Kind::Unknown), 0);
    }

    /**
     * @brief Declare a type
     */
    bool declareType(const std::string &name, const std::string &base_type = "Object", int line = 0)
    {
        // Check if type already exists
        if (types_.find(name) != types_.end())
        {
            return false;
        }

        types_[name] = std::make_shared<TypeSymbol>(name, base_type, line);
        return true;
    }

    /**
     * @brief Store a type declaration for parameter checking
     */
    void storeTypeDeclaration(const std::string &name, TypeDecl *declaration)
    {
        type_declarations_[name] = declaration;
    }

    /**
     * @brief Get a type declaration for parameter checking
     */
    TypeDecl *getTypeDeclaration(const std::string &name) const
    {
        auto found = type_declarations_.find(name);
        return (found != type_declarations_.end()) ? found->second : nullptr;
    }

    /**
     * @brief Check if type is declared
     */
    bool isTypeDeclared(const std::string &name) const
    {
        return types_.find(name) != types_.end();
    }

    /**
     * @brief Look up a type
     */
    std::shared_ptr<TypeSymbol> lookupType(const std::string &name)
    {
        auto found = types_.find(name);
        return (found != types_.end()) ? found->second : nullptr;
    }

    /**
     * @brief Add an attribute to a type
     */
    bool addTypeAttribute(const std::string &type_name, const std::string &attr_name,
                          const TypeInfo &type, int line = 0)
    {
        auto type_symbol = lookupType(type_name);
        if (!type_symbol)
        {
            return false;
        }

        // Check if attribute already exists
        if (type_symbol->attributes.find(attr_name) != type_symbol->attributes.end())
        {
            return false;
        }

        type_symbol->attributes[attr_name] = Symbol(attr_name, type, true, line);
        return true;
    }

    /**
     * @brief Add a method to a type
     */
    bool addTypeMethod(const std::string &type_name, const std::string &method_name,
                       const std::vector<TypeInfo> &params, const TypeInfo &return_type, int line = 0)
    {
        auto type_symbol = lookupType(type_name);
        if (!type_symbol)
        {
            return false;
        }

        // Check if method already exists
        if (type_symbol->methods.find(method_name) != type_symbol->methods.end())
        {
            return false;
        }

        type_symbol->methods[method_name] = std::make_shared<FunctionSymbol>(method_name, params, return_type, line);
        return true;
    }

    /**
     * @brief Look up an attribute in a type (recursively in base types)
     */
    Symbol *lookupAttribute(const std::string &type_name, const std::string &attr_name)
    {
        auto type_symbol = lookupType(type_name);
        if (!type_symbol)
            return nullptr;

        auto found = type_symbol->attributes.find(attr_name);
        if (found != type_symbol->attributes.end())
            return &found->second;

        // Buscar en el padre si existe y no es Object
        if (type_symbol->base_type != "Object")
            return lookupAttribute(type_symbol->base_type, attr_name);

        return nullptr;
    }

    /**
     * @brief Look up a method in a type (recursively in base types)
     */
    std::shared_ptr<FunctionSymbol> lookupMethod(const std::string &type_name, const std::string &method_name)
    {
        auto type_symbol = lookupType(type_name);
        if (!type_symbol)
            return nullptr;

        auto found = type_symbol->methods.find(method_name);
        if (found != type_symbol->methods.end())
            return found->second;

        // Buscar en el padre si existe y no es Object
        if (type_symbol->base_type != "Object")
            return lookupMethod(type_symbol->base_type, method_name);

        return nullptr;
    }

private:
    /**
     * @brief Add built-in functions to symbol table
     */
    void addBuiltinFunctions()
    {
        // Math functions
        declareFunction("sin", {TypeInfo(TypeInfo::Kind::Number)}, TypeInfo(TypeInfo::Kind::Number));
        declareFunction("cos", {TypeInfo(TypeInfo::Kind::Number)}, TypeInfo(TypeInfo::Kind::Number));
        declareFunction("sqrt", {TypeInfo(TypeInfo::Kind::Number)}, TypeInfo(TypeInfo::Kind::Number));
        declareFunction("exp", {TypeInfo(TypeInfo::Kind::Number)}, TypeInfo(TypeInfo::Kind::Number));
        declareFunction("log", {TypeInfo(TypeInfo::Kind::Number), TypeInfo(TypeInfo::Kind::Number)},
                        TypeInfo(TypeInfo::Kind::Number));
        declareFunction("rand", {}, TypeInfo(TypeInfo::Kind::Number));

        // String functions
        declareFunction("print", {TypeInfo(TypeInfo::Kind::Unknown)}, TypeInfo(TypeInfo::Kind::Void));
    }
};