#include "codegen.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

CodeGenerator::CodeGenerator()
    : current_function_(""),
      current_value_(""),
      current_scope_(nullptr),
      var_counter_(0),
      builtins_registered_(false)
{
    // Create initial scope
    scopes_.push_back(std::make_unique<Scope>());
    current_scope_ = scopes_.back().get();
}

CodeGenerator::~CodeGenerator() = default;

void CodeGenerator::initialize(const std::string &module_name)
{
    module_name_ = module_name;
    ir_code_.str(""); // Clear the stream
    ir_code_.clear();
    registerBuiltinFunctions();
}

void CodeGenerator::registerBuiltinFunctions()
{
    if (builtins_registered_)
        return;

    // Add built-in function declarations
    ir_code_ << "; Built-in function declarations\n";
    ir_code_ << "declare double @sqrt(double)\n";
    ir_code_ << "declare double @sin(double)\n";
    ir_code_ << "declare double @cos(double)\n";
    ir_code_ << "declare double @tan(double)\n";
    ir_code_ << "declare double @exp(double)\n";
    ir_code_ << "declare double @log(double)\n";
    ir_code_ << "declare i32 @rand()\n";
    ir_code_ << "declare i32 @printf(i8*, ...)\n";
    ir_code_ << "declare i32 @sprintf(i8*, i8*, ...)\n";
    ir_code_ << "declare i8* @malloc(i32)\n";
    ir_code_ << "declare void @free(i8*)\n";
    ir_code_ << "\n";

    // Add print functions for different types
    ir_code_ << "; Print functions for different types\n";

    // Print double
    ir_code_ << "define double @print_double(double %value) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %format = getelementptr [4 x i8], [4 x i8]* @.str_double, i32 0, i32 0\n";
    ir_code_ << "  %result = call i32 (i8*, ...) @printf(i8* %format, double %value)\n";
    ir_code_ << "  ret double %value\n";
    ir_code_ << "}\n\n";

    // Print boolean
    ir_code_ << "define double @print_bool(i1 %value) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %format_true = getelementptr [6 x i8], [6 x i8]* @.str_bool_true, i32 0, i32 0\n";
    ir_code_ << "  %format_false = getelementptr [7 x i8], [7 x i8]* @.str_bool_false, i32 0, i32 0\n";
    ir_code_ << "  %format = select i1 %value, i8* %format_true, i8* %format_false\n";
    ir_code_ << "  %result = call i32 (i8*, ...) @printf(i8* %format)\n";
    ir_code_ << "  %value_double = uitofp i1 %value to double\n";
    ir_code_ << "  ret double %value_double\n";
    ir_code_ << "}\n\n";

    // Print string
    ir_code_ << "define double @print_string(i8* %value) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %format = getelementptr [4 x i8], [4 x i8]* @.str_string, i32 0, i32 0\n";
    ir_code_ << "  %result = call i32 (i8*, ...) @printf(i8* %format, i8* %value)\n";
    ir_code_ << "  ret double 0.0\n";
    ir_code_ << "}\n\n";

    // Generic print function that dispatches based on type
    ir_code_ << "define double @print(double %value) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %result = call double @print_double(double %value)\n";
    ir_code_ << "  ret double %result\n";
    ir_code_ << "}\n\n";

    // String conversion functions
    ir_code_ << "; String conversion functions\n";

    // Convert double to string
    ir_code_ << "define i8* @double_to_string(double %value) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %buffer = call i8* @malloc(i32 64)\n";
    ir_code_ << "  %format = getelementptr [4 x i8], [4 x i8]* @.str_double_format, i32 0, i32 0\n";
    ir_code_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, double %value)\n";
    ir_code_ << "  ret i8* %buffer\n";
    ir_code_ << "}\n\n";

    // Convert boolean to string
    ir_code_ << "define i8* @bool_to_string(i1 %value) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %buffer = call i8* @malloc(i32 8)\n";
    ir_code_ << "  %true_str = getelementptr [5 x i8], [5 x i8]* @.str_true, i32 0, i32 0\n";
    ir_code_ << "  %false_str = getelementptr [6 x i8], [6 x i8]* @.str_false, i32 0, i32 0\n";
    ir_code_ << "  %str_to_copy = select i1 %value, i8* %true_str, i8* %false_str\n";
    ir_code_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %str_to_copy)\n";
    ir_code_ << "  ret i8* %buffer\n";
    ir_code_ << "}\n\n";

    // String concatenation function
    ir_code_ << "define i8* @concat_strings(i8* %str1, i8* %str2) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %buffer = call i8* @malloc(i32 256)\n";
    ir_code_ << "  %format = getelementptr [7 x i8], [7 x i8]* @.str_concat_format, i32 0, i32 0\n";
    ir_code_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)\n";
    ir_code_ << "  ret i8* %buffer\n";
    ir_code_ << "}\n\n";

    // String concatenation with space function
    ir_code_ << "define i8* @concat_strings_ws(i8* %str1, i8* %str2) {\n";
    ir_code_ << "entry:\n";
    ir_code_ << "  %buffer = call i8* @malloc(i32 256)\n";
    ir_code_ << "  %format = getelementptr [10 x i8], [10 x i8]* @.str_concat_ws_format, i32 0, i32 0\n";
    ir_code_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)\n";
    ir_code_ << "  ret i8* %buffer\n";
    ir_code_ << "}\n\n";

    // Add string constants for formatting
    ir_code_ << "@.str_double = private unnamed_addr constant [4 x i8] c\"%f\\0A\\00\"\n";
    ir_code_ << "@.str_bool_true = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n";
    ir_code_ << "@.str_bool_false = private unnamed_addr constant [7 x i8] c\"false\\0A\\00\"\n";
    ir_code_ << "@.str_string = private unnamed_addr constant [4 x i8] c\"%s\\0A\\00\"\n";
    ir_code_ << "@.str_double_format = private unnamed_addr constant [3 x i8] c\"%f\\00\"\n";
    ir_code_ << "@.str_true = private unnamed_addr constant [5 x i8] c\"true\\00\"\n";
    ir_code_ << "@.str_false = private unnamed_addr constant [6 x i8] c\"false\\00\"\n";
    ir_code_ << "@.str_concat_format = private unnamed_addr constant [5 x i8] c\"%s%s\\00\"\n";
    ir_code_ << "@.str_concat_ws_format = private unnamed_addr constant [6 x i8] c\"%s %s\\00\"\n";
    ir_code_ << "\n";

    builtins_registered_ = true;
}

