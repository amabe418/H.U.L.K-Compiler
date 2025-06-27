#include "../include/Parser/grammar.hpp"

class Parser
{
public:
    Grammar m_grammar;
    ErrorHandler errorHandler;

    

public:
    Parser( ErrorHandler& errorHandler,std::string filegrammar="grammar.txt"):errorHandler(errorHandler)
    {
        m_grammar= Grammar::loadGrammar(filegrammar);
        errorHandler=m_grammar.errorHandler;

    }

    ParseTree* parse(std::vector<Token> tokens)
    {
        if(m_grammar.errorHandler.hasErrors())
        {
            return nullptr;
        }
        auto temp= m_grammar.parse(tokens);
        ParseTree* tree= new ParseTree(temp);
        errorHandler=m_grammar.errorHandler;
        return tree;
    }

};