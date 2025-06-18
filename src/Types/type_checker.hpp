// type_checker.hpp
#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../AST/ast.hpp"
#include "../SymbolCollector/symbol_collector.hpp"
#include "../Symbols/symbol.hpp"
#include "../Symbols/symbol_table.hpp"
#include "type_info.hpp"

class TypeChecker : public ExprVisitor, public StmtVisitor
{
   public:
    TypeChecker() : symbols(SymbolTable::getInstance())
    {
        // Agregar funciones built-in
        addBuiltinFunction("print", {TypeInfo::Any()}, TypeInfo::Void());
        addBuiltinFunction("sqrt", {TypeInfo::Float()}, TypeInfo::Float());
        addBuiltinFunction("sin", {TypeInfo::Float()}, TypeInfo::Float());
        addBuiltinFunction("cos", {TypeInfo::Float()}, TypeInfo::Float());
        addBuiltinFunction("exp", {TypeInfo::Float()}, TypeInfo::Float());
        addBuiltinFunction("log", {TypeInfo::Float(), TypeInfo::Float()}, TypeInfo::Float());
        addBuiltinFunction("rand", {}, TypeInfo::Float());
    }

    bool
    check(Program* prog)
    {
        errors.clear();  // Clear any previous errors
        try
        {
            prog->accept(this);
            return errors.empty();
        }
        catch (const std::exception& e)
        {
            errors.push_back(std::string("Type error: ") + e.what());
            return false;
        }
    }

    const std::vector<std::string>&
    getErrors() const
    {
        return errors;
    }

    void
    error(const std::string& msg)
    {
        errors.push_back(msg);
    }

    std::shared_ptr<TypeInfo>
    getType(Expr* expr)
    {
        expr->accept(this);
        return currentType;
    }

    std::shared_ptr<TypeInfo>
    getType(Stmt* stmt)
    {
        stmt->accept(this);
        return currentType;
    }

    void
    visit(NumberExpr* expr) override
    {
        expr->inferredType = TypeInfo::Int();
    }
    void
    visit(StringExpr* expr) override
    {
        expr->inferredType = TypeInfo::String();
    }
    void
    visit(BooleanExpr* expr) override
    {
        expr->inferredType = TypeInfo::Bool();
    }

    void
    visit(VariableExpr* expr) override
    {
        if (auto s = symbols.lookup(expr->name))
        {
            expr->inferredType = s->type;
        }
        else
        {
            reportError("Undefined variable: " + expr->name);
            expr->inferredType = TypeInfo::Error();
        }
    }
    void
    visit(UnaryExpr* expr) override
    {
        std::cout << "[DEBUG] Processing UnaryExpr with operator: " << expr->op << std::endl;
        auto operandType = getType(expr->operand.get());
        std::cout << "[DEBUG] Operand type: " << operandType->toString() << std::endl;

        switch (expr->op)
        {
            case UnaryExpr::OP_NEG:
                if (operandType->kind == TypeInfo::Kind::Int ||
                    operandType->kind == TypeInfo::Kind::Float)
                {
                    currentType = operandType;
                    std::cout << "[DEBUG] Valid unary operation, result type: "
                              << currentType->toString() << std::endl;
                }
                else
                {
                    std::cout << "[DEBUG] Invalid unary operation: operand must be numeric"
                              << std::endl;
                    reportError("Unary '-' operand must be numeric");
                    currentType = TypeInfo::Error();
                }
                break;
        }
    }

