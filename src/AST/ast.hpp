// ast.hpp
#pragma once

#ifndef AST_HPP
#define AST_HPP

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../Types/type_info.hpp"
#include "visitor.hpp"

struct Program;
struct NumberExpr;
struct StringExpr;
struct BooleanExpr;
struct UnaryExpr;
struct BinaryExpr;
struct CallExpr;
struct VariableExpr;
struct LetExpr;
struct Stmt;
struct ExprStmt;
struct AssignExpr;
struct FunctionDecl;
struct IfExpr;
struct ExprBlock;
struct WhileExpr;
struct ForExpr;
struct BaseCallExpr;
struct IsExpr;

struct TypeDecl;
struct AttributeDecl;
struct MethodDecl;
struct NewExpr;
struct GetAttrExpr;
struct SetAttrExpr;
struct MethodCallExpr;
struct SelfExpr;

struct Instance;

// Forward declarations for visitors

// Base class for all expression nodes
struct Expr
{
    std::shared_ptr<TypeInfo> inferredType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    int line_number = 0;
    int column_number = 0;
    virtual void accept(ExprVisitor *v) = 0;
    virtual ~Expr() = default;

    // Add methods to check parameter usage
    virtual bool containsArithmeticOperation(const std::string &paramName) const { return false; }
    virtual bool containsBooleanOperation(const std::string &paramName) const { return false; }
    virtual bool containsStringOperation(const std::string &paramName) const { return false; }
};

using ExprPtr = std::unique_ptr<Expr>;

// base class for all statement nodes.
struct Stmt
{
    std::shared_ptr<TypeInfo> inferredType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    int line_number = 0;
    int column_number = 0;
    virtual void accept(StmtVisitor *) = 0;
    virtual ~Stmt() = default;

    // Add methods to check parameter usage
    virtual bool containsArithmeticOperation(const std::string &paramName) const { return false; }
    virtual bool containsBooleanOperation(const std::string &paramName) const { return false; }
    virtual bool containsStringOperation(const std::string &paramName) const { return false; }
};

using StmtPtr = std::unique_ptr<Stmt>;

// program: father of all the statements
struct Program : Stmt
{
    std::vector<StmtPtr> stmts;
    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

// evaluates an expression
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(std::move(e)) {}
    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }

    bool containsArithmeticOperation(const std::string &paramName) const override
    {
        return expr->containsArithmeticOperation(paramName);
    }

    bool containsBooleanOperation(const std::string &paramName) const override
    {
        return expr->containsBooleanOperation(paramName);
    }

    bool containsStringOperation(const std::string &paramName) const override
    {
        return expr->containsStringOperation(paramName);
    }
};

// Literal: numeric
struct NumberExpr : Expr
{
    double value;
    NumberExpr(double v) : value(v) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Literal: string
struct StringExpr : Expr
{
    std::string value;
    StringExpr(const std::string &s) : value(s) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Literal: bool
struct BooleanExpr : Expr
{
    bool value;
    BooleanExpr(bool v) : value(v) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Unary operation: e.g., negation
struct UnaryExpr : Expr
{
    enum Op
    {
        OP_NEG
    } op;
    ExprPtr operand;
    UnaryExpr(Op o, ExprPtr expr) : op(o), operand(std::move(expr)) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Binary operation: +, -, *, /, ^, comparisons, mod
struct BinaryExpr : Expr
{
    enum Op
    {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_POW,
        OP_MOD,
        OP_LT,
        OP_GT,
        OP_LE,
        OP_GE,
        OP_EQ,
        OP_NEQ,
        OP_OR,
        OP_AND,
        OP_CONCAT,
        OP_CONCAT_WS
    } op;
    ExprPtr left;
    ExprPtr right;
    BinaryExpr(Op o, ExprPtr l, ExprPtr r) : op(o), left(std::move(l)), right(std::move(r)) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }

    bool containsArithmeticOperation(const std::string &paramName) const override
    {
        if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV || op == OP_MOD || op == OP_POW)
        {
            return left->containsArithmeticOperation(paramName) || right->containsArithmeticOperation(paramName);
        }
        return left->containsArithmeticOperation(paramName) || right->containsArithmeticOperation(paramName);
    }