std::string CodeGenerator::getLLVMType(const TypeInfo &type)
{
    switch (type.getKind())
    {
    case TypeInfo::Kind::Number:
        return "double";
    case TypeInfo::Kind::String:
        return "i8*";
    case TypeInfo::Kind::Boolean:
        return "i1";
    case TypeInfo::Kind::Void:
        return "double"; // Don't use void for variables
    case TypeInfo::Kind::Object:
        return "i8*"; // For now, treat objects as opaque pointers
    default:
        return "double"; // Default to double
    }
}

std::string CodeGenerator::createConstant(const TypeInfo &type, const std::string &value)
{
    switch (type.getKind())
    {
    case TypeInfo::Kind::Number:
        return value;
    case TypeInfo::Kind::String:
        return "@.str." + std::to_string(var_counter_++);
    case TypeInfo::Kind::Boolean:
        return value == "true" ? "1" : "0";
    default:
        return "0";
    }
}

std::string CodeGenerator::generateUniqueName(const std::string &prefix)
{
    return prefix + "_" + std::to_string(var_counter_++);
}

void CodeGenerator::enterScope()
{
    scopes_.push_back(std::make_unique<Scope>(current_scope_));
    current_scope_ = scopes_.back().get();
}

void CodeGenerator::exitScope()
{
    if (current_scope_->parent)
    {
        current_scope_ = current_scope_->parent;
        scopes_.pop_back();
    }
}

void CodeGenerator::generateCode(Program *program)
{
    program->accept(this);
}

void CodeGenerator::printIR()
{
    std::cout << global_constants_.str() << function_definitions_.str() << ir_code_.str() << std::endl;
}