    void
    visit(BinaryExpr* expr) override
    {
        expr->left->accept(this);
        expr->right->accept(this);

        auto l = expr->left->inferredType;
        auto r = expr->right->inferredType;

        switch (expr->op)
        {
            case BinaryExpr::OP_ADD:
            case BinaryExpr::OP_SUB:
            case BinaryExpr::OP_MUL:
            case BinaryExpr::OP_DIV:
            case BinaryExpr::OP_MOD:
            case BinaryExpr::OP_POW:
                if (l->kind == TypeInfo::Kind::Any || r->kind == TypeInfo::Kind::Any)
                {
                    currentType = TypeInfo::Int();
                    std::cout << "[DEBUG] Any type detected, defaulting to Int" << std::endl;
                }
                else if (isNumeric(l) && isNumeric(r))
                {
                    currentType = promoteNumeric(l, r);
                    std::cout << "[DEBUG] Numeric operation, result type: "
                              << currentType->toString() << std::endl;
                }
                else
                {
                    std::cout << "[DEBUG] Invalid arithmetic operation: operands must be numeric"
                              << std::endl;
                    reportError("Arithmetic operations require numeric operands");
                    currentType = TypeInfo::Error();
                }
                break;
            case BinaryExpr::OP_EQ:
            case BinaryExpr::OP_NEQ:
            case BinaryExpr::OP_LT:
            case BinaryExpr::OP_LE:
            case BinaryExpr::OP_GT:
            case BinaryExpr::OP_GE:
                if (l->kind == TypeInfo::Kind::Any || r->kind == TypeInfo::Kind::Any)
                {
                    currentType = TypeInfo::Bool();
                }
                else if (*l == *r)
                {
                    currentType = TypeInfo::Bool();
                }
                else
                {
                    reportError("Comparison requires operands of same type");
                    currentType = TypeInfo::Error();
                }
                break;
            case BinaryExpr::OP_AND:
            case BinaryExpr::OP_OR:
                if (l->kind == TypeInfo::Kind::Any || r->kind == TypeInfo::Kind::Any)
                {
                    currentType = TypeInfo::Bool();
                }
                else if (l->kind == TypeInfo::Kind::Bool && r->kind == TypeInfo::Kind::Bool)
                {
                    currentType = TypeInfo::Bool();
                }
                else
                {
                    reportError("Logical operators require boolean operands");
                    currentType = TypeInfo::Error();
                }
                break;
            case BinaryExpr::OP_CONCAT:
            case BinaryExpr::OP_CONCAT_WS:
                if (l->kind == TypeInfo::Kind::Any || r->kind == TypeInfo::Kind::Any)
                {
                    currentType = TypeInfo::String();
                }
                else if (l->kind == TypeInfo::Kind::String && r->kind == TypeInfo::Kind::String)
                {
                    currentType = TypeInfo::String();
                }
                else
                {
                    reportError("Concatenation requires string operands");
                    currentType = TypeInfo::Error();
                }
                break;
        }
    }

    void
    visit(AssignExpr* expr) override
    {
        std::cout << "[DEBUG] Processing AssignExpr for variable: " << expr->name << std::endl;
        if (expr->name == "self")
        {
            std::cout << "[DEBUG] Attempt to reassign 'self'" << std::endl;
            reportError("Cannot reassign 'self'");
            currentType = TypeInfo::Error();
            return;
        }

        auto valueType = getType(expr->value.get());
        std::cout << "[DEBUG] Value type: " << valueType->toString() << std::endl;

        if (auto varType = symbols.lookup(expr->name))
        {
            std::cout << "[DEBUG] Found variable with type: " << varType->type->toString()
                      << std::endl;
            if (!varType->isMutable)
            {
                std::cout << "[DEBUG] Variable is immutable" << std::endl;
                reportError("Cannot reassign immutable variable: " + expr->name);
                currentType = TypeInfo::Error();
                return;
            }
            if (*varType->type != *valueType)
            {
                std::cout << "[DEBUG] Type mismatch in assignment" << std::endl;
                reportError("Type mismatch in assignment");
                currentType = TypeInfo::Error();
                return;
            }
            currentType = valueType;
            std::cout << "[DEBUG] Assignment successful, result type: " << currentType->toString()
                      << std::endl;
        }
        else
        {
            std::cout << "[DEBUG] Variable not found in symbol table" << std::endl;
            reportError("Undefined variable: " + expr->name);
            currentType = TypeInfo::Error();
        }
    }

    void
    visit(LetExpr* expr) override
    {
        std::cout << "[DEBUG] Processing LetExpr for variable: " << expr->name << std::endl;

        // Primero evaluar el inicializador para obtener su tipo
        expr->initializer->accept(this);
        auto initType = currentType;
        std::cout << "[DEBUG] Initializer type: " << initType->toString() << std::endl;

        // Registrar la variable con el tipo inferido
        Symbol sym;
        sym.name = expr->name;
        sym.kind = SymbolKind::Variable;
        sym.type = initType;
        sym.isMutable = true;
        symbols.define(expr->name, sym);
        std::cout << "[DEBUG] Variable defined with type: " << initType->toString() << std::endl;

        // Evaluar el cuerpo con la variable en scope
        currentType = getType(expr->body.get());
        std::cout << "[DEBUG] Body type: " << currentType->toString() << std::endl;
    }

