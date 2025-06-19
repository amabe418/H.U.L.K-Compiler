#pragma once
#include <memory>
#include <stdexcept>
#include "../Scope/scope.hpp"
#include "../AST/ast.hpp"

class NameResolver : public StmtVisitor, public ExprVisitor
{
    using SymScope = Scope<SymbolInfo>;
    SymScope::Ptr currentScope_;

public:
    NameResolver()
        : currentScope_(std::make_shared<SymScope>(nullptr)) // scope global
    {                                                        // Pre-declarar las funciones nativas:
        for (auto &fn : {
                 "print", "sqrt", "log", "sin", "cos", "pow",
                 "rand", "range", "iter", "next", "current",
                 "PI", "E", "function", "if", "else",
                 "debug", "type", "assert", "str"})
        {
            currentScope_->declare(fn, SymbolInfo{SymbolInfo::FUNCTION});
        }
    }

    // ---------------- StmtVisitor ----------------
    void visit(Program *p) override
    {
        for (auto &s : p->stmts)
            s->accept(this);
    }

    void visit(ExprStmt *s) override
    {
        // Una expresión suelta → resolver dentro de ella
        s->expr->accept(this);
    }

    void visit(FunctionDecl *f) override
    {
        // 1) Declarar la función
        if (currentScope_->existsInCurrent(f->name))
            throw std::runtime_error("Redeclaración de función: " + f->name);
        currentScope_->declare(f->name, {SymbolInfo::FUNCTION});
        // 2) Nuevo scope para parámetros + cuerpo
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        for (auto &param : f->params)
        {
            if (currentScope_->existsInCurrent(param))
                throw std::runtime_error("Redeclaración de parámetro: " + param);
            currentScope_->declare(param, {SymbolInfo::VARIABLE});
        }
        f->body->accept(this);
        // 3) Cerrar scope
        currentScope_ = parent;
    }

    void visit(TypeDecl *t) override
    {
        // 1) Declarar el tipo
        if (currentScope_->existsInCurrent(t->name))
            throw std::runtime_error("Redeclaración de tipo: " + t->name);
        currentScope_->declare(t->name, {SymbolInfo::TYPE});

        // 2) Nuevo scope para atributos y métodos
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);

        // 3) Agregar self al scope
        currentScope_->declare("self", {SymbolInfo::VARIABLE});

        // 4) Procesar atributos
        for (const auto &attr : t->attributes)
        {
            if (currentScope_->existsInCurrent(attr->name))
                throw std::runtime_error("Redeclaración de atributo: " + attr->name);
            currentScope_->declare(attr->name, {SymbolInfo::VARIABLE});
            attr->initializer->accept(this);
        }

        // 5) Procesar métodos
        for (const auto &method : t->methods)
        {
            if (currentScope_->existsInCurrent(method->name))
                throw std::runtime_error("Redeclaración de método: " + method->name);
            currentScope_->declare(method->name, {SymbolInfo::FUNCTION});

            // Nuevo scope para parámetros del método
            auto methodParent = currentScope_;
            currentScope_ = std::make_shared<SymScope>(methodParent);

            // Agregar parámetros al scope
            for (const auto &param : method->params)
            {
                if (currentScope_->existsInCurrent(param))
                    throw std::runtime_error("Redeclaración de parámetro en método: " + param);
                currentScope_->declare(param, {SymbolInfo::VARIABLE});
            }

            // Procesar cuerpo del método
            method->body->accept(this);

            // Restaurar scope del tipo
            currentScope_ = methodParent;
        }

        // 6) Restaurar scope global
        currentScope_ = parent;
    }

    // ---------------- ExprVisitor ----------------
    // 1) Literales no necesitan resolución
    void visit(NumberExpr *) override {}
    void visit(StringExpr *) override {}
    void visit(BooleanExpr *) override {}

    // 2) Operadores
    void visit(UnaryExpr *expr) override
    {
        expr->operand->accept(this);
    }
    void visit(BinaryExpr *expr) override
    {
        expr->left->accept(this);
        expr->right->accept(this);
    }

    // 3) Llamadas y variables
    void visit(CallExpr *expr) override
    {
        // Verifica que la función exista en el scope
        currentScope_->lookup(expr->callee);
        for (auto &arg : expr->args)
            arg->accept(this);
    }
    void visit(VariableExpr *expr) override
    {
        currentScope_->lookup(expr->name);
    }

    // 4) Let / Assign
    void visit(LetExpr *expr) override
    {
        expr->initializer->accept(this);
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        if (currentScope_->existsInCurrent(expr->name))
            throw std::runtime_error("Redeclaración de variable: " + expr->name);
        currentScope_->declare(expr->name, {SymbolInfo::VARIABLE});
        expr->body->accept(this);
        currentScope_ = parent;
    }
    void visit(AssignExpr *expr) override
    {
        // Verificar variable ya declarada
        currentScope_->lookup(expr->name);
        expr->value->accept(this);
    }

    // 5) Control de flujo
    void visit(IfExpr *expr) override
    {
        expr->condition->accept(this);
        expr->thenBranch->accept(this);
        if (expr->elseBranch)
            expr->elseBranch->accept(this);
    }
    void visit(ExprBlock *expr) override
    {
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        for (auto &stmt : expr->stmts)
            stmt->accept(this);
        currentScope_ = parent;
    }
    void visit(WhileExpr *expr) override
    {
        expr->condition->accept(this);
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        expr->body->accept(this);
        currentScope_ = parent;
    }

    // 6) Expresiones de objetos
    void visit(NewExpr *expr) override
    {
        // Verificar que el tipo existe
        currentScope_->lookup(expr->typeName);
        // Procesar argumentos del constructor
        for (auto &arg : expr->args)
        {
            arg->accept(this);
        }
    }

    void visit(GetAttrExpr *expr) override
    {
        // Procesar el objeto primero
        expr->object->accept(this);
        // No necesitamos verificar el atributo aquí, eso se hace en el análisis semántico
    }

    void visit(SetAttrExpr *expr) override
    {
        // Procesar el objeto primero
        expr->object->accept(this);
        // Procesar el valor
        expr->value->accept(this);
    }

    void visit(MethodCallExpr *expr) override
    {
        // Procesar el objeto primero
        expr->object->accept(this);
        // Procesar los argumentos
        for (auto &arg : expr->args)
        {
            arg->accept(this);
        }
    }

    void visit(SelfExpr *) override
    {
        try
        {
            currentScope_->lookup("self");
        }
        catch (const std::runtime_error &)
        {
            throw std::runtime_error("'self' solo puede usarse dentro de un método");
        }
    }

    void visit(BaseCallExpr *expr) override
    {
        try
        {
            currentScope_->lookup("self");
        }
        catch (const std::runtime_error &)
        {
            throw std::runtime_error("'base' solo puede usarse dentro de un método");
        }
        // Procesar los argumentos
        for (auto &arg : expr->args)
        {
            arg->accept(this);
        }
    }
};
