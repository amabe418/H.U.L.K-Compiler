#pragma once
#include <string>
#include<vector>
#include <iostream>
#include "visitor.hpp"
#include "../include/Lexer/Token.hpp"
class AstNode {
public:
    virtual ~AstNode() {}
    virtual void print(int indent = 0) const = 0;
    std::string type="";
    virtual std::string getType()=0;
    virtual void setType(std::string type)=0;
    virtual void accept(Visitor &visitor)=0;
};

class Expression : public AstNode {
public:
    virtual ~Expression() {}
    virtual void accept(Visitor &visitor) = 0;
    virtual std::string getType()=0;
    virtual void setType(std::string type)=0;

};

class BlockNode : public AstNode {
public:
    std::vector<AstNode*> exprs;
    BlockNode(std::vector<AstNode*> exprs)
        : exprs(exprs) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Block" << "\n";
        for (auto exp : exprs) {
            exp->print(indent + 2);
        }
    }
    std::string getType(){return "";};
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string type){};


};

class ProgramNode : public AstNode {
public:
    std::vector<AstNode*> stmts;
    ProgramNode() {};

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Program" << "\n";
        for (auto exp : stmts) {
            exp->print(indent + 2);
        }
    }

    std::string getType(){return "";};
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string type){};

};

class AttributeNode : public AstNode {
public:
    Token id;
    AstNode* expression;

    AttributeNode(Token id, AstNode* expression)
        : id(id), expression(expression) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Assignment: " << id.lexeme << " = " << std::endl;
        expression->print(indent + 2);
    }
    std::string getType()override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}

};

class MethodNode : public AstNode {
public:
    Token id;
    std::vector<AstNode*> params;
    AstNode* body;
    
    MethodNode(Token id, std::vector<AstNode*> params, AstNode* body)
        : id(id), params(params), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "MethodNode " << id.lexeme << ":" << std::endl;
        std::cout << spacing << "  Params:" << std::endl;
        for (const auto& param : params) {
            param->print(indent + 4);
        }
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    
    void setType(std::string t){type=t;}
};

class TypeNode : public AstNode {
public:
    Token name;
    std::vector<AstNode*> atributes;
    std::vector<AstNode*> methods;
    std::vector<AstNode*> args;
    Token parentName;
    std::vector<AstNode*> parent_args;


    TypeNode(Token name, std::vector<AstNode*> atributes, std::vector<AstNode*> methods,std::vector<AstNode*> args)
        : name(name), atributes(atributes), methods(methods),args(args) {
        
    }

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Type Expression: " << name.lexeme << std::endl;
        if(parentName.lexeme!="")
        {
            std::cout << spacing << "Parent: " << parentName.lexeme << std::endl;
            std::cout << spacing << "ParentArgs: " << std::endl;
            for (const auto& arg : parent_args) {
            arg->print(indent + 4);
        }
        }
        std::cout << spacing << "Arguments: " << std::endl;
        for (const auto& arg : args) {
            arg->print(indent + 4);
        }
        std::cout << spacing << " Atributes:" << std::endl;
        for (const auto& atrib : atributes) {
            atrib->print(indent + 4);
        }
        std::cout << spacing << " Methods:" << std::endl;
        for (const auto& meth : methods) {
            meth->print(indent + 4);
        }
    }
    std::string getType(){return "";};
    void setType(std::string t){}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
};

class BinaryExpression : public Expression {
public:
    AstNode* left;
    Token op;
    AstNode* right;