    void
    visit(IfExpr* expr) override
    {
        std::cout << "[DEBUG] Processing IfExpr" << std::endl;
        auto condType = getType(expr->condition.get());
        std::cout << "[DEBUG] Condition type: " << condType->toString() << std::endl;

        if (condType->kind != TypeInfo::Kind::Bool)
        {
            std::cout << "[DEBUG] Invalid condition type: must be boolean" << std::endl;
            reportError("If condition must be boolean");
            currentType = TypeInfo::Error();
            return;
        }

        auto thenType = getType(expr->thenBranch.get());
        std::cout << "[DEBUG] Then branch type: " << thenType->toString() << std::endl;

        if (expr->elseBranch)
        {
            auto elseType = getType(expr->elseBranch.get());
            std::cout << "[DEBUG] Else branch type: " << elseType->toString() << std::endl;

            if (*thenType != *elseType)
            {
                std::cout << "[DEBUG] Branch type mismatch" << std::endl;
                reportError("If branches must have same type");
                currentType = TypeInfo::Error();
                return;
            }
        }
        currentType = thenType;
        std::cout << "[DEBUG] If expression type: " << currentType->toString() << std::endl;
    }

    void
    visit(ExprBlock* expr) override
    {
        std::cout << "[DEBUG] Processing ExprBlock" << std::endl;
        std::shared_ptr<TypeInfo> lastType;
        for (const auto& stmt : expr->stmts)
        {
            lastType = getType(stmt.get());
            std::cout << "[DEBUG] Statement type: " << lastType->toString() << std::endl;
        }
        currentType = lastType;
        std::cout << "[DEBUG] Block type: " << currentType->toString() << std::endl;
    }

    void
    visit(CallExpr* expr) override
    {
        std::cout << "[DEBUG] Processing CallExpr for function: " << expr->callee << std::endl;

        // Obtener el tipo de la función
        if (auto funcType = symbols.lookup(expr->callee))
        {
            std::cout << "[DEBUG] Found function with type: " << funcType->type->toString()
                      << std::endl;

            if (funcType->type->isFunction())
            {
                // Procesar los argumentos para inferir sus tipos
                std::vector<std::shared_ptr<TypeInfo>> argTypes;
                for (const auto& arg : expr->args)
                {
                    arg->accept(this);
                    argTypes.push_back(currentType);
                    std::cout << "[DEBUG] Argument type: " << currentType->toString() << std::endl;
                }

                // Actualizar los tipos de los parámetros basado en los argumentos
                auto paramTypes = funcType->type->getParamTypes();
                for (size_t i = 0; i < paramTypes.size() && i < argTypes.size(); ++i)
                {
                    if (paramTypes[i]->kind == TypeInfo::Kind::Any)
                    {
                        paramTypes[i] = argTypes[i];
                        std::cout << "[DEBUG] Updated parameter type to: "
                                  << argTypes[i]->toString() << std::endl;
                    }
                }

                // Actualizar el tipo de la función
                funcType->type = TypeInfo::Function(paramTypes, funcType->type->getReturnType());
                std::cout << "[DEBUG] Updated function type: " << funcType->type->toString()
                          << std::endl;

                // El tipo de retorno de la llamada es el tipo de retorno de la función
                currentType = funcType->type->getReturnType();
                std::cout << "[DEBUG] Call type: " << currentType->toString() << std::endl;
            }
            else
            {
                std::cout << "[DEBUG] Symbol is not a function" << std::endl;
                reportError(expr->callee + " is not a function");
                currentType = TypeInfo::Error();
            }
        }
        else
        {
            std::cout << "[DEBUG] Function not found in symbol table" << std::endl;
            reportError("Undefined function: " + expr->callee);
            currentType = TypeInfo::Error();
        }
    }