void CodeGenerator::writeIRToFile(const std::string &filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << global_constants_.str() << function_definitions_.str() << ir_code_.str();
        file.close();
    }
    else
    {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

// StmtVisitor implementations
void CodeGenerator::visit(Program *stmt)
{
    std::cout << "[CodeGen] Processing Program with " << stmt->stmts.size() << " statements" << std::endl;

    // Register built-in functions first
    registerBuiltinFunctions();

    // Generate main function
    ir_code_ << "define i32 @main() {\n";
    ir_code_ << "entry:\n";

    current_function_ = "main";

    // Process all statements
    for (auto &s : stmt->stmts)
    {
        s->accept(this);
    }

    // Return 0 from main
    ir_code_ << "  ret i32 0\n";
    ir_code_ << "}\n\n";

    current_function_ = "";
}

void CodeGenerator::visit(ExprStmt *stmt)
{
    std::cout << "[CodeGen] Processing ExprStmt" << std::endl;
    stmt->expr->accept(this);
}

void CodeGenerator::visit(FunctionDecl *stmt)
{
    std::cout << "[CodeGen] Processing FunctionDecl: " << stmt->name << std::endl;

    // Get return type
    std::string return_type = getLLVMType(*stmt->returnType);

    // Get parameter types - use i8* for Unknown types, preserve known types
    std::string param_types;
    for (size_t i = 0; i < stmt->paramTypes.size(); ++i)
    {
        if (i > 0)
            param_types += ", ";

        std::string param_type;
        if (stmt->paramTypes[i]->getKind() == TypeInfo::Kind::Unknown)
        {
            // For unknown types, use i8* as a dynamic type that can hold any value
            param_type = "i8*";
        }
        else
        {
            // For known types, use the actual LLVM type
            param_type = getLLVMType(*stmt->paramTypes[i]);
        }
        param_types += param_type;
    }

    // Create function in the function definitions stream
    function_definitions_ << "define " << return_type << " @" << stmt->name << "(" << param_types << ") {\n";
    function_definitions_ << "entry:\n";

    current_function_ = stmt->name;

    // Set up parameters
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::string param_name = stmt->params[i];
        std::string param_type;
        if (stmt->paramTypes[i]->getKind() == TypeInfo::Kind::Unknown)
        {
            param_type = "i8*";
        }
        else
        {
            param_type = getLLVMType(*stmt->paramTypes[i]);
        }
        std::string alloca_name = generateUniqueName("param");

        function_definitions_ << "  %" << alloca_name << " = alloca " << param_type << "\n";
        function_definitions_ << "  store " << param_type << " %" << i << ", " << param_type << "* %" << alloca_name << "\n";
        current_scope_->variables[param_name] = alloca_name;
    }

    // Generate function body
    stmt->body->accept(this);

    // If no return statement was generated, add one
    if (return_type != "void")
    {
        if (stmt->returnType && stmt->returnType->getKind() == TypeInfo::Kind::String)
        {
            // For string functions, check if the body is a string expression
            if (stmt->body->inferredType && stmt->body->inferredType->getKind() == TypeInfo::Kind::String)
            {
                // The body is already a string expression, use its value
                function_definitions_ << "  ret " << return_type << " " << current_value_ << "\n";
            }
            else
            {
                // Return a default string constant
                std::string str_name = registerStringConstant("default");
                function_definitions_ << "  ret " << return_type << " " << str_name << "\n";
            }
        }
        else
        {
            function_definitions_ << "  ret " << return_type << " " << current_value_ << "\n";
        }
    }
    else
    {
        function_definitions_ << "  ret void\n";
    }

    function_definitions_ << "}\n\n";
    current_function_ = "";
}

void CodeGenerator::visit(TypeDecl *stmt)
{
    std::cout << "[CodeGen] Processing TypeDecl: " << stmt->name << std::endl;

    // For now, we'll create a simple struct type
    std::string struct_name = "%struct." + stmt->name;
    ir_code_ << struct_name << " = type { ";

    for (size_t i = 0; i < stmt->attributes.size(); ++i)
    {
        if (i > 0)
            ir_code_ << ", ";
        ir_code_ << "double"; // For now, assume all attributes are numbers
    }

    if (stmt->attributes.empty())
    {
        ir_code_ << "i8"; // Empty struct needs at least one field
    }

    ir_code_ << " }\n";
    types_[stmt->name] = struct_name;
}

void CodeGenerator::visit(MethodDecl *stmt)
{
    std::cout << "[CodeGen] Processing MethodDecl: " << stmt->name << std::endl;
    // Method generation would go here
}