    bool containsBooleanOperation(const std::string &paramName) const override
    {
        if (op == OP_AND || op == OP_OR || op == OP_EQ || op == OP_NEQ ||
            op == OP_LT || op == OP_GT || op == OP_LE || op == OP_GE)
        {
            return left->containsBooleanOperation(paramName) || right->containsBooleanOperation(paramName);
        }
        return left->containsBooleanOperation(paramName) || right->containsBooleanOperation(paramName);
    }

    bool containsStringOperation(const std::string &paramName) const override
    {
        if (op == OP_CONCAT || op == OP_CONCAT_WS)
        {
            return left->containsStringOperation(paramName) || right->containsStringOperation(paramName);
        }
        return left->containsStringOperation(paramName) || right->containsStringOperation(paramName);
    }
};

// Function call: sqrt, sin, cos, exp, log, rand
struct CallExpr : Expr
{
    std::string callee;
    std::vector<ExprPtr> args;
    CallExpr(const std::string &name, std::vector<ExprPtr> &&arguments)
        : callee(name), args(std::move(arguments))
    {
    }
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// **VariableExpr**: para referirse a un identificador
struct VariableExpr : Expr
{
    std::string name;
    VariableExpr(const std::string &n) : name(n) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }

    bool containsArithmeticOperation(const std::string &paramName) const override
    {
        return name == paramName;
    }

    bool containsBooleanOperation(const std::string &paramName) const override
    {
        return name == paramName;
    }

    bool containsStringOperation(const std::string &paramName) const override
    {
        return name == paramName;
    }
};

// **LetExpr**: let <name> = <init> in <body>
struct LetExpr : Expr
{
    std::string name;                       // nombre de la variable
    ExprPtr initializer;                    // expresión inicializadora
    StmtPtr body;                           // cuerpo donde la variable está en alcance
    std::shared_ptr<TypeInfo> declaredType; // tipo anotado por el usuario (opcional)

    LetExpr(const std::string &n, ExprPtr init, StmtPtr b,
            std::shared_ptr<TypeInfo> type = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown))
        : name(n), initializer(std::move(init)), body(std::move(b)), declaredType(std::move(type))
    {
    }
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// a := b  destructive assignment
struct AssignExpr : Expr
{
    std::string name;
    ExprPtr value;

    AssignExpr(const std::string &n, ExprPtr v) : name(n), value(std::move(v)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// for function's declaration
struct FunctionDecl : Stmt
{
    std::string name;

    // Lista de nombres de parámetros
    std::vector<std::string> params;

    // Tipos anotados por el usuario (puede haber inferencia si std::nullopt)
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;

    // Tipo de retorno anotado (opcional)
    std::shared_ptr<TypeInfo> returnType;

    // Cuerpo de la función (Expr simple o ExprBlock)
    StmtPtr body;

    FunctionDecl(const std::string &n, std::vector<std::string> &&p, StmtPtr b,
                 std::vector<std::shared_ptr<TypeInfo>> &&pt = {},
                 std::shared_ptr<TypeInfo> rt = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown))
        : name(n),
          params(std::move(p)),
          paramTypes(std::move(pt)),
          returnType(std::move(rt)),
          body(std::move(b))
    {
    }

    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

// if-else expressions:
struct IfExpr : Expr
{
    ExprPtr condition;
    ExprPtr thenBranch;
    ExprPtr elseBranch;

    IfExpr(ExprPtr cond, ExprPtr thenB, ExprPtr elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB))
    {
    }

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// bloques de expresiones
struct ExprBlock : Expr
{
    std::vector<StmtPtr> stmts;
    ExprBlock(std::vector<StmtPtr> &&s) : stmts(std::move(s)) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }

    bool containsArithmeticOperation(const std::string &paramName) const override
    {
        for (const auto &stmt : stmts)
        {
            if (stmt->containsArithmeticOperation(paramName))
            {
                return true;
            }
        }
        return false;
    }

    bool containsBooleanOperation(const std::string &paramName) const override
    {
        for (const auto &stmt : stmts)
        {
            if (stmt->containsBooleanOperation(paramName))
            {
                return true;
            }
        }
        return false;
    }

    bool containsStringOperation(const std::string &paramName) const override
    {
        for (const auto &stmt : stmts)
        {
            if (stmt->containsStringOperation(paramName))
            {
                return true;
            }
        }
        return false;
    }
};

// patra ciclos while
struct WhileExpr : Expr
{
    ExprPtr condition;
    ExprPtr body;