    void
    visit(WhileExpr* expr) override
    {
        std::cout << "[DEBUG] Processing WhileExpr" << std::endl;
        auto condType = getType(expr->condition.get());
        std::cout << "[DEBUG] Condition type: " << condType->toString() << std::endl;

        if (condType->kind != TypeInfo::Kind::Bool)
        {
            std::cout << "[DEBUG] Invalid condition type: must be boolean" << std::endl;
            reportError("While condition must be boolean");
            currentType = TypeInfo::Error();
            return;
        }

        getType(expr->body.get());
        currentType = TypeInfo::Void();
        std::cout << "[DEBUG] While expression type: " << currentType->toString() << std::endl;
    }

    void
    visit(NewExpr* expr) override
    {
        std::cout << "[DEBUG] Processing NewExpr for type: " << expr->typeName << std::endl;
        if (auto type = symbols.lookup(expr->typeName))
        {
            std::cout << "[DEBUG] Found type: " << type->type->toString() << std::endl;
            if (type->type->isClass())
            {
                std::vector<std::shared_ptr<TypeInfo>> paramTypes = type->type->getParamTypes();
                std::vector<std::shared_ptr<TypeInfo>> argTypes;

                std::cout << "[DEBUG] Expected " << paramTypes.size() << " constructor parameters"
                          << std::endl;
                std::cout << "[DEBUG] Got " << expr->args.size() << " arguments" << std::endl;

                for (const auto& arg : expr->args)
                {
                    auto argType = getType(arg.get());
                    argTypes.push_back(argType);
                    std::cout << "[DEBUG] Argument type: " << argType->toString() << std::endl;
                }

                for (size_t i = 0; i < paramTypes.size(); i++)
                {
                    if (paramTypes[i]->kind != TypeInfo::Kind::Any &&
                        paramTypes[i]->kind != argTypes[i]->kind)
                    {
                        std::cout << "[DEBUG] Type mismatch in constructor argument " << i
                                  << std::endl;
                        reportError("Constructor argument types don't match");
                        currentType = TypeInfo::Error();
                        return;
                    }
                }
                currentType = type->type;
                std::cout << "[DEBUG] New expression type: " << currentType->toString()
                          << std::endl;
            }
            else
            {
                std::cout << "[DEBUG] Symbol is not a class" << std::endl;
                reportError(expr->typeName + " is not a class");
                currentType = TypeInfo::Error();
            }
        }
        else
        {
            std::cout << "[DEBUG] Type not found in symbol table" << std::endl;
            reportError("Undefined class: " + expr->typeName);
            currentType = TypeInfo::Error();
        }
    }

    void
    visit(SelfExpr* expr) override
    {
        std::cout << "[DEBUG] Processing SelfExpr" << std::endl;
        if (!currentClass)
        {
            std::cout << "[DEBUG] Self used outside of class" << std::endl;
            throw std::runtime_error("Cannot use self outside of class methods");
        }
        expr->inferredType = TypeInfo::UserDefined(currentClass->name);
        std::cout << "[DEBUG] Self type: " << expr->inferredType->toString() << std::endl;
    }

    void
    visit(GetAttrExpr* expr) override
    {
        std::cout << "\n[DEBUG] Processing GetAttrExpr for attribute: " << expr->attrName
                  << std::endl;

        expr->object->accept(this);
        auto objectType = expr->object->inferredType;
        std::cout << "[DEBUG] Object type: " << objectType->toString() << std::endl;

        if (!objectType)
        {
            std::cout << "[DEBUG] Cannot determine object type" << std::endl;
            throw std::runtime_error("Cannot determine type of object");
        }

        if (auto selfExpr = dynamic_cast<SelfExpr*>(expr->object.get()))
        {
            std::cout << "[DEBUG] Found self expression" << std::endl;
            if (!currentClass)
            {
                std::cout << "[DEBUG] Self used outside of class" << std::endl;
                throw std::runtime_error("Cannot use self outside of class methods");
            }
            objectType = currentClass;
            std::cout << "[DEBUG] Self type set to: " << objectType->toString() << std::endl;
        }

        if (!objectType->isClass())
        {
            std::cout << "[DEBUG] Object is not a class instance" << std::endl;
            throw std::runtime_error("Cannot access attributes on non-class instance");
        }

        auto classSymbol = symbols.lookup(objectType->name);
        if (!classSymbol || classSymbol->kind != SymbolKind::Type)
        {
            std::cout << "[DEBUG] Class not found: " << objectType->name << std::endl;
            throw std::runtime_error("Class not found: " + objectType->name);
        }

        std::cout << "[DEBUG] Found class symbol: " << classSymbol->name << std::endl;

        // Buscar el atributo en el TypeInfo de la clase
        if (auto attrType = objectType->getAttributeType(expr->attrName))
        {
            std::cout << "[DEBUG] Found attribute: " << expr->attrName
                      << " of type: " << (*attrType)->toString() << std::endl;
            expr->inferredType = *attrType;
        }
        else
        {
            std::cout << "[DEBUG] Attribute not found: " << expr->attrName << std::endl;
            throw std::runtime_error("Undefined attribute: " + expr->attrName);
        }
    }