void CodeGenerator::visit(AttributeDecl *stmt)
{
    std::cout << "[CodeGen] Processing AttributeDecl: " << stmt->name << std::endl;
    // Attribute initialization would go here
}

// ExprVisitor implementations
void CodeGenerator::visit(NumberExpr *expr)
{
    std::cout << "[CodeGen] Processing NumberExpr: " << expr->value << std::endl;
    current_value_ = std::to_string(expr->value);
}

void CodeGenerator::visit(StringExpr *expr)
{
    std::cout << "[CodeGen] Processing StringExpr: " << expr->value << std::endl;
    current_value_ = registerStringConstant(expr->value);
}

void CodeGenerator::visit(BooleanExpr *expr)
{
    std::cout << "[CodeGen] Processing BooleanExpr: " << expr->value << std::endl;

    // Always generate boolean values as doubles (1.0 or 0.0) for consistency
    // since we always use double for numbers and comparisons
    current_value_ = expr->value ? "1.0" : "0.0";
}

void CodeGenerator::visit(UnaryExpr *expr)
{
    std::cout << "[CodeGen] Processing UnaryExpr" << std::endl;
    expr->operand->accept(this);
    std::string operand = current_value_;

    std::string result_name = generateUniqueName("unary");

    switch (expr->op)
    {
    case UnaryExpr::OP_NEG:
        getCurrentStream() << "  %" << result_name << " = fneg double " << operand << "\n";
        break;
    }

    current_value_ = "%" + result_name;
}

void CodeGenerator::visit(BinaryExpr *expr)
{
    std::cout << "[CodeGen] Processing BinaryExpr" << std::endl;

    expr->left->accept(this);
    std::string left = current_value_;

    expr->right->accept(this);
    std::string right = current_value_;

    std::string result_name = generateUniqueName("binary");

    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
        getCurrentStream() << "  %" << result_name << " = fadd double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_SUB:
        getCurrentStream() << "  %" << result_name << " = fsub double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_MUL:
        getCurrentStream() << "  %" << result_name << " = fmul double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_DIV:
        getCurrentStream() << "  %" << result_name << " = fdiv double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_LT:
        getCurrentStream() << "  %" << result_name << " = fcmp ult double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_GT:
        getCurrentStream() << "  %" << result_name << " = fcmp ugt double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_EQ:
        getCurrentStream() << "  %" << result_name << " = fcmp ueq double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_NEQ:
        getCurrentStream() << "  %" << result_name << " = fcmp une double " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_AND:
        getCurrentStream() << "  %" << result_name << " = and i1 " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_OR:
        getCurrentStream() << "  %" << result_name << " = or i1 " << left << ", " << right << "\n";
        break;
    case BinaryExpr::OP_CONCAT:
    case BinaryExpr::OP_CONCAT_WS:
    {
        // Convert left operand to string if needed
        std::string left_str = left;
        if (expr->left->inferredType && expr->left->inferredType->getKind() != TypeInfo::Kind::String)
        {
            std::string left_conv = generateUniqueName("left_conv");
            if (expr->left->inferredType->getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << left_conv << " = call i8* @double_to_string(double " << left << ")\n";
            }
            else if (expr->left->inferredType->getKind() == TypeInfo::Kind::Boolean)
            {
                // Convert double to i1 first, then to string
                std::string bool_conv = generateUniqueName("bool_conv");
                getCurrentStream() << "  %" << bool_conv << " = fcmp one double " << left << ", 0.0\n";
                getCurrentStream() << "  %" << left_conv << " = call i8* @bool_to_string(i1 %" << bool_conv << ")\n";
            }
            else
            {
                // For unknown types, assume it's a number
                getCurrentStream() << "  %" << left_conv << " = call i8* @double_to_string(double " << left << ")\n";
            }
            left_str = "%" + left_conv;
        }

        // Convert right operand to string if needed
        std::string right_str = right;
        if (expr->right->inferredType && expr->right->inferredType->getKind() != TypeInfo::Kind::String)
        {
            std::string right_conv = generateUniqueName("right_conv");
            if (expr->right->inferredType->getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << right_conv << " = call i8* @double_to_string(double " << right << ")\n";
            }
            else if (expr->right->inferredType->getKind() == TypeInfo::Kind::Boolean)
            {
                // Convert double to i1 first, then to string
                std::string bool_conv = generateUniqueName("bool_conv");
                getCurrentStream() << "  %" << bool_conv << " = fcmp one double " << right << ", 0.0\n";
                getCurrentStream() << "  %" << right_conv << " = call i8* @bool_to_string(i1 %" << bool_conv << ")\n";
            }
            else
            {
                // For unknown types, assume it's a number
                getCurrentStream() << "  %" << right_conv << " = call i8* @double_to_string(double " << right << ")\n";
            }
            right_str = "%" + right_conv;
        }

        // Perform string concatenation
        if (expr->op == BinaryExpr::OP_CONCAT)
        {
            getCurrentStream() << "  %" << result_name << " = call i8* @concat_strings(i8* " << left_str << ", i8* " << right_str << ")\n";
        }
        else // OP_CONCAT_WS
        {
            getCurrentStream() << "  %" << result_name << " = call i8* @concat_strings_ws(i8* " << left_str << ", i8* " << right_str << ")\n";
        }
        break;
    }
    default:
        std::cerr << "Unsupported binary operator" << std::endl;
        current_value_ = left;
        return;
    }

    current_value_ = "%" + result_name;
}

