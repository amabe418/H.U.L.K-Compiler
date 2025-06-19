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

    // I/O functions
    symbol_table_.declareFunction("print", {"x"});
    symbol_table_.declareFunction("println", {"x"});

    // String functions

    // Register mathematical constants
    symbol_table_.declareVariable("PI", TypeInfo(TypeInfo::Kind::Number), false); // false means immutable
    symbol_table_.declareVariable("E", TypeInfo(TypeInfo::Kind::Number), false);  // false means immutable
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
    std::cout << "DEBUG: Visiting BinaryExpr with operator " << getBinaryOpString(expr->op) << std::endl;

    // Visit left and right operands
    expr->left->accept(this);
    TypeInfo leftType = current_type_;
    std::cout << "DEBUG: Left operand type: " << leftType.toString() << std::endl;

    expr->right->accept(this);
    TypeInfo rightType = current_type_;
    std::cout << "DEBUG: Right operand type: " << rightType.toString() << std::endl;

    // Infer type based on operator
    std::string opStr = getBinaryOpString(expr->op);
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
    case BinaryExpr::OP_SUB:
    case BinaryExpr::OP_MUL:
    case BinaryExpr::OP_DIV:
    case BinaryExpr::OP_MOD:
    case BinaryExpr::OP_POW:
        // If both operands are Unknown, infer both as Number (for arithmetic operations)
        if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.getKind() == TypeInfo::Kind::Unknown)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating Number type to left variable (both Unknown)" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating Number type to right variable (both Unknown)" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            leftType = TypeInfo(TypeInfo::Kind::Number);
            rightType = TypeInfo(TypeInfo::Kind::Number);
        }
        // If one operand is Unknown and the other is Number, propagate Number type
        else if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.isNumeric())
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating Number type to left variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            leftType = TypeInfo(TypeInfo::Kind::Number);
        }
        else if (rightType.getKind() == TypeInfo::Kind::Unknown && leftType.isNumeric())
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating Number type to right variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            rightType = TypeInfo(TypeInfo::Kind::Number);
        }

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
        // If both operands are Unknown, we can't infer a specific type
        // because equality can work with any type. We'll leave them as Unknown
        // and let other operations or context determine their types.
        if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.getKind() == TypeInfo::Kind::Unknown)
        {
            std::cout << "DEBUG: Both operands Unknown in equality comparison - leaving as Unknown" << std::endl;
            // Don't propagate any type - let other operations determine the types
        }
        // If one operand is Unknown and the other has a type, propagate the known type
        else if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.getKind() != TypeInfo::Kind::Unknown)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating type " << rightType.toString() << " to left variable" << std::endl;
                *varExpr->inferredType = rightType;
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = rightType;
                }
            }
            leftType = rightType;
        }
        else if (rightType.getKind() == TypeInfo::Kind::Unknown && leftType.getKind() != TypeInfo::Kind::Unknown)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating type " << leftType.toString() << " to right variable" << std::endl;
                *varExpr->inferredType = leftType;
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = leftType;
                }
            }
            rightType = leftType;
        }

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
        // If both operands are Unknown, infer both as Number (for relational operations)
        if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.getKind() == TypeInfo::Kind::Unknown)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating Number type to left variable (both Unknown, relational)" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating Number type to right variable (both Unknown, relational)" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            leftType = TypeInfo(TypeInfo::Kind::Number);
            rightType = TypeInfo(TypeInfo::Kind::Number);
        }
        // If one operand is Unknown and the other is Number, propagate Number type
        else if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.isNumeric())
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating Number type to left variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            leftType = TypeInfo(TypeInfo::Kind::Number);
        }
        else if (rightType.getKind() == TypeInfo::Kind::Unknown && leftType.isNumeric())
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating Number type to right variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Number);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Number);
                }
            }
            rightType = TypeInfo(TypeInfo::Kind::Number);
        }

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
        // Propagate Boolean type to unknown operands
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
        {
            std::cout << "DEBUG: Found variable in left operand: " << varExpr->name << std::endl;
            if (varExpr->inferredType->getKind() == TypeInfo::Kind::Unknown)
            {
                std::cout << "DEBUG: Propagating Boolean type to left variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Boolean);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Boolean);
                }
            }
        }
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
        {
            std::cout << "DEBUG: Found variable in right operand: " << varExpr->name << std::endl;
            if (varExpr->inferredType->getKind() == TypeInfo::Kind::Unknown)
            {
                std::cout << "DEBUG: Propagating Boolean type to right variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::Boolean);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::Boolean);
                }
            }
        }
        break;
    case BinaryExpr::OP_CONCAT:
    case BinaryExpr::OP_CONCAT_WS:
        // If both operands are Unknown, infer both as String (for concatenation)
        if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.getKind() == TypeInfo::Kind::Unknown)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating String type to left variable (both Unknown, concatenation)" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::String);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::String);
                }
            }
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating String type to right variable (both Unknown, concatenation)" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::String);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::String);
                }
            }
            leftType = TypeInfo(TypeInfo::Kind::String);
            rightType = TypeInfo(TypeInfo::Kind::String);
        }
        // If one operand is Unknown and the other is String, propagate String type
        else if (leftType.getKind() == TypeInfo::Kind::Unknown && rightType.getKind() == TypeInfo::Kind::String)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->left.get()))
            {
                std::cout << "DEBUG: Propagating String type to left variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::String);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::String);
                }
            }
            leftType = TypeInfo(TypeInfo::Kind::String);
        }
        else if (rightType.getKind() == TypeInfo::Kind::Unknown && leftType.getKind() == TypeInfo::Kind::String)
        {
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr->right.get()))
            {
                std::cout << "DEBUG: Propagating String type to right variable" << std::endl;
                *varExpr->inferredType = TypeInfo(TypeInfo::Kind::String);
                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = TypeInfo(TypeInfo::Kind::String);
                }
            }
            rightType = TypeInfo(TypeInfo::Kind::String);
        }

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

    std::cout << "DEBUG: BinaryExpr result type: " << current_type_.toString() << std::endl;
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(CallExpr *expr)
{
    std::cout << "DEBUG: Visiting CallExpr: " << expr->callee << std::endl;

    // Check if function exists
    auto funcInfo = symbol_table_.lookupFunction(expr->callee);
    if (!funcInfo)
    {
        // Function not found - this might be because it hasn't been analyzed yet
        // For now, we'll assume it returns Unknown and continue
        std::cout << "DEBUG: Function " << expr->callee << " not found in symbol table, assuming Unknown return type" << std::endl;

        // Process arguments anyway to infer their types
        for (size_t i = 0; i < expr->args.size(); ++i)
        {
            expr->args[i]->accept(this);
        }

        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Check argument count
    if (expr->args.size() != funcInfo->parameter_types.size())
    {
        reportError(ErrorType::ARGUMENT_COUNT_MISMATCH,
                    "Function '" + expr->callee + "' expects " +
                        std::to_string(funcInfo->parameter_types.size()) +
                        " arguments but got " + std::to_string(expr->args.size()),
                    expr);
    }

    // Check argument types and propagate types
    for (size_t i = 0; i < expr->args.size() && i < funcInfo->parameter_types.size(); ++i)
    {
        // First, check if the argument is a variable and propagate its type
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr->args[i].get()))
        {
            if (varExpr->inferredType->getKind() == TypeInfo::Kind::Unknown)
            {
                std::cout << "DEBUG: Propagating type " << funcInfo->parameter_types[i].toString()
                          << " to argument variable" << std::endl;
                *varExpr->inferredType = funcInfo->parameter_types[i];

                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = funcInfo->parameter_types[i];
                }
            }
        }

        // Then visit the argument to get its type
        expr->args[i]->accept(this);

        // If the argument is a variable and its type is Unknown, but the function expects a specific type,
        // propagate the expected type to the variable
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr->args[i].get()))
        {
            if (varExpr->inferredType->getKind() == TypeInfo::Kind::Unknown)
            {
                std::cout << "DEBUG: Propagating type " << funcInfo->parameter_types[i].toString()
                          << " to argument variable" << std::endl;
                *varExpr->inferredType = funcInfo->parameter_types[i];

                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "DEBUG: Updating symbol table for " << varExpr->name << std::endl;
                    varInfo->type = funcInfo->parameter_types[i];
                }
            }
        }

        if (!current_type_.isCompatibleWith(funcInfo->parameter_types[i]))
        {
            reportError(ErrorType::TYPE_ERROR,
                        "Argument " + std::to_string(i + 1) + " of function '" + expr->callee +
                            "' expects type " + funcInfo->parameter_types[i].toString() +
                            " but got " + current_type_.toString(),
                        expr);
        }
    }

    // Set return type
    current_type_ = funcInfo->return_type;
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(VariableExpr *expr)
{
    std::cout << "DEBUG: Visiting VariableExpr: " << expr->name << std::endl;
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
        std::cout << "DEBUG: Variable type from symbol table: " << current_type_.toString() << std::endl;
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
    std::cout << "\nDEBUG: Visiting FunctionDecl: " << stmt->name << std::endl;

    // Enter new scope for function body
    symbol_table_.enterScope();

    // First pass: declare parameters with unknown types
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        if (!stmt->paramTypes[i])
        {
            stmt->paramTypes[i] = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
        }
        std::cout << "DEBUG: Declaring parameter " << stmt->params[i] << " with type "
                  << stmt->paramTypes[i]->toString() << std::endl;
        symbol_table_.declareVariable(stmt->params[i], *stmt->paramTypes[i]);
    }

    // Visit function body to infer parameter types
    std::cout << "DEBUG: Visiting function body" << std::endl;
    stmt->body->accept(this);

    // Update parameter types based on their inferred types in the body
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        auto paramType = stmt->paramTypes[i];
        if (paramType->getKind() == TypeInfo::Kind::Unknown)
        {
            std::cout << "DEBUG: Checking inferred type for parameter " << stmt->params[i] << std::endl;
            // Look up the variable in the symbol table to get its inferred type
            auto varInfo = symbol_table_.lookupVariable(stmt->params[i]);
            if (varInfo)
            {
                std::cout << "DEBUG: Found variable info with type " << varInfo->type.toString() << std::endl;
                if (varInfo->type.getKind() != TypeInfo::Kind::Unknown)
                {
                    std::cout << "DEBUG: Updating parameter type to " << varInfo->type.toString() << std::endl;
                    *paramType = varInfo->type;
                }
            }
            else
            {
                std::cout << "DEBUG: No variable info found for parameter " << stmt->params[i] << std::endl;
            }
        }
    }

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
        std::cout << "DEBUG: Setting return type to " << stmt->returnType->toString() << std::endl;
    }

    // Exit function scope
    symbol_table_.exitScope();

    // Add function to symbol table with updated types
    std::vector<TypeInfo> paramTypes;
    for (const auto &type : stmt->paramTypes)
    {
        paramTypes.push_back(*type);
    }
    std::cout << "DEBUG: Declaring function with " << paramTypes.size() << " parameters" << std::endl;

    symbol_table_.declareFunction(stmt->name, paramTypes, *stmt->returnType);

    // Look up the function in symbol table and print its types
    auto funcInfo = symbol_table_.lookupFunction(stmt->name);
    if (funcInfo)
    {
        std::cout << "DEBUG: Function " << stmt->name << " in symbol table:" << std::endl;
        std::cout << "  - Parameter types: ";
        for (size_t i = 0; i < funcInfo->parameter_types.size(); ++i)
        {
            std::cout << funcInfo->parameter_types[i].toString();
            if (i < funcInfo->parameter_types.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "  - Return type: " << funcInfo->return_type.toString() << std::endl;
    }
    else
    {
        std::cout << "DEBUG: Function " << stmt->name << " not found in symbol table!" << std::endl;
    }
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
    std::cout << "DEBUG: Starting semantic analysis" << std::endl;

    // First pass: collect all function declarations (but don't declare them yet)
    collectFunctions(program);

    // Multiple passes: analyze function bodies and infer types until no changes
    bool typesChanged = true;
    int pass = 0;

    while (typesChanged && pass < 10)
    { // Limit to 10 passes to avoid infinite loops
        typesChanged = false;
        pass++;
        std::cout << "DEBUG: Starting pass " << pass << std::endl;

        for (auto &stmt : program->stmts)
        {
            if (auto funcDecl = dynamic_cast<FunctionDecl *>(stmt.get()))
            {
                std::cout << "DEBUG: Analyzing function: " << funcDecl->name << std::endl;

                // Store current parameter types to check if they changed
                std::vector<TypeInfo> oldParamTypes;
                for (const auto &type : funcDecl->paramTypes)
                {
                    oldParamTypes.push_back(*type);
                }
                TypeInfo oldReturnType = *funcDecl->returnType;

                funcDecl->accept(this);

                // Check if types changed
                for (size_t i = 0; i < funcDecl->paramTypes.size(); ++i)
                {
                    if (oldParamTypes[i].getKind() != funcDecl->paramTypes[i]->getKind())
                    {
                        typesChanged = true;
                        std::cout << "DEBUG: Parameter " << funcDecl->params[i] << " type changed from "
                                  << oldParamTypes[i].toString() << " to " << funcDecl->paramTypes[i]->toString() << std::endl;
                    }
                }
                if (oldReturnType.getKind() != funcDecl->returnType->getKind())
                {
                    typesChanged = true;
                    std::cout << "DEBUG: Return type changed from " << oldReturnType.toString()
                              << " to " << funcDecl->returnType->toString() << std::endl;
                }
            }
            else
            {
                stmt->accept(this);
            }
        }

        if (typesChanged)
        {
            std::cout << "DEBUG: Types changed in pass " << pass << ", doing another pass" << std::endl;
        }
        else
        {
            std::cout << "DEBUG: No types changed in pass " << pass << ", stopping" << std::endl;
        }
    }

    if (pass >= 10)
    {
        std::cout << "DEBUG: Warning: Reached maximum number of passes, some types may not be fully inferred" << std::endl;
    }
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