    void
    visit(SetAttrExpr* expr) override
    {
        std::cout << "\n[DEBUG] Processing SetAttrExpr for attribute: " << expr->attrName
                  << std::endl;

        // Primero procesar el objeto
        expr->object->accept(this);
        auto objectType = expr->object->inferredType;
        std::cout << "[DEBUG] Object type: " << objectType->toString() << std::endl;

        if (!objectType)
        {
            std::cout << "[DEBUG] Cannot determine object type" << std::endl;
            throw std::runtime_error("Cannot determine type of object");
        }

        if (auto selfExpr = dynamic_cast<SelfExpr*>(expr->object.get()))
        {
            std::cout << "[DEBUG] Found self expression" << std::endl;
            if (!currentClass)
            {
                std::cout << "[DEBUG] Self used outside of class" << std::endl;
                throw std::runtime_error("Cannot use self outside of class methods");
            }
            objectType = currentClass;
            std::cout << "[DEBUG] Self type set to: " << objectType->toString() << std::endl;
        }

        if (!objectType->isClass())
        {
            std::cout << "[DEBUG] Object is not a class instance" << std::endl;
            throw std::runtime_error("Cannot access attributes on non-class instance");
        }

        // Buscar el atributo en el TypeInfo de la clase
        if (auto attrType = objectType->getAttributeType(expr->attrName))
        {
            std::cout << "[DEBUG] Found attribute: " << expr->attrName
                      << " of type: " << (*attrType)->toString() << std::endl;

            // Procesar el valor a asignar
            expr->value->accept(this);
            auto valueType = currentType;
            std::cout << "[DEBUG] Value type: " << valueType->toString() << std::endl;

            // Verificar compatibilidad de tipos
            if (valueType && *attrType && **attrType != *valueType)
            {
                std::cout << "[DEBUG] Type mismatch in attribute assignment" << std::endl;
                throw std::runtime_error("Type mismatch in attribute assignment: expected " +
                                         (*attrType)->toString() + " but got " +
                                         valueType->toString());
            }

            expr->inferredType = valueType;
            std::cout << "[DEBUG] SetAttr type: " << expr->inferredType->toString() << std::endl;
        }
        else
        {
            std::cout << "[DEBUG] Attribute not found: " << expr->attrName << std::endl;
            throw std::runtime_error("Undefined attribute: " + expr->attrName);
        }
    }

