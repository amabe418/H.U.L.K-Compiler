#pragma once

#include "../AST/ast.hpp"
#include "derivation_tree.hpp"
#include <memory>
#include <string>
#include <vector>

// Estructura auxiliar para representar un binding de variable
struct VarBinding
{
    std::string name;
    std::unique_ptr<Expr> initializer;
    std::shared_ptr<TypeInfo> declaredType;

    VarBinding(std::string n, std::unique_ptr<Expr> init, std::shared_ptr<TypeInfo> type = nullptr)
        : name(std::move(n)), initializer(std::move(init)), declaredType(std::move(type)) {}
};

class CSTToASTConverter
{
public:
    // Main conversion function
    std::unique_ptr<Program> convertToAST(DerivationNode *cst);

private:
    // Convert different types of nodes
    std::unique_ptr<Stmt> convertStmt(DerivationNode *node);
    std::unique_ptr<Expr> convertExpr(DerivationNode *node);

    // Convert specific statement types
    std::unique_ptr<FunctionDecl> convertFunctionDef(DerivationNode *node);
    std::unique_ptr<TypeDecl> convertTypeDef(DerivationNode *node);
    std::unique_ptr<ExprStmt> convertExprStmt(DerivationNode *node);

    // Convert specific expression types
    std::unique_ptr<Expr> convertPrimary(DerivationNode *node);
    std::unique_ptr<Expr> convertPrimaryTail(std::unique_ptr<Expr> base, DerivationNode *node);
    std::unique_ptr<BinaryExpr> convertBinaryExpr(DerivationNode *node);
    std::unique_ptr<UnaryExpr> convertUnaryExpr(DerivationNode *node);
    std::unique_ptr<LetExpr> convertLetExpr(DerivationNode *node);
    std::unique_ptr<IfExpr> convertIfExpr(DerivationNode *node);
    std::unique_ptr<WhileExpr> convertWhileExpr(DerivationNode *node);
    std::unique_ptr<ForExpr> convertForExpr(DerivationNode *node);
    std::unique_ptr<CallExpr> convertCallExpr(DerivationNode *node);
    std::unique_ptr<NewExpr> convertNewExpr(DerivationNode *node);
    std::unique_ptr<IsExpr> convertIsExpr(DerivationNode *node);
    std::unique_ptr<AsExpr> convertAsExpr(DerivationNode *node);
    std::unique_ptr<BaseCallExpr> convertBaseCallExpr(DerivationNode *node);
    std::unique_ptr<AssignExpr> convertAssignExpr(DerivationNode *node);
    std::unique_ptr<ExprBlock> convertExprBlock(DerivationNode *node);

    // Expression conversion functions following grammar structure
    void convertStmtList(DerivationNode *node, std::vector<std::unique_ptr<Stmt>> &stmts);
    std::unique_ptr<Expr> convertOrExpr(DerivationNode *node);
    std::unique_ptr<Expr> convertAndExpr(DerivationNode *node);
    std::unique_ptr<Expr> convertCmpExpr(DerivationNode *node);
    std::unique_ptr<Expr> convertConcatExpr(DerivationNode *node);
    std::unique_ptr<Expr> convertAddExpr(DerivationNode *node);
    std::unique_ptr<Expr> convertTerm(DerivationNode *node);
    std::unique_ptr<Expr> convertFactor(DerivationNode *node);
    std::unique_ptr<Expr> convertPower(DerivationNode *node);
    std::unique_ptr<Expr> convertUnary(DerivationNode *node);
    std::unique_ptr<Expr> convertBlockStmt(DerivationNode *node);

    // Prime functions for left-associative recursion (like reference project)
    std::unique_ptr<Expr> convertOrExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);
    std::unique_ptr<Expr> convertAndExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);
    std::unique_ptr<Expr> convertCmpExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);
    std::unique_ptr<Expr> convertConcatExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);
    std::unique_ptr<Expr> convertAddExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);
    std::unique_ptr<Expr> convertTermPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);
    std::unique_ptr<Expr> convertFactorPrime(std::unique_ptr<Expr> inherited, DerivationNode *node);

    // Helper functions
    std::string getTokenValue(DerivationNode *node);
    bool isTerminal(DerivationNode *node);

    // Convert operators
    BinaryExpr::Op convertBinaryOp(const std::string &op);
    UnaryExpr::Op convertUnaryOp(const std::string &op);

    // Convert function arguments
    std::vector<std::string> convertArgIdList(DerivationNode *node);
    std::pair<std::vector<std::string>, std::vector<std::shared_ptr<TypeInfo>>> convertArgIdListWithTypes(DerivationNode *node);
    std::vector<std::unique_ptr<Expr>> convertArgList(DerivationNode *node);
    std::vector<std::unique_ptr<Expr>> convertArgListTail(DerivationNode *node);

    // FunctionDecl helper functions
    std::unique_ptr<Stmt> convertFunctionBody(DerivationNode *node);
    std::pair<std::vector<std::string>, std::vector<std::shared_ptr<TypeInfo>>> convertArgIdListTailWithTypes(DerivationNode *node);
    std::pair<std::string, std::shared_ptr<TypeInfo>> convertArgId(DerivationNode *node);

    // Convert type annotations
    std::shared_ptr<TypeInfo> convertTypeAnnotation(DerivationNode *node);

    // TypeDef helper functions
    std::pair<std::vector<std::string>, std::vector<std::shared_ptr<TypeInfo>>> convertTypeParams(DerivationNode *node);
    std::pair<std::string, std::vector<std::unique_ptr<Expr>>> convertTypeInheritance(DerivationNode *node);
    std::vector<std::unique_ptr<Expr>> convertTypeBaseArgs(DerivationNode *node);
    std::pair<std::vector<std::unique_ptr<AttributeDecl>>, std::vector<std::unique_ptr<MethodDecl>>> convertTypeBody(DerivationNode *node);
    std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> convertTypeMember(DerivationNode *node);
    std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> convertTypeMemberTail(const std::string &memberName, DerivationNode *node);

    // LetExpr helper functions
    std::vector<VarBinding> convertVarBindingList(DerivationNode *node);
    std::vector<VarBinding> convertVarBindingListTail(DerivationNode *node);
    std::unique_ptr<Stmt> convertLetBody(DerivationNode *node);
    VarBinding convertVarBinding(DerivationNode *node);

    // IfExpr helper functions
    std::unique_ptr<Stmt> convertIfBody(DerivationNode *node);
    std::vector<std::unique_ptr<IfExpr>> convertElifList(DerivationNode *node);
    std::unique_ptr<IfExpr> convertElifBranch(DerivationNode *node);

    // WhileExpr helper functions
    std::unique_ptr<Stmt> convertWhileBody(DerivationNode *node);

    // ForExpr helper functions
    std::unique_ptr<Stmt> convertForBody(DerivationNode *node);

    // Legacy functions (kept for compatibility but not used)
    DerivationNode *findChild(DerivationNode *node, const std::string &symbol);
    std::vector<DerivationNode *> findChildren(DerivationNode *node, const std::string &symbol);
    bool isEpsilon(DerivationNode *node);

    // Helper functions for line and column information
    std::pair<int, int> getLineColumn(DerivationNode *node);
    void setLineColumn(Expr *expr, DerivationNode *node);
    void setLineColumn(Stmt *stmt, DerivationNode *node);
};