#include "semantic_checker.hpp"
#include <algorithm>
#include <set>
#include <sstream>

void SemanticAnalyzer::registerBuiltinFunctions()
{
    // Register built-in functions with their signatures    // Math functions
    symbol_table_.declareFunction("sqrt", {"x"});
    symbol_table_.declareFunction("sin", {"x"});
    symbol_table_.declareFunction("cos", {"x"});
    symbol_table_.declareFunction("exp", {"x"});
    symbol_table_.declareFunction("log", {"x"});
    symbol_table_.declareFunction("pow", {"base", "exponent"});
    symbol_table_.declareFunction("rand", {});
    symbol_table_.declareFunction("floor", {"x"});
    symbol_table_.declareFunction("ceil", {"x"});

    // I/O functions
    symbol_table_.declareFunction("print", {"x"});
    symbol_table_.declareFunction("println", {"x"});

    // String functions
    symbol_table_.declareFunction("parse", {"s"});
    symbol_table_.declareFunction("str", {"x"}); // Agregar str para conversión
}

void SemanticAnalyzer::collectFunctions(Program *program)
{
    for (auto &stmt : program->stmts)
    {
        if (auto funcDecl = dynamic_cast<FunctionDecl *>(stmt.get()))
        {
            if (symbol_table_.isFunctionDeclared(funcDecl->name))
            {
                error_manager_.reportError(ErrorType::REDEFINED_FUNCTION,
                                           "Función '" + funcDecl->name + "' ya está definida",
                                           funcDecl->line_number, funcDecl->column_number,
                                           "declaración de función", "SemanticAnalyzer");
            }
            else
            {
                symbol_table_.declareFunction(funcDecl->name, funcDecl->params);
            }
        }
        else if (auto typeDecl = dynamic_cast<TypeDecl *>(stmt.get()))
        {
            if (symbol_table_.isTypeDeclared(typeDecl->name))
            {
                error_manager_.reportError(ErrorType::REDEFINED_TYPE,
                                           "Tipo '" + typeDecl->name + "' ya está definido",
                                           typeDecl->line_number, typeDecl->column_number,
                                           "declaración de tipo", "SemanticAnalyzer");
            }
            else
            {
                symbol_table_.declareType(typeDecl->name);
            }
        }
    }
}

void SemanticAnalyzer::reportError(ErrorType type, const std::string &message,
                                   Expr *expr, const std::string &context)
{
    int line = expr ? expr->line_number : 0;
    int col = expr ? expr->column_number : 0;
    error_manager_.reportError(type, message, line, col, context, "SemanticAnalyzer");
}

void SemanticAnalyzer::reportError(ErrorType type, const std::string &message,
                                   Stmt *stmt, const std::string &context)
{
    int line = stmt ? stmt->line_number : 0;
    int col = stmt ? stmt->column_number : 0;
    error_manager_.reportError(type, message, line, col, context, "SemanticAnalyzer");
}

void SemanticAnalyzer::visit(Program *prog)
{
    // Enter global scope
    symbol_table_.enterScope();

    // Visit all statements
    for (const auto &stmt : prog->stmts)
    {
        stmt->accept(this);
    }

    // Exit global scope
    symbol_table_.exitScope();
}

