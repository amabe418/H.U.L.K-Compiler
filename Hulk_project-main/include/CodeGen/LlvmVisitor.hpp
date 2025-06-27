#pragma once
#include "../../common/AstNodes.hpp"
#include "CodeGenerationContext.hpp"


class LlvmVisitor:public Visitor
{
public:
    CodeGenerationContext &cgContext;
    llvm::Value* lastValue;
    llvm::Function* currentFunction;
    std::string currentMethodName;

    LlvmVisitor(CodeGenerationContext &cgContext)
    : cgContext(cgContext),lastValue(nullptr){}

    void visit(ProgramNode* node)     ;
    void visit(TypeNode* node)            ;
    void visit(BlockNode* node)            ;

    void visit(BinaryExpression* node)     ;
    void visit(LiteralNode* node)          ;
    void visit(IdentifierNode* node)    ;
    void visit(AttributeNode* node)          ;
    void visit(MethodNode* node)        ;
    void visit(IfExpression* node)          ;
    void visit(WhileExpression* node)       ;
    void visit(ForExression* node)          ;
    void visit(LetExpression* node)         ;
    void visit(UnaryExpression* node)       ;    
    void visit(FunCallNode* node)            ;
    void visit(MemberCall* node)        ;
    void visit(DestructiveAssignNode* node)        ;
    void visit(TypeInstantiation* node)        ;

};