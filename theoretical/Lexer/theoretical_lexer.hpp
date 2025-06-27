#pragma once
#include "../token.hpp"
#include "../automata/nfa.hpp"
#include "../automata/dfa.hpp"
#include "../automata/nfa_to_dfa.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>

class TheoreticalLexer
{
private:
    DFA dfa;
    std::string input;
    size_t currentPos;
    int currentLine;
    int currentColumn;

    // Construir NFA para cada patrón
    NFA buildNumberNFA();
    NFA buildStringNFA();
    NFA buildIdentifierNFA();
    NFA buildKeywordNFA(const std::string &keyword);
    NFA buildOperatorNFA(const std::string &op);
    NFA buildPunctuationNFA(char punct);

    // Construir NFA combinado
    NFA buildCombinedNFA();

    // Construir DFA
    void buildDFA();

    // Utilidades
    void skipWhitespace();
    void skipComments();
    TokenType getTokenType(int state);
    TokenType getTokenType(int state, const std::string &lexeme);

public:
    TheoreticalLexer();
    TheoreticalLexer(const std::string &input);

    // Configuración
    void setInput(const std::string &input);

    // Análisis léxico
    Token getNextToken();
    std::vector<Token> tokenize();

    // Estado
    bool hasMoreTokens() const;
    void reset();

    // Debug
    void printDFA() const;
};