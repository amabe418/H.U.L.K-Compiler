#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../AST/ast.hpp"
#include "../Types/type_checker.hpp"

class TypedAstPrinter : public ExprVisitor, public StmtVisitor
{
   private:
    int indentLevel;
    // NOLINTNEXTLINE(clang-diagnostic-unused-private-field)
    TypeChecker& typeChecker;
    std::string
    indent() const
    {
        return std::string(indentLevel * 2, ' ');
    }

   public:
    TypedAstPrinter(TypeChecker& tc) : indentLevel(0), typeChecker(tc) {}

    void
    visit(NumberExpr* expr) override
    {
        std::cout << indent() << "Number: " << expr->value;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(StringExpr* expr) override
    {
        std::cout << indent() << "String: \"" << expr->value << "\"";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(BooleanExpr* expr) override
    {
        std::cout << indent() << "Boolean: " << (expr->value ? "true" : "false");
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(VariableExpr* expr) override
    {
        std::cout << indent() << "Variable: " << expr->name;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(UnaryExpr* expr) override
    {
        std::cout << indent() << "Unary: " << expr->op;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        expr->operand->accept(this);
        indentLevel--;
    }

    void
    visit(BinaryExpr* expr) override
    {
        std::string opStr;
        switch (expr->op)
        {
            case BinaryExpr::OP_ADD:
                opStr = "+";
                break;
            case BinaryExpr::OP_SUB:
                opStr = "-";
                break;
            case BinaryExpr::OP_MUL:
                opStr = "*";
                break;
            case BinaryExpr::OP_DIV:
                opStr = "/";
                break;
            case BinaryExpr::OP_POW:
                opStr = "^";
                break;
            case BinaryExpr::OP_MOD:
                opStr = "%";
                break;
            case BinaryExpr::OP_LT:
                opStr = "<";
                break;
            case BinaryExpr::OP_GT:
                opStr = ">";
                break;
            case BinaryExpr::OP_LE:
                opStr = "<=";
                break;
            case BinaryExpr::OP_GE:
                opStr = ">=";
                break;
            case BinaryExpr::OP_EQ:
                opStr = "==";
                break;
            case BinaryExpr::OP_NEQ:
                opStr = "!=";
                break;
            case BinaryExpr::OP_OR:
                opStr = "||";
                break;
            case BinaryExpr::OP_AND:
                opStr = "&&";
                break;
            case BinaryExpr::OP_CONCAT:
                opStr = "@";
                break;
            case BinaryExpr::OP_CONCAT_WS:
                opStr = "@@";
                break;
            default:
                opStr = "?";
                break;
        }

        std::cout << indent() << "Binary: " << opStr;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        expr->left->accept(this);
        expr->right->accept(this);
        indentLevel--;
    }

    void
    visit(AssignExpr* expr) override
    {
        std::cout << indent() << "Assign: " << expr->name;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        expr->value->accept(this);
        indentLevel--;
    }

    void
    visit(LetExpr* expr) override
    {
        std::cout << indent() << "Let: " << expr->name;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        expr->initializer->accept(this);
        expr->body->accept(this);
        indentLevel--;
    }

    void
    visit(IfExpr* expr) override
    {
        std::cout << indent() << "If";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        expr->condition->accept(this);
        expr->thenBranch->accept(this);
        if (expr->elseBranch)
        {
            expr->elseBranch->accept(this);
        }
        indentLevel--;
    }

    void
    visit(ExprBlock* expr) override
    {
        std::cout << indent() << "Block";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& stmt : expr->stmts)
        {
            stmt->accept(this);
        }
        indentLevel--;
    }

    void
    visit(CallExpr* expr) override
    {
        std::cout << indent() << "Call: " << expr->callee;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& arg : expr->args)
        {
            arg->accept(this);
        }
        indentLevel--;
    }

    void
    visit(WhileExpr* expr) override
    {
        std::cout << indent() << "While";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        expr->condition->accept(this);
        expr->body->accept(this);
        indentLevel--;
    }

    void
    visit(NewExpr* expr) override
    {
        std::cout << indent() << "New: " << expr->typeName;
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& arg : expr->args)
        {
            arg->accept(this);
        }
        indentLevel--;
    }

    void
    visit(GetAttrExpr* expr) override
    {
        std::cout << indent() << "GetAttr(";
        expr->object->accept(this);
        std::cout << "." << expr->attrName << ")";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(SetAttrExpr* expr) override
    {
        std::cout << indent() << "SetAttr(";
        expr->object->accept(this);
        std::cout << "." << expr->attrName << " := ";
        expr->value->accept(this);
        std::cout << ")";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(MethodCallExpr* expr) override
    {
        std::cout << indent() << "MethodCall(";
        expr->object->accept(this);
        std::cout << "." << expr->methodName << "(";
        for (size_t i = 0; i < expr->args.size(); i++)
        {
            expr->args[i]->accept(this);
            if (i < expr->args.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << "))";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(SelfExpr* expr) override
    {
        std::cout << indent() << "self";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
    }

    void
    visit(BaseCallExpr* expr) override
    {
        std::cout << indent() << "BaseCall";
        if (expr->inferredType)
        {
            std::cout << " : " << expr->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& arg : expr->args)
        {
            arg->accept(this);
        }
        indentLevel--;
    }

    void
    visit(ExprStmt* stmt) override
    {
        std::cout << indent() << "ExprStmt";
        if (stmt->inferredType)
        {
            std::cout << " : " << stmt->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        stmt->expr->accept(this);
        indentLevel--;
    }

    void
    visit(Program* stmt) override
    {
        std::cout << indent() << "Program";
        if (stmt->inferredType)
        {
            std::cout << " : " << stmt->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& s : stmt->stmts)
        {
            s->accept(this);
        }
        indentLevel--;
    }

    void
    visit(FunctionDecl* stmt) override
    {
        std::cout << indent() << "Function: " << stmt->name;
        if (stmt->inferredType)
        {
            std::cout << " : " << stmt->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& param : stmt->params)
        {
            std::cout << indent() << "Param: " << param << std::endl;
        }
        stmt->body->accept(this);
        indentLevel--;
    }

    void
    visit(TypeDecl* stmt) override
    {
        std::cout << indent() << "Type: " << stmt->name;
        if (stmt->baseType != "Object")
        {
            std::cout << " inherits " << stmt->baseType;
        }
        if (stmt->inferredType)
        {
            std::cout << " : " << stmt->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& attr : stmt->attributes)
        {
            std::cout << indent() << "Attribute: " << attr->name << std::endl;
        }
        for (const auto& method : stmt->methods)
        {
            std::cout << indent() << "Method: " << method->name << std::endl;
        }
        indentLevel--;
    }

    void
    visit(MethodDecl* stmt) override
    {
        std::cout << indent() << "Method: " << stmt->name;
        if (stmt->inferredType)
        {
            std::cout << " : " << stmt->inferredType->toString();
        }
        std::cout << std::endl;
        indentLevel++;
        for (const auto& param : stmt->params)
        {
            std::cout << indent() << "Param: " << param << std::endl;
        }
        stmt->body->accept(this);
        indentLevel--;
    }
};
