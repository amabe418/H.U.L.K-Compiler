#pragma once
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include "theoretical/token.hpp"
#include "derivation_tree.hpp"
#include "AST/ast.hpp"

// Estructura para representar una producción
struct Production
{
    std::string lhs;
    std::vector<std::string> rhs;
};

class LL1Parser
{
public:
    // Constructor: recibe archivo de gramática y tokens
    LL1Parser(const std::string &grammar_file, const std::vector<Token> &tokens);

    // Método principal: hace todo el flujo completo
    std::pair<DerivationNodePtr, Program *> parse();

    // Métodos individuales para debugging
    void printProductions() const;
    void printFirstSets() const;
    void printFollowSets() const;
    void printLL1Table() const;
    void printDerivationTree() const;
    void printAST(Program *ast) const;

private:
    std::string grammar_file;
    std::vector<Token> tokens;
    std::vector<Production> productions;
    std::map<std::string, std::set<std::string>> first_sets;
    std::map<std::string, std::set<std::string>> follow_sets;
    std::map<std::string, std::map<std::string, std::vector<std::string>>> ll1_table;
    std::string start_symbol;
    size_t current_token_pos;
    DerivationNodePtr cst_root;

    // 1. Leer gramática desde archivo
    std::vector<Production> readGrammar(const std::string &filename);

    // 2. Calcular conjuntos FIRST
    std::map<std::string, std::set<std::string>> computeFirst(const std::vector<Production> &productions);

    // 3. Calcular conjuntos FOLLOW
    std::map<std::string, std::set<std::string>> computeFollow(
        const std::vector<Production> &productions,
        const std::map<std::string, std::set<std::string>> &first,
        const std::string &start_symbol);

    // 4. Construir tabla LL(1)
    std::map<std::string, std::map<std::string, std::vector<std::string>>> buildLL1Table(
        const std::vector<Production> &productions,
        const std::map<std::string, std::set<std::string>> &first,
        const std::map<std::string, std::set<std::string>> &follow);

    // 5. Construir árbol de derivación (CST)
    DerivationNodePtr buildDerivationTree();

    // 6. Construir AST desde CST
    Program *buildAST(const DerivationNodePtr &cst_root);

    // Métodos auxiliares para CST
    bool isTerminal(const std::string &symbol) const;
    std::string tokenTypeToSymbol(TokenType type) const;
    TokenType symbolToTokenType(const std::string &symbol) const;
    void printError(const std::string &message) const;
    void printError(const std::string &message, const Token &current_token) const;

    // Métodos auxiliares para AST
    Stmt *buildStmtFromCST(const DerivationNodePtr &cst_node);
    Expr *buildExprFromCST(const DerivationNodePtr &cst_node);
    std::vector<StmtPtr> buildStmtListFromCST(const DerivationNodePtr &cst_node);

    // Métodos auxiliares para FIRST/FOLLOW
    std::map<std::string, std::vector<std::vector<std::string>>> groupProductions(const std::vector<Production> &productions);
    std::set<std::string> computeFirstOfString(const std::vector<std::string> &string, const std::map<std::string, std::set<std::string>> &first);

    // Métodos auxiliares para validación
    bool validateLL1Grammar() const;

    // Métodos auxiliares para imprimir el CST
    void printNode(const DerivationNode *node, int depth) const;
    int countNodes(const DerivationNode *node) const;
    int getMaxDepth(const DerivationNode *node) const;
};