void SemanticAnalyzer::visit(NumberExpr *expr)
{
    current_type_ = TypeInfo(TypeInfo::Kind::Number);
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(StringExpr *expr)
{
    current_type_ = TypeInfo(TypeInfo::Kind::String);
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(BooleanExpr *expr)
{
    current_type_ = TypeInfo(TypeInfo::Kind::Boolean);
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(UnaryExpr *expr)
{
    // Visit operand first
    expr->operand->accept(this);
    TypeInfo operandType = current_type_;

    // Infer type based on operator
    std::string opStr = expr->op == UnaryExpr::OP_NEG ? "-" : "!";
    current_type_ = TypeInfo::inferUnaryOp(opStr, operandType);

    // Check if operation is valid
    if (current_type_.isUnknown())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Invalid unary operation '" + opStr + "' on type " + operandType.toString(),
                    expr);
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(BinaryExpr *expr)
{
    // Visit left and right operands
    expr->left->accept(this);
    TypeInfo leftType = current_type_;

    expr->right->accept(this);
    TypeInfo rightType = current_type_;

    // // Infer type based on operator
    std::string opStr = getBinaryOpString(expr->op);
    // current_type_ = TypeInfo::inferBinaryOp(opStr, leftType, rightType);
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
    case BinaryExpr::OP_SUB:
    case BinaryExpr::OP_MUL:
    case BinaryExpr::OP_DIV:
    case BinaryExpr::OP_MOD:
    case BinaryExpr::OP_POW:
        if (!(leftType.isNumeric() || leftType.isUnknown()) && (rightType.isNumeric() || rightType.isUnknown()))
        {
            reportError(ErrorType::TYPE_ERROR,
                        "Invalid binary operation '" + opStr + "' between types " +
                            leftType.toString() + " and " + rightType.toString(),
                        expr);
        }
        current_type_ = TypeInfo(TypeInfo::Kind::Number);
        break;
    case BinaryExpr::OP_EQ:
    case BinaryExpr::OP_NEQ:
        if (!(leftType.getKind() == rightType.getKind()))
        {
            reportError(ErrorType::TYPE_ERROR,
                        "Invalid binary operation '" + opStr + "' between types " +
                            leftType.toString() + " and " + rightType.toString(),
                        expr);
        }
        current_type_ = TypeInfo(TypeInfo::Kind::Boolean);
        break;

    case BinaryExpr::OP_LT:
    case BinaryExpr::OP_LE:
    case BinaryExpr::OP_GT:
    case BinaryExpr::OP_GE:
        if (!(leftType.isNumeric() || leftType.isUnknown()) && (rightType.isNumeric() || rightType.isUnknown()))
        {
            reportError(ErrorType::TYPE_ERROR,
                        "Invalid binary operation '" + opStr + "' between types " +
                            leftType.toString() + " and " + rightType.toString(),
                        expr);
        }
        current_type_ = TypeInfo(TypeInfo::Kind::Boolean);
        break;
    case BinaryExpr::OP_AND:
    case BinaryExpr::OP_OR:
        if (!(leftType.isBoolean() || leftType.isUnknown()) && (rightType.isBoolean() || rightType.isUnknown()))
        {
            reportError(ErrorType::TYPE_ERROR,
                        "Invalid binary operation '" + opStr + "' between types " +
                            leftType.toString() + " and " + rightType.toString(),
                        expr);
        }
        current_type_ = TypeInfo(TypeInfo::Kind::Boolean);
        break;
    case BinaryExpr::OP_CONCAT:
    case BinaryExpr::OP_CONCAT_WS:
        if ((leftType.isObject() || rightType.isObject()))
        {
            reportError(ErrorType::TYPE_ERROR,
                        "Invalid binary operation '" + opStr + "' between types " +
                            leftType.toString() + " and " + rightType.toString(),
                        expr);
        }
        current_type_ = TypeInfo(TypeInfo::Kind::String);
        break;
    }
    // Check if operation is valid
    if (current_type_.isUnknown())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Invalid binary operation '" + opStr + "' between types " +
                        leftType.toString() + " and " + rightType.toString(),
                    expr);
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(CallExpr *expr)
{
    // // Check if function exists
    // auto funcInfo = symbol_table_.lookupFunction(expr->callee);
    // if (!funcInfo)
    // {
    //     reportError(ErrorType::UNDEFINED_FUNCTION,
    //                 "Function '" + expr->callee + "' is not defined",
    //                 expr);
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    //     expr->inferredType = std::make_shared<TypeInfo>(current_type_);
    //     return;
    // }

    // // Check argument count
    // if (expr->args.size() != funcInfo->parameter_types.size())
    // {
    //     reportError(ErrorType::ARGUMENT_COUNT_MISMATCH,
    //                 "Function '" + expr->callee + "' expects " +
    //                     std::to_string(funcInfo->parameter_types.size()) +
    //                     " arguments but got " + std::to_string(expr->args.size()),
    //                 expr);
    // }

    // // Check argument types
    // for (size_t i = 0; i < expr->args.size() && i < funcInfo->parameter_types.size(); ++i)
    // {
    //     expr->args[i]->accept(this);
    //     if (!current_type_.isCompatibleWith(funcInfo->parameter_types[i]))
    //     {
    //         reportError(ErrorType::TYPE_ERROR,
    //                     "Argument " + std::to_string(i + 1) + " of function '" + expr->callee +
    //                         "' expects type " + funcInfo->parameter_types[i].toString() +
    //                         " but got " + current_type_.toString(),
    //                     expr);
    //     }
    // }

    // // Set return type
    // current_type_ = funcInfo->return_type_ ? *funcInfo->return_type_ : TypeInfo(TypeInfo::Kind::Unknown);
    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(VariableExpr *expr)
{
    // Look up variable in symbol table
    auto varInfo = symbol_table_.lookupVariable(expr->name);
    if (!varInfo)
    {
        reportError(ErrorType::UNDEFINED_VARIABLE,
                    "Variable '" + expr->name + "' is not defined",
                    expr);
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    }
    else
    {
        current_type_ = varInfo->type;
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(LetExpr *expr)
{
    // Visit initializer first
    expr->initializer->accept(this);
    TypeInfo initType = current_type_;
    // Enter new scope
    symbol_table_.enterScope();

    // Add variable to symbol table
    // Declare the variable with the type from the initializer
    if (symbol_table_.isVariableDeclared(expr->name))
    {
        reportError(ErrorType::REDEFINED_VARIABLE,
                    "Variable '" + expr->name + "' ya está definida en este ámbito",
                    expr, "expresión let");
    }
    else
    {
        symbol_table_.declareVariable(expr->name, initType);
    }

    // Visit body
    expr->body->accept(this);

    // Exit scope
    symbol_table_.exitScope();

    // Let expression returns the type of its body
    expr->inferredType = expr->body->inferredType;
}

void SemanticAnalyzer::visit(AssignExpr *expr)
{
    // Visit value first
    expr->value->accept(this);
    TypeInfo valueType = current_type_;

    // Look up variable
    auto varInfo = symbol_table_.lookupVariable(expr->name);
    if (!varInfo)
    {
        reportError(ErrorType::UNDEFINED_VARIABLE,
                    "Cannot assign to undefined variable '" + expr->name + "'",
                    expr);
    }
    else if (!valueType.isCompatibleWith(varInfo->type))
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot assign value of type " + valueType.toString() +
                        " to variable of type " + varInfo->type.toString(),
                    expr);
    }

    // Assignment returns the type of the value
    current_type_ = valueType;
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(IfExpr *expr)
{
    // Check condition type
    expr->condition->accept(this);
    if (!current_type_.isBoolean())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "If condition must be of type Boolean, got " + current_type_.toString(),
                    expr);
    }

    // Visit then branch
    expr->thenBranch->accept(this);
    TypeInfo thenType = current_type_;

    // Visit else branch
    expr->elseBranch->accept(this);
    TypeInfo elseType = current_type_;

    expr->inferredType = std::make_shared<TypeInfo>(TypeInfo::Kind::Void);
}

void SemanticAnalyzer::visit(ExprBlock *expr)
{
    // // Enter new scope
    // symbol_table_.enterScope();

    // // Visit all statements
    // for (const auto &stmt : expr->stmts)
    // {
    //     stmt->accept(this);
    // }

    // // Exit scope
    // symbol_table_.exitScope();

    // // Block returns the type of its last statement
    // if (!expr->stmts.empty())
    // {
    //     expr->inferredType = expr->stmts.back()->inferredType;
    // }
    // else
    // {
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    //     expr->inferredType = std::make_shared<TypeInfo>(current_type_);
    // }
}

void SemanticAnalyzer::visit(WhileExpr *expr)
{
    // // Check condition type
    // expr->condition->accept(this);
    // if (!current_type_.isBoolean())
    // {
    //     reportError(ErrorType::TYPE_ERROR,
    //                 "While condition must be of type Boolean, got " + current_type_.toString(),
    //                 expr);
    // }

    // // Visit body
    // expr->body->accept(this);

    // // While loop returns void
    // current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(FunctionDecl *stmt)
{
    // Enter new scope for function body
    symbol_table_.enterScope();
    // Add parameters to symbol table
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::shared_ptr<TypeInfo> paramType = stmt->paramTypes[i];
        symbol_table_.declareVariable(stmt->params[i], *paramType);
    }

    // Visit function body
    stmt->body->accept(this);
    // Check return type
    if (stmt->returnType->getKind() != TypeInfo::Kind::Unknown && !current_type_.isCompatibleWith(*stmt->returnType))
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Function '" + stmt->name + "' returns type " + current_type_.toString() +
                        " but declared return type is " + stmt->returnType->toString(),
                    stmt);
    }
    else if (stmt->returnType->getKind() == TypeInfo::Kind::Unknown)
    {
        *stmt->returnType = current_type_;
        std::cout << "return type de " << stmt->name << " es " << stmt->returnType->toString() << "\n";
    }

    // Exit function scope
    symbol_table_.exitScope();

    // Add function to symbol table
    std::vector<TypeInfo> paramTypes;
    for (const auto &type : stmt->paramTypes)
    {
        paramTypes.push_back(*type);
    }
    symbol_table_.declareFunction(stmt->name, paramTypes, *stmt->returnType);
}