    void
    visit(MethodCallExpr* expr) override
    {
        std::cout << "[DEBUG] Processing MethodCallExpr for method: " << expr->methodName
                  << std::endl;

        // Obtener el tipo del objeto
        expr->object->accept(this);
        auto objectType = currentType;
        std::cout << "[DEBUG] Object type: " << objectType->toString() << std::endl;

        // Si el tipo es un tipo definido por el usuario
        if (objectType->isClass())
        {
            std::cout << "[DEBUG] Looking up class: " << objectType->name << std::endl;
            // Buscar el tipo en la tabla de símbolos
            if (auto classSymbol = symbols.lookup(objectType->name))
            {
                std::cout << "[DEBUG] Found class symbol: " << classSymbol->name << std::endl;

                // Buscar el método en el TypeInfo de la clase
                if (auto methodType = objectType->getMethodType(expr->methodName))
                {
                    std::cout << "[DEBUG] Found method: " << expr->methodName
                              << " with type: " << (*methodType)->toString() << std::endl;

                    // Procesar los argumentos
                    for (const auto& arg : expr->args)
                    {
                        arg->accept(this);
                    }

                    // El tipo de retorno será el tipo del método
                    currentType = *methodType;
                }
                else
                {
                    std::cout << "[DEBUG] Method not found: " << expr->methodName << std::endl;
                    throw TypeError("Undefined method: " + expr->methodName);
                }
            }
            else
            {
                throw TypeError("Undefined type: " + objectType->name);
            }
        }
        else
        {
            throw TypeError("Cannot call method on non-class type: " + objectType->toString());
        }

        expr->inferredType = currentType;
        std::cout << "[DEBUG] Method call type: " << currentType->toString() << std::endl;
    }

    void
    visit(BaseCallExpr* expr) override
    {
        std::cout << "\n[DEBUG] Processing BaseCallExpr" << std::endl;
        if (currentClass && currentClass->hasBase())
        {
            std::cout << "[DEBUG] Current class: " << currentClass->name << std::endl;
            std::vector<std::shared_ptr<TypeInfo>> argTypes;
            for (const auto& arg : expr->args)
            {
                auto argType = getType(arg.get());
                argTypes.push_back(argType);
                std::cout << "[DEBUG] Argument type: " << argType->toString() << std::endl;
            }
            if (auto methodType = currentClass->getBaseMethodType(currentMethodName))
            {
                std::cout << "[DEBUG] Found base method with type: " << (*methodType)->toString()
                          << std::endl;
                if ((*methodType)->getParamTypes() != argTypes)
                {
                    std::cout << "[DEBUG] Base method call argument types don't match" << std::endl;
                    reportError("Base method call argument types don't match");
                    currentType = TypeInfo::Error();
                    return;
                }
                currentType = (*methodType)->getReturnType();
                std::cout << "[DEBUG] Base call type: " << currentType->toString() << std::endl;
            }
            else
            {
                std::cout << "[DEBUG] Base method not found: " << currentMethodName << std::endl;
                reportError("Undefined base method: " + currentMethodName);
                currentType = TypeInfo::Error();
            }
        }
        else
        {
            std::cout << "[DEBUG] Base call used outside of class or class has no base"
                      << std::endl;
            reportError("'base' can only be used inside class methods with a base class");
            currentType = TypeInfo::Error();
        }
    }

    void
    visit(ExprStmt* stmt) override
    {
        std::cout << "[DEBUG] Processing ExprStmt" << std::endl;
        getType(stmt->expr.get());
        std::cout << "[DEBUG] Expression type: " << currentType->toString() << std::endl;
    }

    void
    visit(Program* prog) override
    {
        std::cout << "[DEBUG] Processing Program" << std::endl;
        for (const auto& stmt : prog->stmts)
        {
            stmt->accept(this);
        }
    }

    void
    visit(FunctionDecl* stmt) override
    {
        std::cout << "\n[DEBUG] Processing FunctionDecl: " << stmt->name << std::endl;

        // Entrar en el scope de la función
        symbols.enterScope();
        std::cout << "[DEBUG] Entered function scope" << std::endl;

        // Registrar los parámetros con tipo Any inicialmente
        std::vector<std::shared_ptr<TypeInfo>> paramTypes;
        for (const auto& param : stmt->params)
        {
            Symbol paramSym;
            paramSym.name = param;
            paramSym.kind = SymbolKind::Parameter;
            paramSym.type = TypeInfo::Any();  // Tipo inicial Any, se inferirá después
            symbols.define(param, paramSym);
            paramTypes.push_back(paramSym.type);
            std::cout << "[DEBUG] Parameter registered: " << param << std::endl;
        }

        // Procesar el cuerpo para inferir el tipo de retorno
        if (auto exprStmt = dynamic_cast<ExprStmt*>(stmt->body.get()))
        {
            currentType = getType(exprStmt->expr.get());
        }
        else
        {
            currentType = getType(stmt->body.get());
        }
        std::cout << "[DEBUG] Function body type: " << currentType->toString() << std::endl;

        // Crear el tipo de la función con los tipos inferidos
        auto funcType = TypeInfo::Function(paramTypes, currentType);

        // Registrar la función en la tabla de símbolos
        Symbol sym;
        sym.name = stmt->name;
        sym.kind = SymbolKind::Function;
        sym.type = funcType;
        symbols.define(stmt->name, sym);
        std::cout << "[DEBUG] Function registered with type: " << funcType->toString() << std::endl;

        // Salir del scope de la función
        symbols.exitScope();
        std::cout << "[DEBUG] Exited function scope" << std::endl;
    }

