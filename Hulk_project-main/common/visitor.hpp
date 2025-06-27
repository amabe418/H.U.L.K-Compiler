
class ProgramNode;
class TypeNode;
class BlockNode;
class BinaryExpression;
class LiteralNode;
class IdentifierNode;
class AttributeNode;
class MethodNode;
class IfExpression;
class WhileExpression;
class ForExression;
class LetExpression;
class UnaryExpression;
class FunCallNode;
class MemberCall;
class DestructiveAssignNode ;
class TypeInstantiation;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(ProgramNode* node)       = 0;
    virtual void visit(TypeNode* node)          = 0;
    virtual void visit(BlockNode* node)         = 0;
    virtual void visit(BinaryExpression* node)  = 0;
    virtual void visit(LiteralNode* node)         = 0;
    virtual void visit(IdentifierNode* node)      = 0;
    virtual void visit(AttributeNode* node)        = 0;
    virtual void visit(MethodNode* node)          = 0;
    virtual void visit(IfExpression* node)        = 0;
    virtual void visit(WhileExpression* node)     = 0;
    virtual void visit(ForExression* node)        = 0;
    virtual void visit(LetExpression* node)         = 0;
    virtual void visit(UnaryExpression* node)         = 0;
    virtual void visit(FunCallNode* node)        = 0;
    virtual void visit(MemberCall* node)        = 0;
    virtual void visit(DestructiveAssignNode* node)        = 0;
    virtual void visit(TypeInstantiation* node)        = 0;
};