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

    // Helper function to get the proper type string
    std::string getTypeString(std::shared_ptr<TypeInfo> type)
    {
        if (!type)
            return "(tipo desconocido)";

        if (type->getKind() == TypeInfo::Kind::Object && !type->getTypeName().empty())
        {
            return type->getTypeName(); // Show the specific object type name
        }

        return type->toString();
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
        std::cout << "|_ " << expr->value << " " << getTypeString(expr->inferredType) << std::endl;
    }

    void
    visit(StringExpr *expr) override
    {
        printIndent();
        std::cout << "|_ \"" << expr->value << " " << getTypeString(expr->inferredType) << std::endl;
    }

    void
    visit(BooleanExpr *expr) override
    {
        printIndent();
        std::cout << "|_ Boolean: " << getTypeString(expr->inferredType) << "\n";
    }

    void
    visit(UnaryExpr *expr) override
    {
        printIndent();
        std::cout << "|_ UnaryOp: " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->operand->accept(this);
        indentLevel--;
    }

    void
    visit(BinaryExpr *expr) override
    {
        printIndent();
        std::cout << "|_ BinaryOp: " << opToString(expr->op) << " " << getTypeString(expr->inferredType) << "\n";
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
            std::cout << "|_ Call: " << expr->callee << " " << getTypeString(expr->inferredType) << "\n";
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
            std::cout << "|_ Variable: " << expr->name << " " << getTypeString(expr->inferredType) << "\n";
        }
    }

    void
    visit(LetExpr *expr) override
    {
        printIndent();
        if (!expr->inferredType)
        {
        }
        std::cout << "|_ LetExpr: " << expr->name << " " << getTypeString(expr->body->inferredType) << "\n";
        indentLevel++;

        // 1) Mostrar subárbol del inicializador
        printIndent();
        std::cout << "|_ Initializer: " << getTypeString(expr->declaredType) << "\n";
        indentLevel++;
        expr->initializer->accept(this);
        indentLevel--;

        // 2) Mostrar subárbol del cuerpo
        printIndent();
        std::cout << "|_ Body: " << getTypeString(expr->body->inferredType) << "\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(AssignExpr *expr) override
    {
        printIndent();
        std::cout << "|_ AssignExpr: " << expr->name << " " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->value->accept(this);
        indentLevel--;
    }

    void
    visit(FunctionDecl *f) override
    {
        printIndent();
        std::cout << "|_FunctionDecl: " << f->name << " " << getTypeString(f->returnType) << "\n";
        indentLevel++;
        for (int i = 0; i < f->paramTypes.size(); i++)
        {
            printIndent();
            std::cout << "|_ Param: " << f->params[i] << " " << getTypeString(f->paramTypes[i]) << "\n";
        }
        f->body->accept(this);
        indentLevel--;
    }

    void
    visit(IfExpr *expr) override
    {
        printIndent();
        std::cout << "|_ IfExpr " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Condition: " << getTypeString(expr->condition->inferredType) << "\n";
        indentLevel++;
        expr->condition->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Then: " << getTypeString(expr->thenBranch->inferredType) << "\n";
        indentLevel++;
        expr->thenBranch->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Else: " << getTypeString(expr->elseBranch->inferredType) << "\n";
        indentLevel++;
        expr->elseBranch->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(ExprBlock *b) override
    {
        printIndent();
        std::cout << "|_ ExprBlock " << getTypeString(b->inferredType) << "\n";
        indentLevel++;
        for (auto &stmt : b->stmts)
            stmt->accept(this);
        indentLevel--;
    }

    void
    visit(WhileExpr *expr) override
    {
        printIndent();
        std::cout << "|_ WhileExpr " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Condition: " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->condition->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Body: " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(ForExpr *expr) override
    {
        printIndent();
        std::cout << "|_ ForExpr " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Variable: " << expr->variable << "\n";

        printIndent();
        std::cout << "|_ Iterable: " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->iterable->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Body: " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(TypeDecl *t) override
    {
        printIndent();
        std::cout << "|_ TypeDecl: " << t->name << " " << getTypeString(t->inferredType) << "\n";
        indentLevel++;

        for (auto &attr : t->attributes)
        {
            printIndent();
            std::cout << "|_ Attribute: " << attr->name << " " << getTypeString(attr->declaredType) << "\n";
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
        std::cout << "|_ Method: " << m->name << " " << getTypeString(m->returnType) << "\n";
        indentLevel++;
        m->body->accept(this);
        indentLevel--;
    }

    void
    visit(NewExpr *e) override
    {
        printIndent();
        std::cout << "|_ New: " << e->typeName << " " << getTypeString(e->inferredType) << "\n";
    }

    void
    visit(GetAttrExpr *e) override
    {
        printIndent();
        std::cout << "|_ GetAttr: ." << e->attrName << " " << getTypeString(e->inferredType) << "\n";
        indentLevel++;
        e->object->accept(this);
        indentLevel--;
    }

    void
    visit(SetAttrExpr *e) override
    {
        printIndent();
        std::cout << "|_ SetAttr: ." << e->attrName << " " << getTypeString(e->inferredType) << "\n";
        indentLevel++;
        e->object->accept(this);
        e->value->accept(this);
        indentLevel--;
    }

    void
    visit(MethodCallExpr *e) override
    {
        printIndent();
        std::cout << "|_ MethodCall: " << e->methodName << " " << getTypeString(e->inferredType) << "\n";
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
        std::cout << "|_ self " << getTypeString(e->inferredType) << "\n";
    }

    void
    visit(BaseCallExpr *e) override
    {
        printIndent();
        std::cout << "|_ Call base() " << getTypeString(e->inferredType) << "\n";
    }
    void visit(AttributeDecl *attr) override
    {
        printIndent();
        std::cout << "|_ Attribute: " << attr->name << " " << getTypeString(attr->declaredType) << "\n";
        indentLevel++;
        attr->initializer->accept(this);
        indentLevel--;
    }

    void visit(IsExpr *expr) override
    {
        printIndent();
        std::cout << "|_ IsExpr: " << expr->typeName << " " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->expr->accept(this);
        indentLevel--;
    }

    void visit(AsExpr *expr) override
    {
        printIndent();
        std::cout << "|_ AsExpr: " << expr->typeName << " " << getTypeString(expr->inferredType) << "\n";
        indentLevel++;
        expr->expr->accept(this);
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
        case BinaryExpr::OP_CONCAT_WS:
            return "@@";
        default:
            return "?";
        }
    }
};

#endif // PRINT_VISITOR_HPP