    WhileExpr(ExprPtr cond, ExprPtr b) : condition(std::move(cond)), body(std::move(b)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// para ciclos for
struct ForExpr : Expr
{
    std::string variable; // nombre de la variable de iteración
    ExprPtr iterable;     // expresión que evalúa a un iterable
    ExprPtr body;         // cuerpo del ciclo

    ForExpr(const std::string &var, ExprPtr iter, ExprPtr b)
        : variable(var), iterable(std::move(iter)), body(std::move(b)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// type A
struct TypeDecl : Stmt
{
    std::string name;
    std::vector<std::string> params;
    std::vector<std::shared_ptr<TypeInfo>> paramTypes; // Tipos explícitos de los parámetros
    std::vector<std::unique_ptr<AttributeDecl>> attributes;
    std::vector<std::unique_ptr<MethodDecl>> methods;

    // HERENCIA
    std::string baseType = "Object";
    std::vector<ExprPtr> baseArgs;

    TypeDecl(std::string n, std::vector<std::string> &&params_,
             std::vector<std::shared_ptr<TypeInfo>> &&paramTypes_,
             std::vector<std::unique_ptr<AttributeDecl>> attrs,
             std::vector<std::unique_ptr<MethodDecl>> meths, std::string base = "Object",
             std::vector<ExprPtr> &&args = {})
        : name(std::move(n)),
          params(std::move(params_)),
          paramTypes(std::move(paramTypes_)),
          attributes(std::move(attrs)),
          methods(std::move(meths)),
          baseType(std::move(base)),
          baseArgs(std::move(args))
    {
    }

    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
    const std::vector<std::string> &
    getParams() const
    {
        return params;
    }
    const std::vector<std::shared_ptr<TypeInfo>> &
    getParamTypes() const
    {
        return paramTypes;
    }
};
// atributos de tipos
struct AttributeDecl : Stmt
{
    std::string name;
    ExprPtr initializer;
    AttributeDecl(std::string n, ExprPtr expr) : name(std::move(n)), initializer(std::move(expr)) {}

    void accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

// metodos de tipos
struct MethodDecl : Stmt
{
    std::string name;
    std::vector<std::string> params;
    StmtPtr body;

    MethodDecl(const std::string &n, std::vector<std::string> &&p, StmtPtr b)
        : name(n), params(std::move(p)), body(std::move(b))
    {
    }

    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

struct NewExpr : Expr
{
    std::string typeName;
    std::vector<ExprPtr> args; // nuevos

    // Constructor con lista de argumentos
    NewExpr(std::string name, std::vector<ExprPtr> &&args_)
        : typeName(std::move(name)), args(std::move(args_))
    {
    }

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

struct GetAttrExpr : Expr
{
    ExprPtr object;
    std::string attrName;

    GetAttrExpr(ExprPtr obj, std::string attr) : object(std::move(obj)), attrName(std::move(attr))
    {
    }

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

struct SetAttrExpr : Expr
{
    ExprPtr object;
    std::string attrName;
    ExprPtr value;

    SetAttrExpr(ExprPtr obj, std::string attr, ExprPtr val)
        : object(std::move(obj)), attrName(std::move(attr)), value(std::move(val))
    {
    }

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

struct MethodCallExpr : Expr
{
    ExprPtr object;
    std::string methodName;
    std::vector<ExprPtr> args;

    MethodCallExpr(ExprPtr obj, std::string method, std::vector<ExprPtr> &&arguments)
        : object(std::move(obj)), methodName(std::move(method)), args(std::move(arguments))
    {
    }

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

struct SelfExpr : Expr
{
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

struct BaseCallExpr : Expr
{
    std::vector<ExprPtr> args;
    BaseCallExpr(std::vector<ExprPtr> &&a) : args(std::move(a)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

struct IsExpr : Expr
{
    ExprPtr object;
    std::string typeName;

    IsExpr(ExprPtr obj, std::string type) : object(std::move(obj)), typeName(std::move(type)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

#endif // AST_HPP
