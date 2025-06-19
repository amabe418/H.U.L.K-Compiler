#ifndef PRINT_VISITOR_HPP
#define PRINT_VISITOR_HPP

#include <iostream>

#include "../AST/ast.hpp"

struct PrintVisitor : StmtVisitor, ExprVisitor
{
    int indentLevel = 0;

    void
    printIndent()
    {
        for (int i = 0; i < indentLevel; ++i)
            std::cout << "    "; // 4 espacios por nivel
    }

    // StmtVisitor:
    void
    visit(Program *p) override
    {
        std::cout << "Program\n";
        indentLevel++;
        for (auto &s : p->stmts)
            s->accept(this);
        indentLevel--;
    }

    void
    visit(ExprStmt *e) override
    {
        printIndent();
        std::cout << "|- ExprStmt\n";
        indentLevel++;
        e->expr->accept(this);
        indentLevel--;
    }

    // ExprVisitor
    void
    visit(NumberExpr *expr) override
    {
        printIndent();
        std::cout << "|_ " << expr->value << " " << expr->inferredType->toString() << std::endl;
    }

    void
    visit(StringExpr *expr) override
    {
        printIndent();
        std::cout << "|_ \"" << expr->value << " " << expr->inferredType->toString() << std::endl;
    }

    void
    visit(BooleanExpr *expr) override
    {
        printIndent();
        std::cout << "|_ Boolean: " << expr->inferredType->toString() << "\n";
    }

    void
    visit(UnaryExpr *expr) override
    {
        printIndent();
        std::cout << "|_ UnaryOp: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->operand->accept(this);
        indentLevel--;
    }

    void
    visit(BinaryExpr *expr) override
    {
        printIndent();
        std::cout << "|_ BinaryOp: " << opToString(expr->op) << " " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->left->accept(this);
        expr->right->accept(this);
        indentLevel--;
    }

    void
    visit(CallExpr *expr) override
    {
        printIndent();
        if (!expr->inferredType)
        {
            std::cout << "|_ Call: " << expr->callee << " (tipo desconocido)\n";
        }
        else
        {
            std::cout << "|_ Call: " << expr->callee << " " << expr->inferredType->toString() << "\n";
        }
        indentLevel++;
        for (const auto &arg : expr->args)
        {
            if (arg)
            {
                arg->accept(this);
            }
        }
        indentLevel--;
    }

    void
    visit(VariableExpr *expr) override
    {
        printIndent();
        if (!expr->inferredType)
        {
            std::cout << "|_ Variable: " << expr->name << " (tipo desconocido)\n";
        }
        else
        {
            std::cout << "|_ Variable: " << expr->name << " " << expr->inferredType->toString() << "\n";
        }
    }

