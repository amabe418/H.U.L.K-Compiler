#pragma once
#include<vector>
#include <string>

enum SymbolType
{
    Terminal,
    NonTerminal,
    Epsilon
};

struct Symbol
{
    std::string value;
    SymbolType type;

    bool operator<(const Symbol& other) const {
        return value < other.value; // se ordena según el valor de 'name'
    }

    bool operator==(const Symbol& other) const {
        return value == other.value; // se ordena según el valor de 'name'
    }

    
    
    Symbol(std::string val,SymbolType t)
    {
        value=val;
        type=t;
        
    }
    Symbol()
    :value("ε"),type(SymbolType::Epsilon){}

    

};

struct Production
{

    Symbol left;
    std::vector<Symbol> right;


    Production(Symbol left,std::vector<Symbol>right)
    {
        this->left=left;
        this->right=right;
    } 
    
};