    void
    visit(MethodDecl* stmt) override
    {
        std::cout << "[DEBUG] Processing MethodDecl: " << stmt->name << std::endl;

        // Guardar el nombre del método actual
        currentMethodName = stmt->name;

        // Entrar en el scope del método
        symbols.enterScope();
        std::cout << "[DEBUG] Entered method scope for: " << stmt->name << std::endl;

        // Registrar el parámetro 'self' implícito
        Symbol selfSymbol;
        selfSymbol.name = "self";
        selfSymbol.kind = SymbolKind::Parameter;
        selfSymbol.type = currentClass;
        symbols.define("self", selfSymbol);
        std::cout << "[DEBUG] Registered self parameter" << std::endl;

        // Registrar los parámetros del método
        std::vector<std::shared_ptr<TypeInfo>> paramTypes;
        for (const auto& param : stmt->params)
        {
            std::cout << "[DEBUG] Registering parameter: " << param << std::endl;
            Symbol paramSymbol;
            paramSymbol.name = param;
            paramSymbol.kind = SymbolKind::Parameter;

            // Para métodos set, inferir el tipo del parámetro basado en el atributo
            if (stmt->name.substr(0, 3) == "set" && stmt->name.length() > 3)
            {
                std::string attrName = stmt->name.substr(3);
                // Convertir primera letra a minúscula
                attrName[0] = std::tolower(attrName[0]);
                if (auto attrType = currentClass->getAttributeType(attrName))
                {
                    paramSymbol.type = *attrType;
                    std::cout << "[DEBUG] Inferred parameter type from attribute: "
                              << (*attrType)->toString() << std::endl;
                }
                else
                {
                    paramSymbol.type = TypeInfo::Any();
                }
            }
            else
            {
                paramSymbol.type = TypeInfo::Any();
            }

            symbols.define(param, paramSymbol);
            paramTypes.push_back(paramSymbol.type);
        }

        // Procesar el cuerpo del método
        if (stmt->body)
        {
            stmt->body->accept(this);
            stmt->inferredType = currentType;
            std::cout << "[DEBUG] Method body type: " << currentType->toString() << std::endl;
        }

        // Registrar el tipo del método en el TypeInfo de la clase
        auto methodType = TypeInfo::Function(paramTypes, stmt->inferredType);
        currentClass->addMethod(stmt->name, methodType);
        std::cout << "[DEBUG] Method type registered: " << methodType->toString() << std::endl;

        // Salir del scope del método
        symbols.exitScope();
        std::cout << "[DEBUG] Exited method scope for: " << stmt->name << std::endl;

        // Limpiar el nombre del método actual
        currentMethodName = "";
    }