void CodeGenerator::visit(CallExpr *expr)
{
    std::cout << "[CodeGen] Processing CallExpr: " << expr->callee << std::endl;

    // Special handling for print function
    if (expr->callee == "print" && expr->args.size() == 1)
    {
        expr->args[0]->accept(this);
        std::string arg_value = current_value_;
        std::string result_name = generateUniqueName("call");

        // Determine the type of the argument and call appropriate print function
        if (expr->args[0]->inferredType)
        {
            switch (expr->args[0]->inferredType->getKind())
            {
            case TypeInfo::Kind::Boolean:
            {
                // For boolean values, we need to convert from double to i1 first
                std::string bool_value = generateUniqueName("bool_conv");
                // Convert the double value to a proper boolean comparison
                getCurrentStream() << "  %" << bool_value << " = fcmp one double " << arg_value << ", 0.0\n";
                getCurrentStream() << "  %" << result_name << " = call double @print_bool(i1 %" << bool_value << ")\n";
                break;
            }
            case TypeInfo::Kind::String:
            {
                // For string values, we need to get the pointer to the string
                getCurrentStream() << "  %" << result_name << " = call double @print_string(i8* " << arg_value << ")\n";
                break;
            }
            case TypeInfo::Kind::Number:
            default:
            {
                // Default: treat as double
                getCurrentStream() << "  %" << result_name << " = call double @print_double(double " << arg_value << ")\n";
                break;
            }
            }
        }
        else
        {
            // If no type information, default to double
            getCurrentStream() << "  %" << result_name << " = call double @print_double(double " << arg_value << ")\n";
        }

        current_value_ = "%" + result_name;
        return;
    }

    // Get function info to determine parameter types
    // We need to check the function signature to see which parameters are Unknown
    std::vector<std::string> args;
    std::vector<std::string> arg_types;

    for (size_t i = 0; i < expr->args.size(); ++i)
    {
        auto &arg = expr->args[i];
        arg->accept(this);
        std::string arg_value = current_value_;

        // Determine argument type based on the inferred type of the argument
        std::string arg_type = "double"; // default
        if (arg->inferredType)
        {
            if (arg->inferredType->getKind() == TypeInfo::Kind::String)
            {
                arg_type = "i8*";
            }
            else if (arg->inferredType->getKind() == TypeInfo::Kind::Boolean)
            {
                // Booleans are always passed as doubles since we use double for everything
                arg_type = "double";
            }
            else if (arg->inferredType->getKind() == TypeInfo::Kind::Number)
            {
                arg_type = "double";
            }
            else if (arg->inferredType->getKind() == TypeInfo::Kind::Unknown)
            {
                // For unknown types, we need to convert to a dynamic type
                // Since we use double for everything, convert to i8* for dynamic type
                if (arg_value.find(".") != std::string::npos) // It's a double
                {
                    // Convert double to i8* for dynamic type
                    std::string conv_name = generateUniqueName("double_to_ptr");
                    getCurrentStream() << "  %" << conv_name << " = bitcast double " << arg_value << " to i8*\n";
                    arg_value = "%" + conv_name;
                }
                else
                {
                    // Assume it's already a pointer or convert as needed
                    std::string conv_name = generateUniqueName("to_ptr");
                    getCurrentStream() << "  %" << conv_name << " = inttoptr i32 " << arg_value << " to i8*\n";
                    arg_value = "%" + conv_name;
                }
                arg_type = "i8*";
            }
        }
        else
        {
            // If no type information, default to double
            arg_type = "double";
        }

        args.push_back(arg_value);
        arg_types.push_back(arg_type);
    }

    std::string result_name = generateUniqueName("call");
    std::string arg_list;
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            arg_list += ", ";
        arg_list += arg_types[i] + " " + args[i];
    }

    // Check the inferred return type of this call expression
    if (expr->inferredType && expr->inferredType->getKind() == TypeInfo::Kind::String)
    {
        // This function returns a string, so we need to call it as i8*
        getCurrentStream() << "  %" << result_name << " = call i8* @" << expr->callee << "(" << arg_list << ")\n";
        current_value_ = "%" + result_name;
    }
    else
    {
        // This function returns a number or other type
        getCurrentStream() << "  %" << result_name << " = call double @" << expr->callee << "(" << arg_list << ")\n";
        current_value_ = "%" + result_name;
    }
}