void SemanticAnalyzer::visit(TypeDecl *stmt)
{
    // // Enter new scope for type
    // symbol_table_.enterScope();

    // // Add self reference
    // symbol_table_.defineVariable("self", TypeInfo(TypeInfo::Kind::Object, stmt->name));

    // // Visit attributes
    // for (const auto &attr : stmt->attributes)
    // {
    //     attr->initializer->accept(this);
    //     symbol_table_.defineVariable(attr->name, current_type_);
    // }

    // // Visit methods
    // for (const auto &method : stmt->methods)
    // {
    //     method->accept(this);
    // }

    // // Exit type scope
    // symbol_table_.exitScope();

    // // Add type to symbol table
    // TypeInfo typeInfo(TypeInfo::Kind::Object, stmt->name);
    // symbol_table_.defineType(stmt->name, typeInfo);
}

void SemanticAnalyzer::visit(NewExpr *expr)
{
    // // Check if type exists
    // auto typeInfo = symbol_table_.lookupType(expr->typeName);
    // if (!typeInfo)
    // {
    //     reportError(ErrorType::UNDEFINED_TYPE,
    //                 "Type '" + expr->typeName + "' is not defined",
    //                 expr);
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    // }
    // else
    // {
    //     current_type_ = *typeInfo;
    // }

    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(GetAttrExpr *expr)
{
    // // Visit object first
    // expr->object->accept(this);
    // if (!current_type_.isObject())
    // {
    //     reportError(ErrorType::TYPE_ERROR,
    //                 "Cannot access attribute '" + expr->attrName + "' on non-object type " +
    //                     current_type_.toString(),
    //                 expr);
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    // }
    // else
    // {
    //     // Look up attribute in type
    //     auto attrInfo = symbol_table_.lookupAttribute(current_type_.getTypeName(), expr->attrName);
    //     if (!attrInfo)
    //     {
    //         reportError(ErrorType::UNDEFINED_ATTRIBUTE,
    //                     "Type '" + current_type_.getTypeName() + "' has no attribute '" +
    //                         expr->attrName + "'",
    //                     expr);
    //         current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    //     }
    //     else
    //     {
    //         current_type_ = attrInfo->type;
    //     }
    // }

    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(SetAttrExpr *expr)
{
    // // Visit object first
    // expr->object->accept(this);
    // if (!current_type_.isObject())
    // {
    //     reportError(ErrorType::TYPE_ERROR,
    //                 "Cannot set attribute '" + expr->attrName + "' on non-object type " +
    //                     current_type_.toString(),
    //                 expr);
    // }
    // else
    // {
    //     // Look up attribute in type
    //     auto attrInfo = symbol_table_.lookupAttribute(current_type_.getTypeName(), expr->attrName);
    //     if (!attrInfo)
    //     {
    //         reportError(ErrorType::UNDEFINED_ATTRIBUTE,
    //                     "Type '" + current_type_.getTypeName() + "' has no attribute '" +
    //                         expr->attrName + "'",
    //                     expr);
    //     }
    //     else
    //     {
    //         // Visit value
    //         expr->value->accept(this);
    //         if (!current_type_.isCompatibleWith(attrInfo->type))
    //         {
    //             reportError(ErrorType::TYPE_ERROR,
    //                         "Cannot assign value of type " + current_type_.toString() +
    //                             " to attribute of type " + attrInfo->type.toString(),
    //                         expr);
    //         }
    //     }
    // }

    // // Set attribute returns the type of the value
    // expr->inferredType = expr->value->inferredType;
}

void SemanticAnalyzer::visit(MethodCallExpr *expr)
{
    // // Visit object first
    // expr->object->accept(this);
    // if (!current_type_.isObject())
    // {
    //     reportError(ErrorType::TYPE_ERROR,
    //                 "Cannot call method '" + expr->methodName + "' on non-object type " +
    //                     current_type_.toString(),
    //                 expr);
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    // }
    // else
    // {
    //     // Look up method in type
    //     auto methodInfo = symbol_table_.lookupMethod(current_type_.getTypeName(), expr->methodName);
    //     if (!methodInfo)
    //     {
    //         reportError(ErrorType::UNDEFINED_METHOD,
    //                     "Type '" + current_type_.getTypeName() + "' has no method '" +
    //                         expr->methodName + "'",
    //                     expr);
    //         current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    //     }
    //     else
    //     {
    //         // Check argument count
    //         if (expr->args.size() != methodInfo->parameter_types_.size())
    //         {
    //             reportError(ErrorType::ARGUMENT_COUNT_MISMATCH,
    //                         "Method '" + expr->methodName + "' expects " +
    //                             std::to_string(methodInfo->parameter_types_.size()) +
    //                             " arguments but got " + std::to_string(expr->args.size()),
    //                         expr);
    //         }

    //         // Check argument types
    //         for (size_t i = 0; i < expr->args.size() && i < methodInfo->parameter_types_.size(); ++i)
    //         {
    //             expr->args[i]->accept(this);
    //             if (!current_type_.isCompatibleWith(methodInfo->parameter_types_[i]))
    //             {
    //                 reportError(ErrorType::TYPE_ERROR,
    //                             "Argument " + std::to_string(i + 1) + " of method '" + expr->methodName +
    //                                 "' expects type " + methodInfo->parameter_types_[i].toString() +
    //                                 " but got " + current_type_.toString(),
    //                             expr);
    //             }
    //         }

    //         // Set return type
    //         current_type_ = methodInfo->return_type_ ? *methodInfo->return_type_ : TypeInfo(TypeInfo::Kind::Unknown);
    //     }
    // }

    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(SelfExpr *expr)
{
    // // Look up self in current scope
    // auto selfInfo = symbol_table_.lookupVariable("self");
    // if (!selfInfo)
    // {
    //     reportError(ErrorType::INVALID_SELF,
    //                 "Cannot use 'self' outside of a type definition",
    //                 expr);
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    // }
    // else
    // {
    //     current_type_ = selfInfo->type;
    // }

    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(BaseCallExpr *expr)
{
    // // Look up self in current scope
    // auto selfInfo = symbol_table_.lookupVariable("self");
    // if (!selfInfo)
    // {
    //     reportError(ErrorType::INVALID_BASE,
    //                 "Cannot use 'base' outside of a type definition",
    //                 expr);
    //     current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    // }
    // else
    // {
    //     // Get base type
    //     auto typeInfo = symbol_table_.lookupType(selfInfo->type.getTypeName());
    //     if (!typeInfo || typeInfo->getTypeName() == "Object")
    //     {
    //         reportError(ErrorType::INVALID_BASE,
    //                     "Cannot use 'base' in type without a base type",
    //                     expr);
    //         current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    //     }
    //     else
    //     {
    //         current_type_ = *typeInfo;
    //     }
    // }

    // expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(AttributeDecl *stmt)
{
    // // Analizar el inicializador del atributo
    // stmt->initializer->accept(this);
    // // El tipo del atributo será el tipo inferido del inicializador
    // stmt->inferredType = std::make_shared<TypeInfo>(current_type_);
}

// Helper methods
std::string SemanticAnalyzer::getBinaryOpString(BinaryExpr::Op op)
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
        return "unknown";
    }
}

bool SemanticAnalyzer::areTypesCompatible(const TypeInfo &type1, const TypeInfo &type2)
{
    // Basic type compatibility check
    if (type1.getKind() == TypeInfo::Kind::Unknown || type2.getKind() == TypeInfo::Kind::Unknown)
    {
        return true; // Unknown types are compatible with anything
    }

    if (type1.getKind() == type2.getKind())
    {
        if (type1.getKind() == TypeInfo::Kind::Object)
        {
            return type1.getTypeName() == type2.getTypeName();
        }
        return true;
    }

    return false;
}

bool SemanticAnalyzer::isReservedWord(const std::string &word)
{
    static const std::set<std::string> reserved_words = {
        "base", "self", "new", "type", "if", "else", "while", "for", "in",
        "function", "let", "true", "false", "null", "is", "inherits",
        "protocol", "extends", "class", "method", "attribute"};
    return reserved_words.find(word) != reserved_words.end();
}

void SemanticAnalyzer::analyze(Program *program)
{
    if (!program)
    {
        error_manager_.reportError(ErrorType::GENERAL_ERROR,
                                   "Programa nulo proporcionado para análisis", 0, 0, "", "SemanticAnalyzer");
        return;
    }

    // First pass: collect function declarations
    collectFunctions(program);

    // Second pass: analyze all expressions
    program->accept(this);
}

void SemanticAnalyzer::visit(ExprStmt *stmt)
{
    stmt->expr->accept(this);
    stmt->inferredType = stmt->expr->inferredType;
}

void SemanticAnalyzer::visit(MethodDecl *stmt)
{
    // // Enter method scope
    // symbol_table_.enterScope();

    // // Register parameters in the method's scope
    // for (const auto &param : stmt->params)
    // {
    //     symbol_table_.declareVariable(param, TypeInfo::Unknown());
    // }

    // // Visit method body
    // if (stmt->body)
    // {
    //     stmt->body->accept(this);
    // }

    // // Exit method scope
    // symbol_table_.exitScope();
}
