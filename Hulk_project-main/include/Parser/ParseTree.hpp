#pragma once
#include <iostream>
#include "grammarItems.hpp"
#include "ParseTreeVisitor.hpp"

class ParseNode
{
    public:
        std::vector<ParseNode*> m_children;
        ParseNode* m_parent;
        Symbol* m_value;
        Token m_token;

        ParseNode(Symbol*value)
        {
            m_value=value;
            m_parent=nullptr;
        }

        AstNode*  accept(ParseTreeVisitor &visitor)
        {
            return visitor.visit(this);
        }

  
};


class ParseTree
{
    public:
        ParseNode* m_root;
    
        ParseTree(ParseNode* root)
        : m_root(root){};


        AstNode* accept(ParseTreeVisitor &visitor)
        {
            return  m_root->accept(visitor);
        }
        

    void printParseTree(ParseNode* node, int level = 0) 
    {
        if (!node)
            return;

        // Imprimir la indentación (dos espacios por nivel)
        for (int i = 0; i < level; ++i)
            std::cout << "  ";

        // Imprimir el valor del nodo; se asume que m_value no es nulo.
        if (node->m_value)
            std::cout << node->m_value->value << "\n";
        else
            std::cout << "null\n";

        // Recorrer e imprimir recursivamente a cada hijo, incrementando el nivel.
        for (ParseNode* child : node->m_children) {
            printParseTree(child, level + 1);
        }
}

};