void CodeGenerator::visit(VariableExpr *expr)
{
    std::cout << "[CodeGen] Processing VariableExpr: " << expr->name << std::endl;

    // Look up variable in current scope
    Scope *scope = current_scope_;
    while (scope)
    {
        auto it = scope->variables.find(expr->name);
        if (it != scope->variables.end())
        {
            std::string load_name = generateUniqueName("load");

            // Determine the type to load based on the inferred type
            std::string load_type = "double"; // default
            if (expr->inferredType)
            {
                load_type = getLLVMType(*expr->inferredType);
            }

            getCurrentStream() << "  %" << load_name << " = load " << load_type << ", " << load_type << "* %" << it->second << "\n";
            current_value_ = "%" + load_name;
            return;
        }
        scope = scope->parent;
    }

    std::cerr << "Variable " << expr->name << " not found" << std::endl;
}

void CodeGenerator::visit(LetExpr *expr)
{
    std::cout << "[CodeGen] Processing LetExpr: " << expr->name << std::endl;

    // Evaluate initializer
    expr->initializer->accept(this);
    std::string init_value = current_value_;

    // Enter new scope
    enterScope();

    // Create alloca for variable
    std::string var_type;
    if (expr->initializer->inferredType &&
        expr->initializer->inferredType->getKind() != TypeInfo::Kind::Void &&
        expr->initializer->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        var_type = getLLVMType(*expr->initializer->inferredType);
    }
    else
    {
        var_type = "double"; // Default to double for void/unknown types
    }

    std::string alloca_name = generateUniqueName(expr->name);

    getCurrentStream() << "  %" << alloca_name << " = alloca " << var_type << "\n";
    getCurrentStream() << "  store " << var_type << " " << init_value << ", " << var_type << "* %" << alloca_name << "\n";
    current_scope_->variables[expr->name] = alloca_name;

    // Evaluate body
    expr->body->accept(this);

    // Exit scope
    exitScope();
}

void CodeGenerator::visit(AssignExpr *expr)
{
    std::cout << "[CodeGen] Processing AssignExpr: " << expr->name << std::endl;

    // Evaluate value
    expr->value->accept(this);
    std::string value = current_value_;

    // Find variable in scope
    Scope *scope = current_scope_;
    while (scope)
    {
        auto it = scope->variables.find(expr->name);
        if (it != scope->variables.end())
        {
            // Determine the type to store based on the value's inferred type
            std::string store_type = "double"; // default
            if (expr->value->inferredType)
            {
                store_type = getLLVMType(*expr->value->inferredType);
            }

            getCurrentStream() << "  store " << store_type << " " << value << ", " << store_type << "* %" << it->second << "\n";
            current_value_ = value;
            return;
        }
        scope = scope->parent;
    }

    std::cerr << "Variable " << expr->name << " not found for assignment" << std::endl;
}

