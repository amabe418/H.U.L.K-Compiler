#pragma once

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "grammarItems.hpp"
#include "ParseTree.hpp"
#include "../../common/Error.hpp"

class Grammar {
public:
    Symbol startSymbol;
    std::set<Symbol> terminals;
    std::set<Symbol> non_terminals;
    std::vector<Production> productions;
    ErrorHandler errorHandler;

public:
    Grammar(){};
    Grammar(Symbol startSymbol,
            std::set<Symbol> terminals,
            std::set<Symbol> non_terminals,
            std::vector<Production> productions)
            {
                this->startSymbol=startSymbol;
                this->terminals=terminals;
                this->non_terminals=non_terminals;
                this->productions=productions;
                first=calculateFirst();
                follow=calculateFollow2();

            BuildTable();


            }

    void printParsingTable();
    ParseTree parse(std::vector<Token> inpTokens);
    static Grammar loadGrammar(std::string filename);

private:
    std::map<Symbol, std::set<Symbol>> first;
    std::map<Symbol, std::set<Symbol>> follow;
    std::map<Symbol, std::map<Symbol, int>> parsing_table;

    std::map<Symbol, std::set<Symbol>> calculateFirst();
    std::map<Symbol, std::set<Symbol>> calculateFollow();
    std::map<Symbol, std::set<Symbol>> calculateFollow2();

    std::set<Symbol> list_first(std::vector<Symbol> list);
    void BuildTable();

    static std::string trim(const std::string &s);
};
