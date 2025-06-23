#pragma once

// Forward declarations
struct NumberExpr;
struct StringExpr;
struct BooleanExpr;
struct VariableExpr;
struct UnaryExpr;
struct BinaryExpr;
struct AssignExpr;
struct LetExpr;
struct IfExpr;
struct ExprBlock;
struct CallExpr;
struct WhileExpr;
struct ForExpr;
struct NewExpr;
struct GetAttrExpr;
struct SetAttrExpr;
struct MethodCallExpr;
struct SelfExpr;
struct BaseCallExpr;

struct ExprStmt;
struct Program;
struct FunctionDecl;
struct TypeDecl;
struct MethodDecl;
struct AttributeDecl;
// struct IsExpr;

struct ExprVisitor
{
public:
    virtual void visit(NumberExpr *expr) = 0;
    virtual void visit(StringExpr *expr) = 0;
    virtual void visit(BooleanExpr *expr) = 0;
    virtual void visit(UnaryExpr *expr) = 0;
    virtual void visit(BinaryExpr *expr) = 0;
    virtual void visit(CallExpr *expr) = 0;
    virtual void visit(VariableExpr *expr) = 0;
    virtual void visit(LetExpr *expr) = 0;
    virtual void visit(AssignExpr *expr) = 0;
    virtual void visit(IfExpr *expr) = 0;
    virtual void visit(ExprBlock *expr) = 0;
    virtual void visit(WhileExpr *expr) = 0;
    virtual void visit(ForExpr *expr) = 0;
    virtual void visit(NewExpr *expr) = 0;
    virtual void visit(GetAttrExpr *expr) = 0;
    virtual void visit(SetAttrExpr *expr) = 0;
    virtual void visit(MethodCallExpr *expr) = 0;
    virtual void visit(SelfExpr *expr) = 0;
    virtual void visit(BaseCallExpr *expr) = 0;
    // virtual void visit(IsExpr *expr) = 0;
    virtual ~ExprVisitor() = default;
};

struct StmtVisitor
{
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(ExprStmt *stmt) = 0;
    virtual void visit(Program *stmt) = 0;
    virtual void visit(FunctionDecl *stmt) = 0;
    virtual void visit(TypeDecl *stmt) = 0;
    virtual void visit(MethodDecl *stmt) = 0;
    virtual void visit(AttributeDecl *stmt) = 0;
};