    BinaryExpression(AstNode* left, Token op, AstNode* right)
        : left(left), op(op), right(right) {type="Number";}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "AstBinaryExp (" << op.lexeme << "):" << "\n";
        if (left) left->print(indent + 2);
        if (right) right->print(indent + 2);
    }
    std::string getType()override  {return type;}
    
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class LiteralNode : public Expression {
public:
    Token value;
  
    LiteralNode(Token value)
        : value(value) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << value.lexeme << "\n";
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class IdentifierNode : public Expression {
public:
    Token value;
    
    IdentifierNode(Token value)
        : value(value) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << value.lexeme << "\n";
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class IfExpression : public AstNode {
public:
    AstNode* defaultExp;
    std::vector<std::pair<AstNode*, AstNode*>> exprs_cond;
    Token id;
 

    IfExpression(Token id,AstNode* defaultExp, std::vector<std::pair<AstNode*, AstNode*>> exprs_cond)
        :id(id) ,defaultExp(defaultExp), exprs_cond(exprs_cond) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "If Expression:" << std::endl;
        for (const auto& exp_cond : exprs_cond) {
            std::cout << spacing << "  Condition:" << std::endl;
            exp_cond.first->print(indent + 4);
            std::cout << spacing << "  Exp:" << std::endl;
            exp_cond.second->print(indent + 4);
        }
        std::cout << spacing << "  Default:" << std::endl;
        defaultExp->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class WhileExpression : public AstNode {
public:
    AstNode* condition;
    AstNode* body;
  

    WhileExpression(AstNode* condition, AstNode* body)
        : condition(condition), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "While Expression:" << std::endl;
        std::cout << spacing << "  Condition:" << std::endl;
        condition->print(indent + 4);
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class ForExression : public AstNode {
public:
    AstNode* item;
    AstNode* iterable;
    AstNode* body;
   
    ForExression(AstNode* item, AstNode* iterable, AstNode* body)
        : item(item), iterable(iterable), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "For Expression:" << std::endl;
        std::cout << spacing << "  Item:" << std::endl;
        item->print(indent + 4);
        std::cout << spacing << "  Iterable:" << std::endl;
        iterable->print(indent + 4);
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class LetExpression : public Expression {
public:
    std::vector<AstNode*> assignments;
    AstNode* body;
  

    LetExpression(const std::vector<AstNode*>& assignments, AstNode* body)
        : assignments(assignments), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Let Expression:" << std::endl;
        std::cout << spacing << "  Assignments:" << std::endl;
        for (const auto& assign : assignments) {
            assign->print(indent + 4);
        }
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType()  override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class UnaryExpression :public Expression
{
    public:
    Token value;
    AstNode* exp;

        UnaryExpression(Token value,AstNode* exp)
        : value(value),exp(exp){}


         void print(int indent = 0) const override 
         {
            std::string spacing(indent, ' ');
            std::cout << spacing << "Unary Expression: " <<value.lexeme<< std::endl;
            std::cout << spacing << "  exp:" << std::endl;
            exp->print(indent + 4);
         }
        std::string getType()  override  {return type;}
    

        void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
};

class FunCallNode : public Expression {
public:
    Token id; 
    std::vector<AstNode*> arguments; 
  
    FunCallNode(const Token& id, const std::vector<AstNode*>& arguments)
        : id(id), arguments(arguments) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "FunCall: " << id.lexeme <<std::endl ;
         std::cout << spacing << "Arg: " <<std::endl  ;
        for (size_t i = 0; i < arguments.size(); ++i) {
            arguments[i]->print(indent+4);
        }
     
    }

    std::string getType() override {
        return type;
    }

    void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
};

class MemberCall:public AstNode
{
    public:
    AstNode* obj;
    AstNode* member;

    MemberCall(AstNode* obj,AstNode* member)
    : obj(obj),member(member)
    {}


     void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "MemberCall: "  <<std::endl ;
        std::cout << spacing << "object: " <<std::endl ;
        obj->print(indent+4);
        std::cout << spacing << "Member: " <<std::endl  ;
        member->print(indent+4);
    }

    std::string getType() override {
        return type;
    }

    void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
};

class DestructiveAssignNode : public AstNode {
public:
    AstNode* lhs;  // Lo que está a la izquierda de :=
    Token op_des;
    AstNode* rhs;  // Lo que está a la derecha de :=

 

    DestructiveAssignNode(AstNode* lhs,Token op, AstNode* rhs)
        : lhs(lhs),op_des(op), rhs(rhs) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "DestructiveAssignNode :=" << std::endl;
        std::cout << spacing << "  LHS:" << std::endl;
        lhs->print(indent + 4);
        std::cout << spacing << "  RHS:" << std::endl;
        rhs->print(indent + 4);
    }

    std::string getType() override { return type; }

    void setType(std::string t) { type = t; }

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

class TypeInstantiation:public AstNode
{
    public:
    Token typeName; 
    std::vector<AstNode*> arguments; 
   

    TypeInstantiation(const Token& id, const std::vector<AstNode*>& arguments)
        : typeName(id), arguments(arguments) {type=id.lexeme;}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "TypeInstatiation: " << typeName.lexeme <<std::endl ;
         std::cout << spacing << "Arg: " <<std::endl  ;
        for (size_t i = 0; i < arguments.size(); ++i) {
            arguments[i]->print(indent+4);
        }
     
    }

    std::string getType() override {
        return type;
    }

    void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
        
};