void CodeGenerator::visit(IfExpr *expr)
{
    std::cout << "[CodeGen] Processing IfExpr" << std::endl;

    // Evaluate condition
    expr->condition->accept(this);
    std::string condition = current_value_;

    std::string then_label = generateUniqueName("then");
    std::string else_label = generateUniqueName("else");
    std::string merge_label = generateUniqueName("ifcont");

    // Create conditional branch
    getCurrentStream() << "  br i1 " << condition << ", label %" << then_label << ", label %" << else_label << "\n";

    // Generate then block
    getCurrentStream() << "\n"
                       << then_label << ":\n";
    expr->thenBranch->accept(this);
    std::string then_value = current_value_;
    getCurrentStream() << "  br label %" << merge_label << "\n";

    // Generate else block
    getCurrentStream() << "\n"
                       << else_label << ":\n";
    expr->elseBranch->accept(this);
    std::string else_value = current_value_;
    getCurrentStream() << "  br label %" << merge_label << "\n";

    // Merge block
    getCurrentStream() << "\n"
                       << merge_label << ":\n";

    // Determine the result type based on the inferred type of the if expression
    std::string llvm_type = "double"; // default
    if (expr->inferredType)
    {
        llvm_type = getLLVMType(*expr->inferredType);
    }

    // Create phi node for result
    std::string phi_name = generateUniqueName("iftmp");
    getCurrentStream() << "  %" << phi_name << " = phi " << llvm_type << " [ " << then_value << ", %" << then_label << " ], [ " << else_value << ", %" << else_label << " ]\n";
    current_value_ = "%" + phi_name;
}

void CodeGenerator::visit(ExprBlock *expr)
{
    std::cout << "[CodeGen] Processing ExprBlock" << std::endl;

    enterScope();

    std::string last_value;
    for (auto &stmt : expr->stmts)
    {
        stmt->accept(this);
        last_value = current_value_;
    }

    current_value_ = last_value;
    exitScope();
}

void CodeGenerator::visit(WhileExpr *expr)
{
    std::cout << "[CodeGen] Processing WhileExpr" << std::endl;

    std::string loop_header = generateUniqueName("while_header");
    std::string loop_body = generateUniqueName("while_body");
    std::string loop_exit = generateUniqueName("while_exit");

    // Branch to loop header
    getCurrentStream() << "  br label %" << loop_header << "\n";

    // Loop header: evaluate condition
    getCurrentStream() << "\n"
                       << loop_header << ":\n";
    expr->condition->accept(this);
    std::string condition = current_value_;
    getCurrentStream() << "  br i1 " << condition << ", label %" << loop_body << ", label %" << loop_exit << "\n";

    // Loop body
    getCurrentStream() << "\n"
                       << loop_body << ":\n";
    expr->body->accept(this);
    getCurrentStream() << "  br label %" << loop_header << "\n";

    // Loop exit
    getCurrentStream() << "\n"
                       << loop_exit << ":\n";
    current_value_ = "0"; // void value
}

