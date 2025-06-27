#pragma once
#include "grammarItems.hpp"
#include "../../common/AstNodes.hpp"

class ParseNode;

class ParseTreeVisitor
{
    public:
        virtual ~ParseTreeVisitor(){};
        virtual AstNode* visit(ParseNode* node)=0;

};