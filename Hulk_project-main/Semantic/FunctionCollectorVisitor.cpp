#pragma once
#include "../include/Semantic/FunctionCollectorVisitor.hpp"
  
 void FunctionCollectorVisitor::visit(ProgramNode* node)   
{
        context= new Context();
        context->loadInternalTypeAndMethod();
        for(auto stmt : node->stmts)
        {
           stmt->accept(*this);
        };
}

void FunctionCollectorVisitor::visit(MethodNode* node)
{
    if(context->exist_Method(node->id.lexeme))
    {
        std::string msg="The method method  "+ node->id.lexeme +" is already defined";
        errorHandler.reportError(node->id,msg);
        return;
    }

    std::vector<Attribute> args;
    for(auto param:node->params)
    {
        if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) {
        args.push_back(Attribute(p->value.lexeme,p->type));
        }
        else
        {
            std::string msg="Unexpected error in builder method  "+ node->id.lexeme;
            errorHandler.reportError(node->id,msg);
            return;
        }
    }
    context->DefineMethod(node->id.lexeme,node->type,args);
        
};

void FunctionCollectorVisitor::visit(AttributeNode* node)       
{
    
};


void FunctionCollectorVisitor::visit(TypeNode* node)   {}; 
void FunctionCollectorVisitor::visit(BlockNode* node)          {};
void FunctionCollectorVisitor::visit(BinaryExpression* node)   {};
void FunctionCollectorVisitor::visit(LiteralNode* node)        {};
void FunctionCollectorVisitor::visit(IdentifierNode* node)     {}; 

void FunctionCollectorVisitor::visit(IfExpression* node)       {};
void FunctionCollectorVisitor::visit(WhileExpression* node)    {};
void FunctionCollectorVisitor::visit(ForExression* node)       {};
void FunctionCollectorVisitor::visit(LetExpression* node)      {}; 
void FunctionCollectorVisitor::visit(UnaryExpression* node)      {};
void FunctionCollectorVisitor::visit(FunCallNode* node)        {};
void FunctionCollectorVisitor::visit(MemberCall* node)        {};
void FunctionCollectorVisitor::visit(DestructiveAssignNode* node) {};
void FunctionCollectorVisitor::visit(TypeInstantiation* node)    {}    ;