    void
    visit(TypeDecl* stmt) override
    {
        std::cout << "\n[DEBUG] Processing TypeDecl: " << stmt->name << std::endl;

        // Registrar el tipo en la tabla de símbolos
        Symbol typeSymbol;
        typeSymbol.name = stmt->name;
        typeSymbol.kind = SymbolKind::Type;
        typeSymbol.type = TypeInfo::UserDefined(stmt->name);
        symbols.define(stmt->name, typeSymbol);
        std::cout << "[DEBUG] Type registered in symbol table" << std::endl;

        // Entrar en el scope del tipo
        symbols.enterScope();
        std::cout << "[DEBUG] Entered type scope" << std::endl;

        // Guardar el tipo actual como TypeInfo
        currentClass = TypeInfo::UserDefined(stmt->name);

        // Registrar los parámetros del constructor primero
        std::vector<std::shared_ptr<TypeInfo>> paramTypes;
        for (const auto& param : stmt->params)
        {
            std::cout << "[DEBUG] Registering constructor parameter: " << param << std::endl;
            Symbol paramSymbol;
            paramSymbol.name = param;
            paramSymbol.kind = SymbolKind::Parameter;
            paramSymbol.type = TypeInfo::Any();  // Temporalmente asignamos Any
            symbols.define(param, paramSymbol);
            paramTypes.push_back(paramSymbol.type);
        }

        // Registrar el constructor en el TypeInfo
        currentClass->setConstructorParams(paramTypes);

        // Primero registrar todos los atributos con tipo Any
        for (const auto& attr : stmt->attributes)
        {
            std::cout << "[DEBUG] Registering attribute: " << attr->name << std::endl;
            Symbol attrSymbol;
            attrSymbol.name = attr->name;
            attrSymbol.kind = SymbolKind::Attribute;
            attrSymbol.type = TypeInfo::Any();  // Temporalmente asignamos Any
            symbols.define(attr->name, attrSymbol);
            // Agregar el atributo al TypeInfo de la clase
            currentClass->addAttribute(attr->name, attrSymbol.type);
        }

        // Luego evaluar los inicializadores
        for (const auto& attr : stmt->attributes)
        {
            if (attr->initializer)
            {
                attr->initializer->accept(this);
                // Actualizar el tipo del atributo con el tipo inferido
                if (auto symbol = symbols.lookup(attr->name))
                {
                    symbol->type = currentType;
                    // Actualizar el tipo en el TypeInfo de la clase
                    currentClass->addAttribute(attr->name, currentType);
                    std::cout << "[DEBUG] Attribute " << attr->name
                              << " has type: " << symbol->type->toString() << std::endl;
                }
            }
        }

        // Registrar y procesar los métodos
        for (const auto& method : stmt->methods)
        {
            std::cout << "[DEBUG] Registering method: " << method->name << std::endl;
            Symbol methodSymbol;
            methodSymbol.name = method->name;
            methodSymbol.kind = SymbolKind::Method;
            methodSymbol.type = TypeInfo::Any();
            symbols.define(method->name, methodSymbol);

            // Procesar el cuerpo del método
            std::cout << "[DEBUG] Processing method body: " << method->name << std::endl;
            method->accept(this);

            // Guardar el tipo del método en el TypeInfo de la clase
            currentClass->addMethod(method->name, method->inferredType);
            std::cout << "[DEBUG] Method " << method->name
                      << " has type: " << method->inferredType->toString() << std::endl;
        }

        // Actualizar el tipo en la tabla de símbolos con toda la información
        typeSymbol.type = currentClass;
        symbols.update(stmt->name, typeSymbol);

        // Restaurar el tipo actual
        currentClass = nullptr;

        // Salir del scope del tipo
        symbols.exitScope();
        std::cout << "[DEBUG] Exited type scope" << std::endl;
    }

   private:
    std::shared_ptr<TypeInfo> currentType;
    SymbolTable& symbols;
    std::shared_ptr<TypeInfo> currentClass = nullptr;
    std::string currentMethodName;
    std::vector<std::string> errors;

    void
    addBuiltinFunction(const std::string& name,
                       const std::vector<std::shared_ptr<TypeInfo>>& paramTypes,
                       const std::shared_ptr<TypeInfo>& returnType)
    {
        Symbol sym;
        sym.name = name;
        sym.kind = SymbolKind::Function;
        sym.type = TypeInfo::Function(paramTypes, returnType);
        symbols.define(name, sym);
    }

    void
    reportError(const std::string& message)
    {
        error(message);
    }

    bool
    isNumeric(const std::shared_ptr<TypeInfo>& type)
    {
        return type->kind == TypeInfo::Kind::Int || type->kind == TypeInfo::Kind::Float;
    }

    std::shared_ptr<TypeInfo>
    promoteNumeric(const std::shared_ptr<TypeInfo>& t1, const std::shared_ptr<TypeInfo>& t2)
    {
        if (t1->kind == TypeInfo::Kind::Float || t2->kind == TypeInfo::Kind::Float)
        {
            return TypeInfo::Float();
        }
        return TypeInfo::Int();
    }
};