void CodeGenerator::visit(ForExpr *expr)
{
    std::cout << "[CodeGen] Processing ForExpr for variable: " << expr->variable << std::endl;

    // Generate labels for the loop structure
    std::string loop_header = generateUniqueName("for_header");
    std::string loop_body = generateUniqueName("for_body");
    std::string loop_exit = generateUniqueName("for_exit");

    // Enter new scope for the loop variable
    enterScope();

    // Create alloca for the loop variable
    std::string var_alloca = generateUniqueName(expr->variable);
    getCurrentStream() << "  %" << var_alloca << " = alloca double\n";
    current_scope_->variables[expr->variable] = var_alloca;

    // Check if the iterable is a range() call
    std::string range_start, range_end;
    bool is_range_call = false;

    if (auto callExpr = dynamic_cast<CallExpr *>(expr->iterable.get()))
    {
        if (callExpr->callee == "range" && callExpr->args.size() == 2)
        {
            std::cout << "[CodeGen] Detected range() call, extracting start and end values" << std::endl;
            is_range_call = true;

            // Evaluate start argument
            callExpr->args[0]->accept(this);
            range_start = current_value_;

            // Evaluate end argument
            callExpr->args[1]->accept(this);
            range_end = current_value_;
        }
    }

    if (!is_range_call)
    {
        // Fallback to the old behavior for non-range iterables
        std::cout << "[CodeGen] Non-range iterable, using default behavior" << std::endl;
        expr->iterable->accept(this);
        range_start = "0.0";
        range_end = current_value_;
    }

    // Convert start and end to integers for comparison
    std::string start_int = generateUniqueName("start_int");
    std::string end_int = generateUniqueName("end_int");
    getCurrentStream() << "  %" << start_int << " = fptosi double " << range_start << " to i32\n";
    getCurrentStream() << "  %" << end_int << " = fptosi double " << range_end << " to i32\n";

    // Initialize counter with start value
    std::string counter = generateUniqueName("counter");
    getCurrentStream() << "  %" << counter << " = alloca i32\n";
    getCurrentStream() << "  store i32 %" << start_int << ", i32* %" << counter << "\n";

    // Branch to loop header
    getCurrentStream() << "  br label %" << loop_header << "\n";

    // Loop header: check if we should continue
    getCurrentStream() << "\n"
                       << loop_header << ":\n";
    std::string counter_load = generateUniqueName("counter_load");
    getCurrentStream() << "  %" << counter_load << " = load i32, i32* %" << counter << "\n";

    // Compare counter with range end (loop continues while counter < end)
    std::string continue_cond = generateUniqueName("continue_cond");
    getCurrentStream() << "  %" << continue_cond << " = icmp slt i32 %" << counter_load << ", %" << end_int << "\n";
    getCurrentStream() << "  br i1 %" << continue_cond << ", label %" << loop_body << ", label %" << loop_exit << "\n";

    // Loop body
    getCurrentStream() << "\n"
                       << loop_body << ":\n";

    // Set the loop variable to the current counter value (as double)
    std::string counter_double = generateUniqueName("counter_double");
    getCurrentStream() << "  %" << counter_double << " = sitofp i32 %" << counter_load << " to double\n";
    getCurrentStream() << "  store double %" << counter_double << ", double* %" << var_alloca << "\n";

    // Execute the loop body
    expr->body->accept(this);

    // Increment counter
    std::string counter_inc = generateUniqueName("counter_inc");
    getCurrentStream() << "  %" << counter_inc << " = add i32 %" << counter_load << ", 1\n";
    getCurrentStream() << "  store i32 %" << counter_inc << ", i32* %" << counter << "\n";

    // Branch back to header
    getCurrentStream() << "  br label %" << loop_header << "\n";

    // Loop exit
    getCurrentStream() << "\n"
                       << loop_exit << ":\n";

    // Exit scope
    exitScope();

    // Set return value to 0 (void equivalent)
    current_value_ = "0.0";
}

void CodeGenerator::visit(NewExpr *expr)
{
    std::cout << "[CodeGen] Processing NewExpr: " << expr->typeName << std::endl;
    // Object creation would go here
}

void CodeGenerator::visit(GetAttrExpr *expr)
{
    std::cout << "[CodeGen] Processing GetAttrExpr: " << expr->attrName << std::endl;
    // Attribute access would go here
}

void CodeGenerator::visit(SetAttrExpr *expr)
{
    std::cout << "[CodeGen] Processing SetAttrExpr: " << expr->attrName << std::endl;
    // Attribute assignment would go here
}

void CodeGenerator::visit(MethodCallExpr *expr)
{
    std::cout << "[CodeGen] Processing MethodCallExpr: " << expr->methodName << std::endl;
    // Method call would go here
}

void CodeGenerator::visit(SelfExpr *expr)
{
    std::cout << "[CodeGen] Processing SelfExpr" << std::endl;
    // Self reference would go here
}

void CodeGenerator::visit(BaseCallExpr *expr)
{
    std::cout << "[CodeGen] Processing BaseCallExpr" << std::endl;
    // Base class call would go here
}

std::string CodeGenerator::registerStringConstant(const std::string &value)
{
    // Check if this string constant has already been generated
    if (generated_strings_.find(value) != generated_strings_.end())
    {
        // Return the existing constant name
        return "@.str." + std::to_string(std::hash<std::string>{}(value));
    }

    // Generate new constant
    std::string str_name = "@.str." + std::to_string(std::hash<std::string>{}(value));
    global_constants_ << str_name << " = private unnamed_addr constant [" << (value.length() + 1) << " x i8] c\"" << value << "\\00\"\n";
    generated_strings_.insert(value);
    return str_name;
}

std::stringstream &CodeGenerator::getCurrentStream()
{
    if (current_function_ == "main" || current_function_.empty())
    {
        return ir_code_;
    }
    else
    {
        return function_definitions_;
    }
}