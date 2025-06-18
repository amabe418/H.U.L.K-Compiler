// symbol_collector.hpp
#pragma once

#include <iostream>
#include <vector>

#include "AST/ast.hpp"
#include "Symbols/symbol_table.hpp"

class TypeError : public std::runtime_error
{
   public:
    explicit TypeError(const std::string& message) : std::runtime_error(message) {}
};

class SymbolCollector : public ExprVisitor, public StmtVisitor
{
   public:
    SymbolCollector(SymbolTable& table) : symbolTable(table) {}

    // --- StmtVisitor ---
    void visit(Program* prog) override;
    void visit(FunctionDecl* func) override;
    void visit(TypeDecl* typeDecl) override;
    void visit(MethodDecl* method) override;
    void visit(ExprStmt* stmt) override;

    // --- ExprVisitor ---
    void visit(LetExpr* let) override;
    void visit(ExprBlock* block) override;
    void visit(NumberExpr*) override;
    void visit(StringExpr*) override;
    void visit(BooleanExpr*) override;
    void visit(UnaryExpr* expr) override;
    void visit(BinaryExpr* expr) override;
    void visit(CallExpr* expr) override;
    void visit(VariableExpr*) override;
    void visit(AssignExpr* expr) override;
    void visit(IfExpr* expr) override;
    void visit(WhileExpr* expr) override;
    void visit(NewExpr* expr) override;
    void visit(GetAttrExpr* expr) override;
    void visit(SetAttrExpr* expr) override;
    void visit(MethodCallExpr* expr) override;
    void visit(SelfExpr*) override;
    void visit(BaseCallExpr* expr) override;

    // Método para imprimir la tabla de símbolos
    void
    printSymbolTable()
    {
        std::cout << "\n=== Symbol Table Contents ===\n" << std::endl;
        symbolTable.print();
        std::cout << "\n=== End of Symbol Table ===\n" << std::endl;
    }

   private:
    SymbolTable& symbolTable;
    std::vector<std::string> callStack;  // Stack para rastrear las llamadas a funciones

    void
    pushCaller(const std::string& caller)
    {
        callStack.push_back(caller);
    }

    void
    popCaller()
    {
        if (!callStack.empty())
        {
            callStack.pop_back();
        }
    }

    std::string
    getCurrentCaller() const
    {
        return callStack.empty() ? "global" : callStack.back();
    }
};
