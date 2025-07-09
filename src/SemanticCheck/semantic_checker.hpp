#pragma once
#include "../AST/ast.hpp"
#include "../SymbolTable/symbol_table.hpp"
#include "error.hpp"
#include "../Types/type_info.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <set>

/**
 * @brief Semantic analyzer for HULK language
 *
 * This class performs semantic analysis on the AST, including:
 * - Type checking
 * - Variable declaration and usage validation
 * - Function signature checking
 * - Scope management
 */
class SemanticAnalyzer : public ExprVisitor, public StmtVisitor
{
private:
    SymbolTable symbol_table_;
    ErrorManager error_manager_; // Use ErrorManager instead of vector<SemanticError>
    TypeInfo current_type_;
    std::string currentMethodName_; // Track current method name for base() calls

public:
    SemanticAnalyzer() : current_type_(TypeInfo::Kind::Unknown)
    {
        // Configurar el symbol table en TypeInfo para que pueda acceder a la información de herencia
        TypeInfo::setSymbolTable(&symbol_table_);

        // Registrar funciones built-in
        registerBuiltinFunctions();
    }
    /**
     * @brief Analyze a program (entry point)
     */
    void analyze(Program *program);

    /**
     * @brief Set the symbol table to use for analysis
     */
    void setSymbolTable(SymbolTable *symbol_table);

    /**
     * @brief Get all semantic errors found
     */
    const std::vector<SemanticError> &getErrors() const
    {
        return error_manager_.getErrors();
    }

    /**
     * @brief Check if there are any errors
     */
    bool hasErrors() const
    {
        return error_manager_.hasErrors();
    }

    /**
     * @brief Print all errors to stderr
     */
    void printErrors() const
    {
        error_manager_.printErrors();
    }
    /**
     * @brief Get current inferred type
     */
    const TypeInfo &getCurrentType() const
    {
        return current_type_;
    }

    // Visitor pattern implementation - ExprVisitor
    void visit(NumberExpr *expr) override;
    void visit(StringExpr *expr) override;
    void visit(BooleanExpr *expr) override;
    void visit(UnaryExpr *expr) override;
    void visit(BinaryExpr *expr) override;
    void visit(CallExpr *expr) override;
    void visit(VariableExpr *expr) override;
    void visit(LetExpr *expr) override;
    void visit(AssignExpr *expr) override;
    void visit(IfExpr *expr) override;
    void visit(ExprBlock *expr) override;
    void visit(WhileExpr *expr) override;
    void visit(ForExpr *expr) override;
    void visit(NewExpr *expr) override;
    void visit(GetAttrExpr *expr) override;
    void visit(SelfExpr *expr) override;
    void visit(BaseCallExpr *expr) override;
    void visit(SetAttrExpr *expr) override;
    void visit(MethodCallExpr *expr) override;
    void visit(IsExpr *expr) override;
    void visit(AsExpr *expr) override;

    // Visitor pattern implementation - StmtVisitor
    void visit(Program *prog) override;
    void visit(ExprStmt *stmt) override;
    void visit(FunctionDecl *stmt) override;
    void visit(TypeDecl *stmt) override;
    void visit(MethodDecl *stmt) override;
    void visit(AttributeDecl *stmt) override;

private:
    /**
     * @brief First pass: collect all function and type declarations
     */
    void collectFunctions(Program *program);

    /**
     * @brief Register built-in functions in the symbol table
     */
    void registerBuiltinFunctions();

    /**
     * @brief Report a semantic error for expressions
     */
    void reportError(ErrorType type, const std::string &message, Expr *expr,
                     const std::string &context = "");

    /**
     * @brief Report a semantic error for statements
     */
    void reportError(ErrorType type, const std::string &message, Stmt *stmt,
                     const std::string &context = "");

    /**
     * @brief Report a semantic error without specific AST node
     */
    void reportError(ErrorType type, const std::string &message,
                     const std::string &context = "");

    /**
     * @brief Convert binary operator enum to string
     */
    std::string getBinaryOpString(BinaryExpr::Op op);

    /**
     * @brief Check if two types are compatible using conforming relationship
     */
    bool areTypesCompatible(const TypeInfo &type1, const TypeInfo &type2);

    /**
     * @brief Check if a word is reserved in HULK language
     */
    bool isReservedWord(const std::string &word);

    /**
     * @brief Validate inheritance chain and detect cycles
     */
    bool validateInheritanceChain(const std::string &typeName, std::set<std::string> &visited);

    /**
     * @brief Convert TypeInfo to string representation
     */
    std::string typeToString(const TypeInfo &type);

    /**
     * @brief Compute the unified type (LCA) between two types
     */
    TypeInfo computeUnifiedType(const TypeInfo& type1, const TypeInfo& type2);

    /**
     * @brief Find the lowest common ancestor of two types in the inheritance hierarchy
     */
    std::string findLowestCommonAncestor(const std::string& type1, const std::string& type2);

    /**
     * @brief Get the inheritance chain for a type (from child to Object)
     */
    std::vector<std::string> getInheritanceChain(const std::string& typeName);
};