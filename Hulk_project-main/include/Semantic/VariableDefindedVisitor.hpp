#include "../../common/AstNodes.hpp"
#include "../../common/Error.hpp"
#include "Context.hpp"

class VariableDefindedVisitor:public Visitor
{

    public:

        Context* context;
        Type* currentType=nullptr;
        bool comingFromLet=false;
        Method* currentMethod=nullptr; 
        ErrorHandler errorHandler;

        VariableDefindedVisitor(Context* context,ErrorHandler& errorHandler)
        :context(context),errorHandler(errorHandler){}

        void visit(ProgramNode* node)   ;
        
        void visit(TypeNode* node) ;

        void visit(AttributeNode* node);
 
        
        void visit(MethodNode* node)         ;

        void visit(LetExpression* node);

        void visit(BlockNode* node)  ;
       
        void visit(BinaryExpression* node)  ;
        
        void visit(IdentifierNode* node)     ; 

          
        void visit(IfExpression* node)    ;

        void visit(WhileExpression* node)   ;
        
       
        void visit(FunCallNode* node) ;

        void visit(MemberCall* node) ;



        void visit(DestructiveAssignNode* node) ;

       
        void visit(TypeInstantiation* node);


        void visit(ForExression* node)      ;

        void visit(UnaryExpression* node)    ;

        void visit(LiteralNode* node) ;
        
};