    void
    visit(LetExpr *expr) override
    {
        printIndent();
        if (!expr->inferredType)
        {
        }
        std::cout << "|_ LetExpr: " << expr->name << " " << expr->body->inferredType->toString() << "\n";
        indentLevel++;

        // 1) Mostrar subárbol del inicializador
        printIndent();
        std::cout << "|_ Initializer: " << expr->initializer->inferredType->toString() << "\n";
        indentLevel++;
        expr->initializer->accept(this);
        indentLevel--;

        // 2) Mostrar subárbol del cuerpo
        printIndent();
        std::cout << "|_ Body: " << expr->body->inferredType->toString() << "\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(AssignExpr *expr) override
    {
        printIndent();
        std::cout << "|_ AssignExpr: " << expr->name << " " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->value->accept(this);
        indentLevel--;
    }

    void
    visit(FunctionDecl *f) override
    {
        printIndent();
        std::cout << "|_FunctionDecl: " << f->name << " " << f->returnType->toString() << "\n";
        indentLevel++;
        for (int i = 0; i < f->paramTypes.size(); i++)
        {
            printIndent();
            std::cout << "|_ Param: " << f->params[i] << " " << f->paramTypes[i]->toString() << "\n";
        }
        f->body->accept(this);
        indentLevel--;
    }

    void
    visit(IfExpr *expr) override
    {
        printIndent();
        std::cout << "|_ IfExpr " << expr->inferredType->toString() << "\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Condition: " << expr->condition->inferredType->toString() << "\n";
        indentLevel++;
        expr->condition->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Then: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->thenBranch->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Else: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->elseBranch->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(ExprBlock *b) override
    {
        printIndent();
        std::cout << "|_ ExprBlock " << b->inferredType->toString() << "\n";
        indentLevel++;
        for (auto &stmt : b->stmts)
            stmt->accept(this);
        indentLevel--;
    }

    void
    visit(WhileExpr *expr) override
    {
        printIndent();
        std::cout << "|_ WhileExpr " << expr->inferredType->toString() << "\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Condition: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->condition->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Body: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(ForExpr *expr) override
    {
        printIndent();
        std::cout << "|_ ForExpr " << expr->inferredType->toString() << "\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Variable: " << expr->variable << "\n";

        printIndent();
        std::cout << "|_ Iterable: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->iterable->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Body: " << expr->inferredType->toString() << "\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(TypeDecl *t) override
    {
        printIndent();
        std::cout << "|_ TypeDecl: " << t->name << " " << t->inferredType->toString() << "\n";
        indentLevel++;

        for (auto &attr : t->attributes)
        {
            printIndent();
            std::cout << "|_ Attribute: " << attr->name << " " << attr->inferredType->toString() << "\n";
        }

        for (auto &method : t->methods)
        {
            method->accept(this);
        }
        indentLevel--;
    }

    void
    visit(MethodDecl *m) override
    {
        printIndent();
        std::cout << "|_ Method: " << m->name << " " << m->inferredType->toString() << "\n";
        indentLevel++;
        m->body->accept(this);
        indentLevel--;
    }

    void
    visit(NewExpr *e) override
    {
        printIndent();
        std::cout << "|_ New: " << e->typeName << " " << e->inferredType->toString() << "\n";
    }

    void
    visit(GetAttrExpr *e) override
    {
        printIndent();
        std::cout << "|_ GetAttr: ." << e->attrName << " " << e->inferredType->toString() << "\n";
        indentLevel++;
        e->object->accept(this);
        indentLevel--;
    }

    void
    visit(SetAttrExpr *e) override
    {
        printIndent();
        std::cout << "|_ SetAttr: ." << e->attrName << " " << e->inferredType->toString() << "\n";
        indentLevel++;
        e->object->accept(this);
        e->value->accept(this);
        indentLevel--;
    }

    void
    visit(MethodCallExpr *e) override
    {
        printIndent();
        std::cout << "|_ MethodCall: " << e->methodName << " " << e->inferredType->toString() << "\n";
        indentLevel++;
        e->object->accept(this);
        for (auto &arg : e->args)
            arg->accept(this);
        indentLevel--;
    }

    void
    visit(SelfExpr *e) override
    {
        printIndent();
        std::cout << "|_ self " << e->inferredType->toString() << "\n";
    }

    void
    visit(BaseCallExpr *e) override
    {
        printIndent();
        std::cout << "|_ Call base() " << e->inferredType->toString() << "\n";
    }
    void visit(AttributeDecl *attr) override
    {
        printIndent();
        std::cout << "|_ Attribute: " << attr->name << " " << attr->inferredType->toString() << "\n";
        indentLevel++;
        attr->initializer->accept(this);
        indentLevel--;
    }

private:
    std::string
    opToString(BinaryExpr::Op op)
    {
        switch (op)
        {
        case BinaryExpr::OP_ADD:
            return "+";
        case BinaryExpr::OP_SUB:
            return "-";
        case BinaryExpr::OP_MUL:
            return "*";
        case BinaryExpr::OP_DIV:
            return "/";
        case BinaryExpr::OP_POW:
            return "^";
        case BinaryExpr::OP_MOD:
            return "%";
        case BinaryExpr::OP_LT:
            return "<";
        case BinaryExpr::OP_GT:
            return ">";
        case BinaryExpr::OP_LE:
            return "<=";
        case BinaryExpr::OP_GE:
            return ">=";
        case BinaryExpr::OP_EQ:
            return "==";
        case BinaryExpr::OP_NEQ:
            return "!=";
        case BinaryExpr::OP_OR:
            return "||";
        case BinaryExpr::OP_AND:
            return "&&";
        case BinaryExpr::OP_CONCAT:
            return "@";
        default:
            return "?";
        }
    }
};

#endif // PRINT_VISITOR_HPP
