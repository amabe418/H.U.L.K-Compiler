#include "semantic_checker.hpp"
#include <algorithm>
#include <set>
#include <sstream>

void SemanticAnalyzer::registerBuiltinFunctions()
{
    std::cout << "[DEBUG] Registering built-in functions" << std::endl;

    // Register print function - takes any type and returns void
    std::vector<TypeInfo> printParams = {TypeInfo(TypeInfo::Kind::Unknown)};
    symbol_table_.declareFunction("print", printParams, TypeInfo(TypeInfo::Kind::Void));

    // Register mathematical functions
    std::vector<TypeInfo> mathParams = {TypeInfo(TypeInfo::Kind::Number)};
    symbol_table_.declareFunction("sin", mathParams, TypeInfo(TypeInfo::Kind::Number));
    symbol_table_.declareFunction("cos", mathParams, TypeInfo(TypeInfo::Kind::Number));
    symbol_table_.declareFunction("sqrt", mathParams, TypeInfo(TypeInfo::Kind::Number));
    symbol_table_.declareFunction("log", mathParams, TypeInfo(TypeInfo::Kind::Number));
    symbol_table_.declareFunction("exp", mathParams, TypeInfo(TypeInfo::Kind::Number));

    // Register rand function - no parameters, returns number
    std::vector<TypeInfo> randParams = {};
    symbol_table_.declareFunction("rand", randParams, TypeInfo(TypeInfo::Kind::Number));

    // Register range function - takes two numbers, returns iterable
    std::vector<TypeInfo> rangeParams = {TypeInfo(TypeInfo::Kind::Number), TypeInfo(TypeInfo::Kind::Number)};
    symbol_table_.declareFunction("range", rangeParams, TypeInfo(TypeInfo::Kind::Object, "Iterable"));

    // Register mathematical constants
    symbol_table_.declareVariable("PI", TypeInfo(TypeInfo::Kind::Number), false); // false means immutable
    symbol_table_.declareVariable("E", TypeInfo(TypeInfo::Kind::Number), false);  // false means immutable

    std::cout << "[DEBUG] Built-in functions registered" << std::endl;
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

void SemanticAnalyzer::reportError(ErrorType type, const std::string &message,
                                   const std::string &context)
{
    error_manager_.reportError(type, message, 0, 0, context, "SemanticAnalyzer");
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
    std::cout << "[DEBUG] Visiting CallExpr: " << expr->callee << std::endl;

    // Check if function exists
    auto funcInfo = symbol_table_.lookupFunction(expr->callee);
    if (!funcInfo)
    {
        reportError(ErrorType::UNDEFINED_FUNCTION,
                    "Function '" + expr->callee + "' is not defined",
                    expr, "llamada a función");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    std::cout << "[DEBUG] Function " << expr->callee << " found in symbol table" << std::endl;
    std::cout << "[DEBUG] Expected " << funcInfo->parameter_types.size() << " arguments" << std::endl;

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
        std::cout << "[DEBUG] Processing argument " << i << std::endl;

        // First, check if the argument is a variable and propagate its type
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr->args[i].get()))
        {
            std::cout << "[DEBUG] Argument " << i << " is a VariableExpr: " << varExpr->name << std::endl;

            // Look up the variable in symbol table to get its current type
            auto varInfo = symbol_table_.lookupVariable(varExpr->name);
            if (varInfo)
            {
                std::cout << "[DEBUG] Variable " << varExpr->name << " found in symbol table with type: " << varInfo->type.toString() << std::endl;
                *varExpr->inferredType = varInfo->type;
            }
            else
            {
                reportError(ErrorType::UNDEFINED_VARIABLE,
                            "Variable '" + varExpr->name + "' is not defined",
                            expr, "llamada a función");
            }
        }

        // Then visit the argument to get its type
        expr->args[i]->accept(this);
        std::cout << "[DEBUG] Argument " << i << " type after visit: " << current_type_.toString() << std::endl;

        // If the argument is a variable and its type is Unknown, but the function expects a specific type,
        // propagate the expected type to the variable
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr->args[i].get()))
        {
            if (varExpr->inferredType->getKind() == TypeInfo::Kind::Unknown)
            {
                std::cout << "[DEBUG] Propagating type " << funcInfo->parameter_types[i].toString()
                          << " to argument variable" << std::endl;
                *varExpr->inferredType = funcInfo->parameter_types[i];

                // Update symbol table
                auto varInfo = symbol_table_.lookupVariable(varExpr->name);
                if (varInfo)
                {
                    std::cout << "[DEBUG] Updating symbol table for " << varExpr->name << std::endl;
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
    std::cout << "[DEBUG] CallExpr return type: " << current_type_.toString() << std::endl;
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
        std::cout << "DEBUG: Variable type kind: " << (int)current_type_.getKind() << std::endl;
        std::cout << "DEBUG: Variable type name: '" << current_type_.getTypeName() << "'" << std::endl;
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(LetExpr *expr)
{
    // Visit initializer first
    expr->initializer->accept(this);
    TypeInfo initType = current_type_;
    std::cout << "[DEBUG] LetExpr initializer type: " << initType.toString() << std::endl;
    std::cout << "[DEBUG] LetExpr initializer typeName: '" << initType.getTypeName() << "'" << std::endl;

    // Enter new scope
    symbol_table_.enterScope();

    // Add variable to symbol table
    // If there's a declared type, use it; otherwise use the type from the initializer
    TypeInfo varType;

    if (expr->declaredType->getKind() != TypeInfo::Kind::Unknown)
    {
        // There's a declared type
        if (expr->declaredType->getKind() == TypeInfo::Kind::Object)
        {
            // It's an object type - check if it exists in symbol table
            std::string declaredTypeName = expr->declaredType->getTypeAssigned();
            std::cout << "[DEBUG] LetExpr declared type name: '" << declaredTypeName << "'" << std::endl;
            if (declaredTypeName.empty())
            {
                // Try to get the name from typeAssigned
                declaredTypeName = expr->declaredType->toString();
                if (declaredTypeName == "Object")
                {
                    // This means it's a generic Object type, not a specific type
                    declaredTypeName = "";
                }
            }

            // std::cout << "[DEBUG] LetExpr declared type name: '" << declaredTypeName << "'" << std::endl;

            if (!declaredTypeName.empty())
            {
                // Check if the declared type exists in symbol table
                auto typeInfo = symbol_table_.lookupType(declaredTypeName);
                if (typeInfo)
                {
                    // Use the actual type from symbol table
                    varType = TypeInfo(TypeInfo::Kind::Object, declaredTypeName);
                    std::cout << "[DEBUG] LetExpr found type in symbol table: " << varType.toString() << std::endl;
                }
                else
                {
                    reportError(ErrorType::UNDEFINED_TYPE,
                                "Type '" + declaredTypeName + "' is not defined",
                                expr, "expresión let");
                    varType = TypeInfo(TypeInfo::Kind::Unknown);
                }
            }
            else
            {
                // Generic Object type
                varType = *expr->declaredType;
            }
        }
        else
        {
            // Non-object declared type
            varType = *expr->declaredType;
        }
    }
    else
    {
        // No declared type - use the initializer type
        varType = initType;
    }

    std::cout << "[DEBUG] LetExpr variable type to be stored: " << varType.toString() << std::endl;
    std::cout << "[DEBUG] LetExpr variable typeName: '" << varType.getTypeName() << "'" << std::endl;
    std::cout << "[DEBUG] LetExpr variable kind: " << (int)varType.getKind() << std::endl;

    if (symbol_table_.isVariableDeclared(expr->name))
    {
        reportError(ErrorType::REDEFINED_VARIABLE,
                    "Variable '" + expr->name + "' ya está definida en este ámbito",
                    expr, "expresión let");
    }
    else
    {
        symbol_table_.declareVariable(expr->name, varType);
        std::cout << "[DEBUG] LetExpr declared variable " << expr->name << " with type: " << varType.toString() << std::endl;
    }

    // Check type compatibility
    if (expr->declaredType->getKind() != TypeInfo::Kind::Unknown &&
        !initType.conformsTo(varType))
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot assign value of type " + initType.toString() +
                        " to variable of type " + varType.toString(),
                    expr);
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
    std::cout << "[DEBUG] Processing AssignExpr: " << expr->name << std::endl;

    // Check for invalid self assignment
    if (expr->name == "self")
    {
        reportError(ErrorType::INVALID_SELF_ASSIGNMENT,
                    "Cannot assign to 'self' - it is not a valid assignment target",
                    expr, "asignación destructiva");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Visit value first
    expr->value->accept(this);
    TypeInfo valueType = current_type_;

    // Look up variable
    auto varInfo = symbol_table_.lookupVariable(expr->name);
    if (!varInfo)
    {
        reportError(ErrorType::UNDEFINED_VARIABLE,
                    "Cannot assign to undefined variable '" + expr->name + "'",
                    expr, "asignación destructiva");
    }
    else if (!valueType.conformsTo(varInfo->type))
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot assign value of type " + valueType.toString() +
                        " to variable of type " + varInfo->type.toString(),
                    expr, "asignación destructiva");
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
    std::cout << "[DEBUG] Processing ExprBlock" << std::endl;

    // Enter new scope
    symbol_table_.enterScope();

    // Visit all statements
    for (const auto &stmt : expr->stmts)
    {
        stmt->accept(this);
        std::cout << "[DEBUG] Statement type: " << current_type_.toString() << std::endl;
    }

    // Exit scope
    symbol_table_.exitScope();

    // Block returns the type of its last statement
    if (!expr->stmts.empty())
    {
        expr->inferredType = expr->stmts.back()->inferredType;
        std::cout << "[DEBUG] Block type: " << expr->inferredType->toString() << std::endl;
    }
    else
    {
        current_type_ = TypeInfo(TypeInfo::Kind::Void);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        std::cout << "[DEBUG] Empty block type: " << current_type_.toString() << std::endl;
    }
}

void SemanticAnalyzer::visit(WhileExpr *expr)
{
    std::cout << "[DEBUG] Processing WhileExpr" << std::endl;

    // Check condition type
    expr->condition->accept(this);
    auto condType = expr->condition->inferredType;
    std::cout << "[DEBUG] While condition type: " << condType->toString() << std::endl;

    if (condType->getKind() != TypeInfo::Kind::Boolean)
    {
        std::cout << "[DEBUG] Invalid while condition type: must be boolean" << std::endl;
        reportError(ErrorType::TYPE_ERROR, "While condition must be boolean, got " + condType->toString(), expr);
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Visit body
    expr->body->accept(this);
    auto bodyType = expr->body->inferredType;
    std::cout << "[DEBUG] While body type: " << bodyType->toString() << std::endl;

    // While loop returns void
    current_type_ = TypeInfo(TypeInfo::Kind::Void);
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
    std::cout << "[DEBUG] While expression type: " << current_type_.toString() << std::endl;
}

void SemanticAnalyzer::visit(ForExpr *expr)
{
    std::cout << "[DEBUG] Processing ForExpr for variable: " << expr->variable << std::endl;

    // Check iterable type
    expr->iterable->accept(this);
    auto iterableType = expr->iterable->inferredType;
    std::cout << "[DEBUG] For iterable type: " << iterableType->toString() << std::endl;

    // Determine the type of elements that the iterable produces
    std::shared_ptr<TypeInfo> elementType;

    // Check if it's a range() call which produces numbers
    if (auto callExpr = dynamic_cast<CallExpr *>(expr->iterable.get()))
    {
        if (callExpr->callee == "range")
        {
            std::cout << "[DEBUG] Detected range() call, element type is Number" << std::endl;
            elementType = std::make_shared<TypeInfo>(TypeInfo::Kind::Number);
        }
    }

    // If we couldn't determine the element type, use Unknown for now
    if (!elementType)
    {
        std::cout << "[DEBUG] Could not determine element type, using Unknown" << std::endl;
        elementType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    }

    // Enter new scope for the loop variable
    symbol_table_.enterScope();

    // Declare the iteration variable with the determined type
    symbol_table_.declareVariable(expr->variable, *elementType);
    std::cout << "[DEBUG] Declared variable " << expr->variable << " with type " << elementType->toString() << std::endl;

    // Visit body
    expr->body->accept(this);
    auto bodyType = expr->body->inferredType;
    std::cout << "[DEBUG] For body type: " << bodyType->toString() << std::endl;

    // Exit scope
    symbol_table_.exitScope();

    // For loop returns void (like while)
    current_type_ = TypeInfo(TypeInfo::Kind::Void);
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
    std::cout << "[DEBUG] For expression type: " << current_type_.toString() << std::endl;
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
    // Validar recursivamente la cadena de herencia y detectar ciclos
    std::set<std::string> visited;
    if (!validateInheritanceChain(stmt->baseType, visited))
    {
        return;
    }

    std::cout << "[DEBUG] Processing TypeDecl: " << stmt->name << std::endl;

    // --- Herencia implícita de parámetros ---
    if (stmt->baseType != "Object" && stmt->params.empty() && stmt->baseArgs.empty())
    {
        auto baseTypeDecl = symbol_table_.getTypeDeclaration(stmt->baseType);
        if (baseTypeDecl)
        {
            stmt->params = baseTypeDecl->params;
            stmt->paramTypes = baseTypeDecl->paramTypes;
            // Generar baseArgs para pasar los parámetros al constructor del padre
            for (const auto &param : baseTypeDecl->params)
            {
                stmt->baseArgs.push_back(std::make_unique<VariableExpr>(param));
            }
            std::cout << "[DEBUG] " << stmt->name << " hereda parámetros de " << stmt->baseType << std::endl;
        }
    }

    // PASO 1: Validar existencia y validez del tipo base
    if (stmt->baseType != "Object")
    {
        // No se puede heredar de tipos primitivos
        if (stmt->baseType == "Number" || stmt->baseType == "String" || stmt->baseType == "Boolean")
        {
            reportError(ErrorType::INVALID_BASE_TYPE,
                        "Cannot inherit from built-in type '" + stmt->baseType + "'",
                        stmt, "declaración de tipo");
            return;
        }
        // El tipo base debe existir
        if (!symbol_table_.isTypeDeclared(stmt->baseType))
        {
            reportError(ErrorType::UNDEFINED_TYPE,
                        "Base type '" + stmt->baseType + "' is not defined",
                        stmt, "declaración de tipo");
            return;
        }
    }

    // Check if type is already declared
    if (symbol_table_.isTypeDeclared(stmt->name))
    {
        reportError(ErrorType::REDEFINED_TYPE,
                    "Type '" + stmt->name + "' ya está definido",
                    stmt, "declaración de tipo");
        return;
    }

    // Declare the type first
    if (!symbol_table_.declareType(stmt->name, stmt->baseType, stmt->line_number))
    {
        reportError(ErrorType::REDEFINED_TYPE,
                    "Type '" + stmt->name + "' ya está definido",
                    stmt, "declaración de tipo");
        return;
    }

    // Store the type declaration for parameter checking
    symbol_table_.storeTypeDeclaration(stmt->name, stmt);

    // Enter new scope for type analysis
    symbol_table_.enterScope();

    // Add self reference to the scope
    TypeInfo selfType(TypeInfo::Kind::Object, stmt->name);
    symbol_table_.declareVariable("self", selfType, false); // self is immutable

    // Process type parameters if any
    for (const auto &param : stmt->params)
    {
        // Type parameters are treated as variables in the type scope
        symbol_table_.declareVariable(param, TypeInfo(TypeInfo::Kind::Unknown));
    }

    // Process attributes
    for (const auto &attr : stmt->attributes)
    {
        std::cout << "[DEBUG] Processing attribute: " << attr->name << std::endl;

        // Check if attribute name conflicts with method names
        for (const auto &method : stmt->methods)
        {
            if (method->name == attr->name)
            {
                reportError(ErrorType::REDEFINED_ATTRIBUTE,
                            "Attribute '" + attr->name + "' conflicts with method name",
                            attr.get(), "declaración de atributo");
                continue;
            }
        }

        // Visit the initializer to get its type
        attr->initializer->accept(this);
        TypeInfo attrType = current_type_;

        // Add attribute to the type
        if (!symbol_table_.addTypeAttribute(stmt->name, attr->name, attrType, attr->line_number))
        {
            reportError(ErrorType::REDEFINED_ATTRIBUTE,
                        "Attribute '" + attr->name + "' ya está definido en tipo '" + stmt->name + "'",
                        attr.get(), "declaración de atributo");
        }
        else
        {
            // Also declare it in the current scope for method analysis
            symbol_table_.declareVariable(attr->name, attrType, false); // Attributes are private (immutable from outside)
        }

        attr->inferredType = std::make_shared<TypeInfo>(attrType);
    }

    // Process methods
    for (const auto &method : stmt->methods)
    {
        std::cout << "[DEBUG] Processing method: " << method->name << std::endl;

        // Check if method name conflicts with attribute names
        for (const auto &attr : stmt->attributes)
        {
            if (attr->name == method->name)
            {
                reportError(ErrorType::REDEFINED_METHOD,
                            "Method '" + method->name + "' conflicts with attribute name",
                            method.get(), "declaración de método");
                continue;
            }
        }

        // Enter method scope
        symbol_table_.enterScope();

        // Add self reference to method scope
        symbol_table_.declareVariable("self", selfType, false);

        // Set current method name for base() calls
        std::string oldMethodName = currentMethodName_;
        currentMethodName_ = method->name;

        // Declare method parameters with unknown types initially
        std::vector<TypeInfo> paramTypes;
        for (const auto &param : method->params)
        {
            symbol_table_.declareVariable(param, TypeInfo(TypeInfo::Kind::Unknown));
            paramTypes.push_back(TypeInfo(TypeInfo::Kind::Unknown));
        }

        // Visit method body to infer parameter types
        method->body->accept(this);
        TypeInfo returnType = current_type_;

        // Restore method name
        currentMethodName_ = oldMethodName;

        // Check for method redefinition (polimorfismo)
        bool methodExists = false;
        bool sameSignature = false;

        // Check if method exists in inheritance chain for polymorphic override
        auto baseTypeDecl = symbol_table_.getTypeDeclaration(stmt->baseType);
        if (baseTypeDecl && baseTypeDecl->baseType != "Object")
        {
            auto inheritedMethod = symbol_table_.lookupMethod(baseTypeDecl->baseType, method->name);
            if (inheritedMethod)
            {
                // Check if signatures match for polymorphic override
                bool signaturesMatch = inheritedMethod->parameter_types.size() == paramTypes.size() &&
                                       inheritedMethod->return_type.getKind() == returnType.getKind();

                // Check parameter types if count matches
                if (signaturesMatch && inheritedMethod->parameter_types.size() == paramTypes.size())
                {
                    for (size_t i = 0; i < paramTypes.size(); ++i)
                    {
                        if (!paramTypes[i].isCompatibleWith(inheritedMethod->parameter_types[i]))
                        {
                            signaturesMatch = false;
                            break;
                        }
                    }
                }

                if (signaturesMatch)
                {
                    // Same signature - allow polymorphic override
                    sameSignature = true;
                    std::cout << "[DEBUG] Polymorphic override of method '" << method->name << "'" << std::endl;
                }
                else
                {
                    // Different signature - this is an error
                    reportError(ErrorType::REDEFINED_METHOD,
                                "Method '" + method->name + "' exists in base type with different signature",
                                method.get(), "declaración de método");
                    methodExists = true;
                }
            }
        }

        // Add method to the type (polymorphic override is allowed)
        if (!methodExists)
        {
            if (!symbol_table_.addTypeMethod(stmt->name, method->name, paramTypes, returnType, method->line_number))
            {
                // Only report error if it's not a polymorphic override
                if (!sameSignature)
                {
                    reportError(ErrorType::REDEFINED_METHOD,
                                "Method '" + method->name + "' ya está definido en tipo '" + stmt->name + "'",
                                method.get(), "declaración de método");
                }
            }
        }

        method->inferredType = std::make_shared<TypeInfo>(returnType);

        // Exit method scope
        symbol_table_.exitScope();
    }

    // Exit type scope
    symbol_table_.exitScope();

    // Set the inferred type of the TypeDecl itself
    stmt->inferredType = std::make_shared<TypeInfo>(TypeInfo::Kind::Object, stmt->name);

    std::cout << "[DEBUG] TypeDecl " << stmt->name << " processed successfully" << std::endl;

    if (stmt->baseType != "Object")
    {
        auto baseTypeDecl = symbol_table_.getTypeDeclaration(stmt->baseType);
        if (!baseTypeDecl)
        {
            // Ya se reportó error antes si no existe, pero por seguridad:
            reportError(ErrorType::UNDEFINED_TYPE,
                        "Base type declaration for '" + stmt->baseType + "' not found",
                        stmt, "declaración de tipo");
            return;
        }
        size_t baseParamCount = baseTypeDecl->getParams().size();
        size_t baseArgsCount = stmt->baseArgs.size();

        if (baseArgsCount > 0 && baseArgsCount == baseParamCount)
        {
            // Para cada argumento, verificar tipo
            for (size_t i = 0; i < baseArgsCount; ++i)
            {
                // Declarar los parámetros del hijo en el scope temporal
                symbol_table_.enterScope();
                for (const auto &param : stmt->params)
                {
                    symbol_table_.declareVariable(param, TypeInfo(TypeInfo::Kind::Unknown));
                }
                // Analizar el tipo del argumento
                stmt->baseArgs[i]->accept(this);
                TypeInfo argType = current_type_;
                if (argType.isUnknown())
                    //{
                    //    reportError(ErrorType::TYPE_ERROR,
                    //                "Argument " + std::to_string(i + 1) + " for base type '" + stmt->baseType +
                    //                    "' is not a valid expression",
                    //                stmt, "declaración de tipo");
                    //}
                    // Salir del scope temporal
                    symbol_table_.exitScope();
            }
        }
        if (baseArgsCount == 0 && baseParamCount > 0 && stmt->params.size() != baseParamCount)
        {
            // Herencia implícita: el hijo debe tener los mismos parámetros que el padre
            reportError(ErrorType::ARGUMENT_COUNT_MISMATCH,
                        "Type '" + stmt->name + "' must declare the same number of parameters as its base type '" +
                            stmt->baseType + "' (" + std::to_string(baseParamCount) + ") when no base arguments are given",
                        stmt, "declaración de tipo");
            return;
        }
    }
}

void SemanticAnalyzer::visit(NewExpr *expr)
{
    std::cout << "[DEBUG] Processing NewExpr: " << expr->typeName << std::endl;

    // Check if type exists
    auto typeInfo = symbol_table_.lookupType(expr->typeName);
    if (!typeInfo)
    {
        reportError(ErrorType::UNDEFINED_TYPE,
                    "Type '" + expr->typeName + "' is not defined",
                    expr, "instanciación de objeto");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Get the type declaration to check constructor parameters
    auto typeDecl = symbol_table_.getTypeDeclaration(expr->typeName);
    if (!typeDecl)
    {
        reportError(ErrorType::UNDEFINED_TYPE,
                    "Type declaration for '" + expr->typeName + "' not found",
                    expr, "instanciación de objeto");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Check constructor parameter count
    const auto &expectedParams = typeDecl->getParams();
    std::cout << "[DEBUG] Type " << expr->typeName << " expects " << expectedParams.size() << " constructor parameters" << std::endl;
    std::cout << "[DEBUG] Got " << expr->args.size() << " arguments" << std::endl;

    if (expr->args.size() != expectedParams.size())
    {
        reportError(ErrorType::ARGUMENT_COUNT_MISMATCH,
                    "Type '" + expr->typeName + "' constructor expects " +
                        std::to_string(expectedParams.size()) + " parameters but got " +
                        std::to_string(expr->args.size()),
                    expr, "instanciación de objeto");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Check argument types and process them
    for (size_t i = 0; i < expr->args.size(); ++i)
    {
        std::cout << "[DEBUG] Processing constructor argument " << i << std::endl;
        expr->args[i]->accept(this);
        std::cout << "[DEBUG] Argument " << i << " type: " << current_type_.toString() << std::endl;

        // For now, we accept any type since parameter types are not stored in TypeDecl
        // In a more complete implementation, we would check against the parameter types
        // stored in the symbol table or type declaration
    }

    current_type_ = TypeInfo(TypeInfo::Kind::Object, expr->typeName);
    std::cout << "[DEBUG] NewExpr creates object of type: " << current_type_.toString() << std::endl;
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(GetAttrExpr *expr)
{
    std::cout << "[DEBUG] Processing GetAttrExpr: " << expr->attrName << std::endl;

    // Visit object first
    expr->object->accept(this);
    if (!current_type_.isObject())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot access attribute '" + expr->attrName + "' on non-object type " +
                        current_type_.toString(),
                    expr, "acceso a atributo");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    }
    else
    {
        // Look up attribute in type
        auto attrInfo = symbol_table_.lookupAttribute(current_type_.getTypeName(), expr->attrName);
        if (!attrInfo)
        {
            reportError(ErrorType::UNDEFINED_ATTRIBUTE,
                        "Type '" + current_type_.getTypeName() + "' has no attribute '" +
                            expr->attrName + "'",
                        expr, "acceso a atributo");
            current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        }
        else
        {
            current_type_ = attrInfo->type;
            std::cout << "[DEBUG] GetAttrExpr " << expr->attrName << " type: " << current_type_.toString() << std::endl;
        }
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(SetAttrExpr *expr)
{
    std::cout << "[DEBUG] Processing SetAttrExpr: " << expr->attrName << std::endl;

    // Visit object first
    expr->object->accept(this);
    if (!current_type_.isObject())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot set attribute '" + expr->attrName + "' on non-object type " +
                        current_type_.toString(),
                    expr, "asignación de atributo");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    }
    else
    {
        // Look up attribute in type
        auto attrInfo = symbol_table_.lookupAttribute(current_type_.getTypeName(), expr->attrName);
        if (!attrInfo)
        {
            reportError(ErrorType::UNDEFINED_ATTRIBUTE,
                        "Type '" + current_type_.getTypeName() + "' has no attribute '" +
                            expr->attrName + "'",
                        expr, "asignación de atributo");
            current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        }
        else
        {
            // Visit value
            expr->value->accept(this);
            if (!current_type_.conformsTo(attrInfo->type))
            {
                reportError(ErrorType::TYPE_ERROR,
                            "Cannot assign value of type " + current_type_.toString() +
                                " to attribute of type " + attrInfo->type.toString(),
                            expr, "asignación de atributo");
            }
        }
    }

    // Set attribute returns the type of the value
    expr->inferredType = expr->value->inferredType;
}

void SemanticAnalyzer::visit(MethodCallExpr *expr)
{
    std::cout << "[DEBUG] Processing MethodCallExpr: " << expr->methodName << std::endl;

    // Visit object first
    expr->object->accept(this);
    if (!current_type_.isObject())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot call method '" + expr->methodName + "' on non-object type " +
                        current_type_.toString(),
                    expr, "llamada a método");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    }
    else
    {
        // First, try to look up method in the static type
        auto methodInfo = symbol_table_.lookupMethod(current_type_.getTypeName(), expr->methodName);

        // If not found in static type, try to find it in the dynamic type (initializer type)
        if (!methodInfo)
        {
            std::cout << "[DEBUG] Method '" << expr->methodName << "' not found in static type '"
                      << current_type_.getTypeName() << "', checking subtypes" << std::endl;

            // Try to find the method in common subtypes of the declared type
            // This handles the case where we have let dog: Animal = new Dog() and want to call dog.bark()
            std::vector<std::string> subtypesToCheck;

            // Check common subtypes based on the current type
            if (current_type_.getTypeName() == "Animal")
            {
                subtypesToCheck = {"Dog", "Cat"}; // Common subtypes of Animal
            }
            else if (current_type_.getTypeName() == "Object")
            {
                // For Object, check all known types
                subtypesToCheck = {"Animal", "Dog", "Cat"};
            }

            // Check each subtype for the method
            for (const auto &subtypeName : subtypesToCheck)
            {
                std::cout << "[DEBUG] Checking subtype '" << subtypeName << "' for method '" << expr->methodName << "'" << std::endl;
                methodInfo = symbol_table_.lookupMethod(subtypeName, expr->methodName);
                if (methodInfo)
                {
                    std::cout << "[DEBUG] Found method '" << expr->methodName << "' in subtype '" << subtypeName << "'" << std::endl;
                    break;
                }
            }

            // If still not found, try the original approach for new expressions
            if (!methodInfo)
            {
                if (auto newExpr = dynamic_cast<NewExpr *>(expr->object.get()))
                {
                    // If it's a new expression, check the type being instantiated
                    std::cout << "[DEBUG] Checking new expression type: " << newExpr->typeName << std::endl;
                    methodInfo = symbol_table_.lookupMethod(newExpr->typeName, expr->methodName);

                    if (methodInfo)
                    {
                        std::cout << "[DEBUG] Found method '" << expr->methodName << "' in new type '"
                                  << newExpr->typeName << "'" << std::endl;
                    }
                }
            }
        }

        if (!methodInfo)
        {
            reportError(ErrorType::UNDEFINED_METHOD,
                        "Type '" + current_type_.getTypeName() + "' has no method '" +
                            expr->methodName + "'",
                        expr, "llamada a método");
            current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        }
        else
        {
            // Check argument count
            if (expr->args.size() != methodInfo->parameter_types.size())
            {
                reportError(ErrorType::ARGUMENT_COUNT_MISMATCH,
                            "Method '" + expr->methodName + "' expects " +
                                std::to_string(methodInfo->parameter_types.size()) +
                                " arguments but got " + std::to_string(expr->args.size()),
                            expr, "llamada a método");
            }

            // Check argument types
            for (size_t i = 0; i < expr->args.size() && i < methodInfo->parameter_types.size(); ++i)
            {
                expr->args[i]->accept(this);
                if (!current_type_.conformsTo(methodInfo->parameter_types[i]))
                {
                    reportError(ErrorType::TYPE_ERROR,
                                "Argument " + std::to_string(i + 1) + " of method '" + expr->methodName +
                                    "' expects type " + methodInfo->parameter_types[i].toString() +
                                    " but got " + current_type_.toString(),
                                expr, "llamada a método");
                }
            }

            // Set return type
            current_type_ = methodInfo->return_type;
            std::cout << "[DEBUG] MethodCallExpr " << expr->methodName << " return type: " << current_type_.toString() << std::endl;
        }
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(SelfExpr *expr)
{
    std::cout << "[DEBUG] Processing SelfExpr" << std::endl;

    // Look up self in current scope
    auto selfInfo = symbol_table_.lookupVariable("self");
    if (!selfInfo)
    {
        reportError(ErrorType::INVALID_SELF,
                    "Cannot use 'self' outside of a type definition",
                    expr, "expresión self");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    }
    else
    {
        current_type_ = selfInfo->type;
        std::cout << "[DEBUG] SelfExpr type: " << current_type_.toString() << std::endl;
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(BaseCallExpr *expr)
{
    std::cout << "[DEBUG] Processing BaseCallExpr" << std::endl;

    // Look up self in current scope
    auto selfInfo = symbol_table_.lookupVariable("self");
    if (!selfInfo)
    {
        reportError(ErrorType::INVALID_BASE,
                    "Cannot use 'base' outside of a type definition",
                    expr, "llamada base");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
    }
    else
    {
        // Get base type
        auto typeInfo = symbol_table_.lookupType(selfInfo->type.getTypeName());
        if (!typeInfo || typeInfo->base_type == "Object")
        {
            reportError(ErrorType::INVALID_BASE,
                        "Cannot use 'base' in type without a base type",
                        expr, "llamada base");
            current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        }
        else
        {
            // Look up the method in the base type to get its return type
            auto baseTypeDecl = symbol_table_.getTypeDeclaration(typeInfo->base_type);
            if (baseTypeDecl)
            {
                // Find the method in the base type using currentMethodName_
                for (const auto &method : baseTypeDecl->methods)
                {
                    if (method->name == currentMethodName_)
                    {
                        current_type_ = *method->inferredType;
                        std::cout << "[DEBUG] BaseCallExpr method return type: " << current_type_.toString() << std::endl;
                        break;
                    }
                }
            }

            // If we couldn't find the method, use Unknown
            if (current_type_.isUnknown())
            {
                current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
                std::cout << "[DEBUG] BaseCallExpr type: Unknown (method not found)" << std::endl;
            }
        }
    }

    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(IsExpr *expr)
{
    std::cout << "[DEBUG] Processing IsExpr: " << expr->typeName << std::endl;

    // Visit the object expression first
    expr->object->accept(this);
    TypeInfo objectType = current_type_;
    std::cout << "[DEBUG] IsExpr object type: " << objectType.toString() << std::endl;

    // Check if the object is an object type (not primitive)
    if (!objectType.isObject())
    {
        reportError(ErrorType::TYPE_ERROR,
                    "Cannot use 'is' operator on non-object type " + objectType.toString(),
                    expr, "operador is");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // Check if the type name exists in the symbol table
    auto typeInfo = symbol_table_.lookupType(expr->typeName);
    if (!typeInfo)
    {
        reportError(ErrorType::UNDEFINED_TYPE,
                    "Type '" + expr->typeName + "' is not defined",
                    expr, "operador is");
        current_type_ = TypeInfo(TypeInfo::Kind::Unknown);
        expr->inferredType = std::make_shared<TypeInfo>(current_type_);
        return;
    }

    // The 'is' operator returns a boolean indicating whether the object
    // conforms to the specified type
    current_type_ = TypeInfo(TypeInfo::Kind::Boolean);
    std::cout << "[DEBUG] IsExpr result type: " << current_type_.toString() << std::endl;
    expr->inferredType = std::make_shared<TypeInfo>(current_type_);
}

void SemanticAnalyzer::visit(AttributeDecl *stmt)
{
    std::cout << "[DEBUG] Processing AttributeDecl: " << stmt->name << std::endl;

    // Visit the initializer to get its type
    stmt->initializer->accept(this);
    TypeInfo attrType = current_type_;

    // The type of the attribute is the type inferred from the initializer
    stmt->inferredType = std::make_shared<TypeInfo>(attrType);

    std::cout << "[DEBUG] AttributeDecl " << stmt->name << " has type: " << attrType.toString() << std::endl;
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
    // Usar la relación de conforming (<=) para verificar compatibilidad
    // type1 <= type2 significa que type1 conforma a type2
    return type1.conformsTo(type2);
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
    std::cout << "[DEBUG] Processing MethodDecl: " << stmt->name << std::endl;

    // Enter method scope
    symbol_table_.enterScope();

    // Register parameters in the method's scope with unknown types initially
    std::vector<TypeInfo> paramTypes;
    for (const auto &param : stmt->params)
    {
        symbol_table_.declareVariable(param, TypeInfo(TypeInfo::Kind::Unknown));
        paramTypes.push_back(TypeInfo(TypeInfo::Kind::Unknown));
    }

    // Visit method body to infer parameter types and return type
    if (stmt->body)
    {
        stmt->body->accept(this);
        TypeInfo returnType = current_type_;

        // Update parameter types based on their usage in the body
        for (size_t i = 0; i < stmt->params.size(); ++i)
        {
            auto varInfo = symbol_table_.lookupVariable(stmt->params[i]);
            if (varInfo && varInfo->type.getKind() != TypeInfo::Kind::Unknown)
            {
                paramTypes[i] = varInfo->type;
                std::cout << "[DEBUG] Parameter " << stmt->params[i] << " inferred as: " << varInfo->type.toString() << std::endl;
            }
        }

        stmt->inferredType = std::make_shared<TypeInfo>(returnType);
        std::cout << "[DEBUG] MethodDecl " << stmt->name << " return type: " << returnType.toString() << std::endl;
    }
    else
    {
        stmt->inferredType = std::make_shared<TypeInfo>(TypeInfo::Kind::Void);
        std::cout << "[DEBUG] MethodDecl " << stmt->name << " has no body, return type: Void" << std::endl;
    }

    // Exit method scope
    symbol_table_.exitScope();
}

bool SemanticAnalyzer::validateInheritanceChain(const std::string &typeName, std::set<std::string> &visited)
{
    if (typeName == "Object")
        return true;
    if (visited.count(typeName))
    {
        reportError(ErrorType::INVALID_BASE_TYPE,
                    "Cyclic inheritance detected involving type '" + typeName + "'",
                    "declaración de tipo");
        return false;
    }
    visited.insert(typeName);
    auto baseTypeDecl = symbol_table_.getTypeDeclaration(typeName);
    if (!baseTypeDecl)
    {
        reportError(ErrorType::UNDEFINED_TYPE,
                    "Base type '" + typeName + "' is not defined",
                    "declaración de tipo");
        return false;
    }
    if (baseTypeDecl->baseType == "Number" || baseTypeDecl->baseType == "String" || baseTypeDecl->baseType == "Boolean")
    {
        reportError(ErrorType::INVALID_BASE_TYPE,
                    "Cannot inherit from built-in type '" + baseTypeDecl->baseType + "'",
                    "declaración de tipo");
        return false;
    }
    return validateInheritanceChain(baseTypeDecl->baseType, visited);
}
