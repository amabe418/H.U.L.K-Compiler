#include "../../common/AstNodes.hpp"
#include "../../common/Error.hpp"
#include "Context.hpp"

class FunctionCollectorVisitor: public Visitor
{
    public:
    Context* context;
    ErrorHandler errorHandler;

    FunctionCollectorVisitor( ErrorHandler& errorHandler)
    :errorHandler(errorHandler){}
    
    void visit(ProgramNode* node)   ;
  
    
    void visit(MethodNode* node);
    void visit(AttributeNode* node)  ;


    void visit(TypeNode* node)           ; 
    void visit(BlockNode* node)          ;
    void visit(BinaryExpression* node)   ;
    void visit(LiteralNode* node)        ;
    void visit(IdentifierNode* node)     ; 
    
    void visit(IfExpression* node)       ;
    void visit(WhileExpression* node)    ;
    void visit(ForExression* node)       ;
    void visit(LetExpression* node)      ; 
    void visit(UnaryExpression* node)      ;
    void visit(FunCallNode* node)        ;
    void visit(MemberCall* node)        ;
    void visit(DestructiveAssignNode* node) ;
    void visit(TypeInstantiation* node)        ;

};