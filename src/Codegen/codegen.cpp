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
        // Check if we have a registered type for this object
        if (!type.getTypeName().empty() && types_.find(type.getTypeName()) != types_.end())
        {
            return types_[type.getTypeName()] + "*"; // Return pointer to the struct type
        }
        return "i8*"; // Fallback to opaque pointer for unknown object types
    case TypeInfo::Kind::Unknown:
        return "%struct.BoxedValue*"; // Use boxed value for unknown types
    default:
        return "i8*"; // Default to generic pointer for safety
    }
}

std::string CodeGenerator::getAttributeLLVMType(const std::string &typeName, const std::string &attrName)
{
    // Look up the attribute type in our mapping
    auto type_it = attribute_types_.find(typeName);
    if (type_it != attribute_types_.end())
    {
        auto attr_it = type_it->second.find(attrName);
        if (attr_it != type_it->second.end())
        {
            return attr_it->second;
        }
    }

    // Fallback to generic type if not found
    std::cout << "[CodeGen] Warning: Attribute type not found for " << typeName << "." << attrName << ", using i8*" << std::endl;
    return "i8*";
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
    std::cout << "Tipo de retorno: " << return_type << std::endl;

    // Get parameter types - use %struct.BoxedValue* for Unknown types, preserve known types
    std::string param_types;
    for (size_t i = 0; i < stmt->paramTypes.size(); ++i)
    {
        if (i > 0)
            param_types += ", ";

        std::string param_type;
        if (stmt->paramTypes[i]->getKind() == TypeInfo::Kind::Unknown)
        {
            // For unknown types, use %struct.BoxedValue* as a dynamic type that can hold any value
            param_type = "%struct.BoxedValue*";
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

    // Register function signature for future calls
    std::vector<TypeInfo::Kind> signature;
    for (const auto &paramType : stmt->paramTypes)
    {
        signature.push_back(paramType->getKind());
    }
    function_signatures_[stmt->name] = signature;

    current_function_ = stmt->name;

    // Set up parameters
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::string param_name = stmt->params[i];
        std::string param_type;
        if (stmt->paramTypes[i]->getKind() == TypeInfo::Kind::Unknown)
        {
            param_type = "%struct.BoxedValue*";
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

    // Set current type for method generation
    current_type_ = stmt->name;

    // Create struct type name
    std::string struct_name = "%struct." + stmt->name;

    // Write type declaration to global scope (before main function)
    global_constants_ << struct_name << " = type { ";

    // Process attributes and determine their LLVM types using our hybrid strategy
    std::vector<std::string> attribute_types;
    std::unordered_map<std::string, std::string> type_attr_map;
    std::vector<std::string> param_to_attr_mapping;
    std::unordered_map<std::string, int> attr_index_map;

    // Handle inheritance: add base type pointer as first field
    if (stmt->baseType != "Object")
    {
        std::string base_struct_name = "%struct." + stmt->baseType + "*";
        attribute_types.push_back(base_struct_name);
        type_attr_map["__base"] = base_struct_name;
        attr_index_map["__base"] = 0; // Base pointer is always at index 0
        
        std::cout << "[CodeGen] Type " << stmt->name << " inherits from " << stmt->baseType 
                  << ", adding base pointer as first field" << std::endl;
        
        // Store inheritance information
        inheritance_chain_[stmt->name] = stmt->baseType;
        std::cout << "[CodeGen] Registered inheritance: " << stmt->name << " -> " << stmt->baseType << std::endl;
    }

    // Add own attributes (starting from index 1 if there's inheritance, 0 otherwise)
    int attr_offset = (stmt->baseType != "Object") ? 1 : 0;
    
    for (size_t i = 0; i < stmt->attributes.size(); ++i)
    {
        std::string attr_name = stmt->attributes[i]->name;
        std::string param_name = (i < stmt->params.size()) ? stmt->params[i] : "";

        // All attributes are defined as BoxedValue in the structure
        std::string attr_type = "%struct.BoxedValue*";

        attribute_types.push_back(attr_type);
        type_attr_map[attr_name] = attr_type;
        param_to_attr_mapping.push_back(attr_name);
        attr_index_map[attr_name] = i + attr_offset; // Store the index for direct access

        if (!param_name.empty())
        {
            std::cout << "[CodeGen] Parameter " << param_name << " maps to attribute " << attr_name << " at position " << (i + attr_offset) << std::endl;
        }
    }

    // Build the struct definition
    for (size_t i = 0; i < attribute_types.size(); ++i)
    {
        if (i > 0)
            global_constants_ << ", ";
        global_constants_ << attribute_types[i];
    }

    // If no attributes and no inheritance, add a dummy field to avoid empty struct
    if (stmt->attributes.empty() && stmt->baseType == "Object")
    {
        global_constants_ << "i8"; // Empty struct needs at least one field
    }

    global_constants_ << " }\n";

    // Register the type, its attribute types, and parameter mapping for future use
    types_[stmt->name] = struct_name;
    attribute_types_[stmt->name] = type_attr_map;
    parameter_to_attribute_mappings_[stmt->name] = param_to_attr_mapping;
    attribute_indices_[stmt->name] = attr_index_map;
    type_declarations_[stmt->name] = stmt; // Store the type declaration for access to initializers

    std::cout << "[CodeGen] Created struct type: " << struct_name << " for type " << stmt->name << std::endl;
    std::cout << "[CodeGen] Registered " << type_attr_map.size() << " attributes for type " << stmt->name << std::endl;
    std::cout << "[CodeGen] Parameter mapping: " << param_to_attr_mapping.size() << " parameters mapped to attributes" << std::endl;

    generateConstructorFunction(stmt);

    // Process methods after the type is declared
    for (const auto &method : stmt->methods)
    {
        method->accept(this);
    }

    // Generate inherited method wrappers for methods not overridden
    if (stmt->baseType != "Object")
    {
        generateInheritedMethodWrappers(stmt);
    }

    // Clear current type
    current_type_ = "";
}

void CodeGenerator::visit(MethodDecl *stmt)
{
    std::cout << "[CodeGen] Processing MethodDecl: " << stmt->name << std::endl;

    // Determinar el tipo de retorno
    std::string return_type = "%struct.BoxedValue*";
    bool is_void = false;
    if (stmt->inferredType && stmt->inferredType->getKind() == TypeInfo::Kind::Void)
    {
        return_type = "void";
        is_void = true;
    }
    else if (stmt->inferredType && stmt->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        return_type = getLLVMType(*stmt->inferredType);
        std::cout << "[CodeGen] Method " << stmt->name << " has known return type: "
                  << stmt->inferredType->toString() << " -> LLVM type: " << return_type << std::endl;
    }
    else
    {
        return_type = "%struct.BoxedValue*";
        std::cout << "[CodeGen] Method " << stmt->name << " has unknown/dynamic return type -> using BoxedValue" << std::endl;
    }

    // Build parameter list starting with self
    std::string type_name = current_type_;
    std::string param_list = "%struct." + type_name + "* %self";

    // Add method parameters - MethodDecl doesn't have paramTypes, so all parameters son BoxedValue
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::string param_type = "%struct.BoxedValue*";
        param_list += ", " + param_type + " %" + stmt->params[i];
    }

    // Generate function definition
    std::string function_name = type_name + "_" + stmt->name;
    function_definitions_ << "define " << return_type << " @" << function_name << "(" << param_list << ") {\n";
    function_definitions_ << "entry:\n";

    // Enter a new scope for the method
    enterScope();

    // Register parameters in the current scope - all as BoxedValue
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::string param_type = "%struct.BoxedValue*";
        std::string param_var = generateUniqueName("param");
        function_definitions_ << "  %" << param_var << " = alloca " << param_type << "\n";
        function_definitions_ << "  store " << param_type << " %" << stmt->params[i] << ", " << param_type << "* %" << param_var << "\n";
        current_scope_->variables[stmt->params[i]] = param_var;
    }

    // Process the method body
    if (stmt->body)
    {
        std::stringstream old_stream;
        old_stream << ir_code_.str();
        ir_code_.str("");
        ir_code_.clear();

        stmt->body->accept(this);
        std::string body_code = ir_code_.str();

        ir_code_.str("");
        ir_code_.clear();
        ir_code_ << old_stream.str();

        function_definitions_ << body_code;

        if (is_void)
        {
            function_definitions_ << "  ret void\n";
        }
        else if (!current_value_.empty())
        {
            function_definitions_ << "  ret " << return_type << " " << current_value_ << "\n";
        }
        else
        {
            // Default return value según tipo
            if (return_type == "i8*")
                function_definitions_ << "  ret i8* null\n";
            else if (return_type == "i1")
                function_definitions_ << "  ret i1 false\n";
            else if (return_type == "%struct.BoxedValue*")
                function_definitions_ << "  %null_box = call %struct.BoxedValue* @boxNull()\n  ret %struct.BoxedValue* %null_box\n";
            else
                function_definitions_ << "  ret double 0.0\n";
        }
    }
    else
    {
        if (is_void)
        {
            function_definitions_ << "  ret void\n";
        }
        else if (return_type == "i8*")
            function_definitions_ << "  ret i8* null\n";
        else if (return_type == "i1")
            function_definitions_ << "  ret i1 false\n";
        else if (return_type == "%struct.BoxedValue*")
            function_definitions_ << "  %null_box = call %struct.BoxedValue* @boxNull()\n  ret %struct.BoxedValue* %null_box\n";
        else
            function_definitions_ << "  ret double 0.0\n";
    }

    function_definitions_ << "}\n\n";

    exitScope();
    functions_[function_name] = return_type;

    // Register method in inheritance system
    type_methods_[current_type_].insert(stmt->name);
    std::string method_signature = getMethodSignature(stmt);
    method_signatures_[current_type_ + "." + stmt->name] = method_signature;
    
    std::cout << "[CodeGen] Registered method " << stmt->name << " for type " << current_type_ 
              << " with signature: " << method_signature << std::endl;
}

void CodeGenerator::visit(AttributeDecl *stmt)
{
    std::cout << "[CodeGen] Processing AttributeDecl: " << stmt->name << std::endl;

    // This method is called during attribute initialization in constructors
    // We need to evaluate the initializer expression and store it in the appropriate field

    // Get the current object pointer and attribute index from context
    // These should be set by the calling context (generateConstructorFunction)
    std::string object_ptr = current_object_ptr_;   // Set by calling context
    std::string struct_type = current_struct_type_; // Set by calling context
    int attr_index = current_attr_index_;           // Set by calling context

    // Get the attribute type from our mapping
    std::string store_type = "%struct.BoxedValue*"; // default
    auto attr_types_it = attribute_types_.find(current_type_);
    if (attr_types_it != attribute_types_.end())
    {
        auto attr_it = attr_types_it->second.find(stmt->name);
        if (attr_it != attr_types_it->second.end())
        {
            store_type = attr_it->second;
        }
    }

    // Get the field pointer
    std::string field_ptr = generateUniqueName("field_ptr");
    getCurrentStream() << "  %" << field_ptr << " = getelementptr " << struct_type << ", " << struct_type << "* " << object_ptr << ", i32 0, i32 " << attr_index << "\n";

    // Check if this is a direct parameter assignment (e.g., x = x)
    bool is_direct_param_assignment = false;
    std::string param_name = "";

    if (auto var_expr = dynamic_cast<VariableExpr *>(stmt->initializer.get()))
    {
        // Check if the variable name matches the attribute name
        if (var_expr->name == stmt->name)
        {
            // This is a direct parameter assignment like "x = x"
            is_direct_param_assignment = true;
            param_name = var_expr->name;
            std::cout << "[CodeGen] Direct parameter assignment detected: " << stmt->name << " = " << param_name << std::endl;
        }
    }

    if (is_direct_param_assignment)
    {
        // Direct parameter assignment - just store the parameter directly
        std::string param_value = "%" + param_name; // Parameter is already in scope as %param_name
        getCurrentStream() << "  store " << store_type << " " << param_value << ", " << store_type << "* %" << field_ptr << "\n";
        std::cout << "[CodeGen] Direct store: " << param_value << " -> field " << stmt->name << std::endl;
    }
    else
    {
        // Regular expression evaluation
        // Step 1: Evaluate the initializer expression (e.g., 'a+4' in 'x = a+4')
        stmt->initializer->accept(this);
        std::string init_value = current_value_; // This is the resulting value

        // Step 2: Determine the LLVM type of the value we just got from the initializer
        std::string init_type;
        if (stmt->initializer->inferredType && stmt->initializer->inferredType->getKind() != TypeInfo::Kind::Unknown)
        {
            init_type = getLLVMType(*stmt->initializer->inferredType);
        }
        else
        {
            init_type = "%struct.BoxedValue*";
        }

        std::cout << "[CodeGen] Initializing attribute '" << stmt->name << "'. Field type: " << store_type << ", Initializer type: " << init_type << std::endl;

        // Step 3: Check if the initializer's type matches the field's type. If not, convert.
        if (store_type != init_type)
        {
            // Case A: The field expects a BoxedValue, but the initializer provided a specific type
            if (store_type == "%struct.BoxedValue*")
            {
                std::cout << "[CodeGen] BOXING required for attribute '" << stmt->name << "'. Converting " << init_type << " to BoxedValue." << std::endl;
                init_value = generateBoxedValue(init_value, *stmt->initializer->inferredType);
            }
            // Case B: The field expects a specific type, but the initializer provided a BoxedValue
            else if (init_type == "%struct.BoxedValue*")
            {
                std::cout << "[CodeGen] UNBOXING required for attribute '" << stmt->name << "'. Converting BoxedValue to " << store_type << "." << std::endl;
                TypeInfo::Kind target_kind = TypeInfo::Kind::Number; // default
                if (store_type == "i1")
                    target_kind = TypeInfo::Kind::Boolean;
                else if (store_type == "i8*")
                    target_kind = TypeInfo::Kind::String;
                else if (store_type.find("%struct.") == 0)
                    target_kind = TypeInfo::Kind::Object;
                init_value = generateUnboxedValue(init_value, TypeInfo(target_kind));
            }
            // Case C: Mismatch between two different specific types
            else
            {
                std::cout << "[CodeGen] CONVERTING between specific types for attribute '" << stmt->name << "' (" << init_type << " -> " << store_type << ")." << std::endl;
                std::string temp_boxed = generateBoxedValue(init_value, *stmt->initializer->inferredType);
                TypeInfo::Kind target_kind = TypeInfo::Kind::Number; // default
                if (store_type == "i1")
                    target_kind = TypeInfo::Kind::Boolean;
                else if (store_type == "i8*")
                    target_kind = TypeInfo::Kind::String;
                init_value = generateUnboxedValue(temp_boxed, TypeInfo(target_kind));
            }
        }
        else
        {
            std::cout << "[CodeGen] No conversion needed for attribute '" << stmt->name << "'." << std::endl;
        }

        // Step 4: Store the (potentially converted) value into the struct field
        getCurrentStream() << "  store " << store_type << " " << init_value << ", " << store_type << "* %" << field_ptr << "\n";
    }
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

    // Generate boolean values as i1 (true or false) for proper boolean operations
    std::string bool_name = generateUniqueName("bool");
    getCurrentStream() << "  %" << bool_name << " = add i1 0, " << (expr->value ? "1" : "0") << "\n";
    current_value_ = "%" + bool_name;
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

    // Get the inferred types of the operands
    TypeInfo left_type = expr->left->inferredType ? *expr->left->inferredType : TypeInfo(TypeInfo::Kind::Unknown);
    TypeInfo right_type = expr->right->inferredType ? *expr->right->inferredType : TypeInfo(TypeInfo::Kind::Unknown);

    std::string operation;
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
        operation = "add";
        break;
    case BinaryExpr::OP_SUB:
        operation = "sub";
        break;
    case BinaryExpr::OP_MUL:
        operation = "mul";
        break;
    case BinaryExpr::OP_DIV:
        operation = "div";
        break;
    case BinaryExpr::OP_LT:
        operation = "lt";
        break;
    case BinaryExpr::OP_GT:
        operation = "gt";
        break;
    case BinaryExpr::OP_LE:
        operation = "le";
        break;
    case BinaryExpr::OP_GE:
        operation = "ge";
        break;
    case BinaryExpr::OP_EQ:
        operation = "eq";
        break;
    case BinaryExpr::OP_NEQ:
        operation = "neq";
        break;
    case BinaryExpr::OP_AND:
        operation = "and";
        break;
    case BinaryExpr::OP_OR:
        operation = "or";
        break;
    case BinaryExpr::OP_CONCAT:
        operation = "concat";
        break;
    case BinaryExpr::OP_CONCAT_WS:
        operation = "concat_ws";
        break;
    case BinaryExpr::OP_MOD:
        operation = "mod";
        break;
    case BinaryExpr::OP_POW:
        operation = "pow";
        break;
    default:
        std::cerr << "Unsupported binary operator" << std::endl;
        current_value_ = left;
        return;
    }

    // Use the boxed value operation system
    current_value_ = generateBoxedValueOperation(left, right, operation, left_type, right_type);
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
                // For boolean values, we need to handle i1 values correctly
                if (arg_value.find("%") == 0)
                {
                    // It's already a register (i1), use it directly
                    getCurrentStream() << "  %" << result_name << " = call double @print_bool(i1 " << arg_value << ")\n";
                }
                else
                {
                    // It's a literal, convert to i1 first
                    std::string bool_value = generateUniqueName("bool_conv");
                    if (arg_value == "true" || arg_value == "1" || arg_value == "1.0")
                    {
                        getCurrentStream() << "  %" << bool_value << " = add i1 0, 1\n";
                    }
                    else
                    {
                        getCurrentStream() << "  %" << bool_value << " = add i1 0, 0\n";
                    }
                    getCurrentStream() << "  %" << result_name << " = call double @print_bool(i1 %" << bool_value << ")\n";
                }
                break;
            }
            case TypeInfo::Kind::String:
            {
                // For string values, we need to get the pointer to the string
                getCurrentStream() << "  %" << result_name << " = call double @print_string(i8* " << arg_value << ")\n";
                break;
            }
            case TypeInfo::Kind::Number:
            {
                // Default: treat as double
                getCurrentStream() << "  %" << result_name << " = call double @print_double(double " << arg_value << ")\n";
                break;
            }
            default:
            {
                getCurrentStream() << "  %" << result_name << " = call double @print_boxed(%struct.BoxedValue* " << arg_value << ")\n";
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

    // Get the expected parameter types for this function
    std::vector<TypeInfo::Kind> expected_param_types;
    auto signature_it = function_signatures_.find(expr->callee);
    if (signature_it != function_signatures_.end())
    {
        expected_param_types = signature_it->second;
    }
    else
    {
        // Function signature not found - assume all parameters are double
        expected_param_types.resize(expr->args.size(), TypeInfo::Kind::Number);
    }

    // Ensure we have the right number of expected types
    if (expected_param_types.size() != expr->args.size())
    {
        std::cerr << "[CodeGen] Warning: Function " << expr->callee << " expects "
                  << expected_param_types.size() << " parameters but got " << expr->args.size() << std::endl;
        // Pad with Number types if needed
        while (expected_param_types.size() < expr->args.size())
        {
            expected_param_types.push_back(TypeInfo::Kind::Number);
        }
    }

    for (size_t i = 0; i < expr->args.size(); ++i)
    {
        auto &arg = expr->args[i];
        arg->accept(this);
        std::string arg_value = current_value_;

        // Get the actual type of the argument
        TypeInfo::Kind arg_type_kind = TypeInfo::Kind::Number; // default
        if (arg->inferredType)
        {
            arg_type_kind = arg->inferredType->getKind();
        }

        // Get the expected type for this parameter
        TypeInfo::Kind expected_type = expected_param_types[i];

        // Determine if we need to convert the argument
        std::string final_arg_value = arg_value;
        std::string final_arg_type;

        if (expected_type == TypeInfo::Kind::Unknown)
        {
            // Function expects BoxedValue - convert if needed
            if (arg_type_kind != TypeInfo::Kind::Unknown)
            {
                // Convert specific type to BoxedValue
                final_arg_value = generateBoxedValue(arg_value, *arg->inferredType);
                final_arg_type = "%struct.BoxedValue*";
            }
            else
            {
                // Already BoxedValue
                final_arg_type = "%struct.BoxedValue*";
            }
        }
        else
        {
            // Function expects specific type
            if (arg_type_kind == TypeInfo::Kind::Unknown)
            {
                // Convert BoxedValue to expected type
                final_arg_value = generateUnboxedValue(arg_value, TypeInfo(expected_type));
                final_arg_type = getLLVMType(TypeInfo(expected_type));
            }
            else if (arg_type_kind != expected_type)
            {
                // Type mismatch - convert if possible
                if (expected_type == TypeInfo::Kind::Number && arg_type_kind == TypeInfo::Kind::Boolean)
                {
                    // Convert boolean to number
                    std::string conv_name = generateUniqueName("bool_to_num");
                    getCurrentStream() << "  %" << conv_name << " = uitofp i1 " << arg_value << " to double\n";
                    final_arg_value = "%" + conv_name;
                    final_arg_type = "double";
                }
                else if (expected_type == TypeInfo::Kind::Boolean && arg_type_kind == TypeInfo::Kind::Number)
                {
                    // Convert number to boolean
                    std::string conv_name = generateUniqueName("num_to_bool");
                    getCurrentStream() << "  %" << conv_name << " = fcmp one double " << arg_value << ", 0.0\n";
                    final_arg_value = "%" + conv_name;
                    final_arg_type = "i1";
                }
                else
                {
                    // Incompatible types - use BoxedValue as fallback
                    final_arg_value = generateBoxedValue(arg_value, *arg->inferredType);
                    final_arg_type = "%struct.BoxedValue*";
                }
            }
            else
            {
                // Types match
                final_arg_type = getLLVMType(*arg->inferredType);
            }
        }

        args.push_back(final_arg_value);
        arg_types.push_back(final_arg_type);
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
    std::string return_type = "double"; // default
    if (expr->inferredType)
    {
        return_type = getLLVMType(*expr->inferredType);
    }

    // Generate the function call with the correct return type
    getCurrentStream() << "  %" << result_name << " = call " << return_type << " @" << expr->callee << "(" << arg_list << ")\n";
    current_value_ = "%" + result_name;
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

            // Check if this variable is stored as a boxed value
            auto boxed_it = scope->boxed_variables.find(expr->name);
            if (boxed_it != scope->boxed_variables.end())
            {
                // Variable is stored as a boxed value
                getCurrentStream() << "  %" << load_name << " = load %struct.BoxedValue*, %struct.BoxedValue** %" << boxed_it->second << "\n";
                current_value_ = "%" + load_name;
                return;
            }

            // Variable is stored as a regular value
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

    // Get the type of the initializer
    TypeInfo init_type = expr->initializer->inferredType ? *expr->initializer->inferredType : TypeInfo(TypeInfo::Kind::Unknown);

    // Check if we need to use boxed values
    if (init_type.getKind() == TypeInfo::Kind::Unknown)
    {
        // Use boxed value storage for unknown types
        std::string boxed_alloca = generateBoxedValueAlloca(expr->name);
        generateBoxedValueStore(boxed_alloca, init_value, init_type);
        current_scope_->boxed_variables[expr->name] = boxed_alloca;
        current_scope_->variable_types[expr->name] = init_type;
    }
    else
    {
        // Use regular storage for known types
        std::string var_type = getLLVMType(init_type);
        std::string alloca_name = generateUniqueName(expr->name);

        getCurrentStream() << "  %" << alloca_name << " = alloca " << var_type << "\n";
        getCurrentStream() << "  store " << var_type << " " << init_value << ", " << var_type << "* %" << alloca_name << "\n";
        current_scope_->variables[expr->name] = alloca_name;
        current_scope_->variable_types[expr->name] = init_type;
    }

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
        // Check if this variable is stored as a boxed value
        auto boxed_it = scope->boxed_variables.find(expr->name);
        if (boxed_it != scope->boxed_variables.end())
        {
            // Variable is stored as a boxed value
            TypeInfo value_type = expr->value->inferredType ? *expr->value->inferredType : TypeInfo(TypeInfo::Kind::Unknown);
            generateBoxedValueStore(boxed_it->second, value, value_type);
            current_value_ = value;
            return;
        }

        auto it = scope->variables.find(expr->name);
        if (it != scope->variables.end())
        {
            // Variable is stored as a regular value
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

    // Look up the type to get its struct name
    auto type_it = types_.find(expr->typeName);
    if (type_it == types_.end())
    {
        std::cerr << "[CodeGen] Error: Type " << expr->typeName << " not found in types map" << std::endl;
        current_value_ = "null";
        return;
    }

    std::string struct_type = type_it->second;
    std::string result_name = generateUniqueName("new_obj");

    // Check if we have a constructor function for this type
    std::string constructor_name = "construct_" + expr->typeName;
    auto constructor_it = functions_.find(constructor_name);

    if (constructor_it != functions_.end())
    {
        // Use the constructor function
        std::cout << "[CodeGen] Using constructor function: " << constructor_name << std::endl;

        // Prepare arguments for constructor
        std::vector<std::string> args;
        std::string arg_list;

        for (size_t i = 0; i < expr->args.size(); ++i)
        {
            expr->args[i]->accept(this);
            std::string arg_value = current_value_;

            // Convert argument to BoxedValue for constructor
            if (expr->args[i]->inferredType &&
                expr->args[i]->inferredType->getKind() != TypeInfo::Kind::Unknown)
            {
                arg_value = generateBoxedValue(arg_value, *expr->args[i]->inferredType);
            }

            if (i > 0)
                arg_list += ", ";
            arg_list += "%struct.BoxedValue* " + arg_value;
        }

        // Call the constructor function
        getCurrentStream() << "  %" << result_name << " = call " << struct_type << "* @" << constructor_name << "(" << arg_list << ")\n";
    }
    else
    {
        // Always use malloc for object creation
        std::cout << "[CodeGen] Using malloc for object creation" << std::endl;

        // Use malloc for object allocation
        int struct_size = calculateStructSize(expr->typeName);
        std::string malloc_size = std::to_string(struct_size);
        getCurrentStream() << "  %" << result_name << "_raw = call i8* @malloc(i32 " << malloc_size << ")\n";
        getCurrentStream() << "  %" << result_name << " = bitcast i8* %" << result_name << "_raw to " << struct_type << "\n";

        // Get the type declaration to access attribute initializers
        auto attr_types_it = attribute_types_.find(expr->typeName);

        if (type_declarations_.find(expr->typeName) != type_declarations_.end() &&
            attr_types_it != attribute_types_.end())
        {
            TypeDecl *type_decl = type_declarations_[expr->typeName];
            const auto &attr_types = attr_types_it->second;

            std::cout << "[CodeGen] Initializing object with " << expr->args.size() << " constructor arguments" << std::endl;

            // Validate number of arguments
            if (expr->args.size() != type_decl->params.size())
            {
                std::cerr << "[CodeGen] Warning: Type " << expr->typeName << " expects "
                          << type_decl->params.size() << " parameters but got " << expr->args.size() << std::endl;
            }

            // Enter a new scope for constructor parameters
            enterScope();

            // Process constructor arguments and store them as local variables
            // These will be available as parameters in the attribute initializers
            for (size_t i = 0; i < expr->args.size(); ++i)
            {
                expr->args[i]->accept(this);
                std::string arg_value = current_value_;

                // Store parameter as local variable
                std::string param_var = generateUniqueName("param");

                // Get the parameter type from the type system
                std::string param_type;
                if (expr->args[i]->inferredType &&
                    expr->args[i]->inferredType->getKind() != TypeInfo::Kind::Unknown)
                {
                    // Parameter has a known type
                    param_type = getLLVMType(*expr->args[i]->inferredType);
                    std::cout << "[CodeGen] Parameter " << i << " has known type: "
                              << expr->args[i]->inferredType->toString() << " -> " << param_type << std::endl;
                }
                else
                {
                    // Parameter has unknown type - use BoxedValue
                    param_type = "%struct.BoxedValue*";
                    std::cout << "[CodeGen] Parameter " << i << " has unknown type -> using BoxedValue" << std::endl;

                    // Convert the argument to BoxedValue if it's not already
                    if (expr->args[i]->inferredType &&
                        expr->args[i]->inferredType->getKind() != TypeInfo::Kind::Unknown)
                    {
                        arg_value = generateBoxedValue(arg_value, *expr->args[i]->inferredType);
                    }
                }

                getCurrentStream() << "  %" << param_var << " = call i8* @malloc(i32 8)\n";
                getCurrentStream() << "  %" << param_var << "_cast = bitcast i8* %" << param_var << " to " << param_type << "*\n";
                getCurrentStream() << "  store " << param_type << " " << arg_value << ", " << param_type << "* %" << param_var << "_cast\n";

                // Register parameter in current scope
                if (i < type_decl->params.size())
                {
                    current_scope_->variables[type_decl->params[i]] = param_var;
                    std::cout << "[CodeGen] Registered parameter " << type_decl->params[i] << " as " << param_var << " (type: " << param_type << ")" << std::endl;
                }
            }

            // Initialize attributes by evaluating their initializers
            // Each attribute's initializer is evaluated in the context where parameters are available
            for (size_t attr_index = 0; attr_index < type_decl->attributes.size(); ++attr_index)
            {
                const auto &attr = type_decl->attributes[attr_index];
                std::string attr_name = attr->name;
                std::string store_type = attr_types.at(attr_name); // The LLVM type of the struct field

                // Get the field pointer
                std::string field_ptr = generateUniqueName("field_ptr");
                function_definitions_ << "  %" << field_ptr << " = getelementptr " << struct_type << ", " << struct_type << "* %" << result_name << ", i32 0, i32 " << attr_index << "\n";

                // Step 1: Evaluate the initializer expression (e.g., 'a+4' in 'x = a+4')
                attr->initializer->accept(this);
                std::string init_value = current_value_; // This is the resulting value (e.g., a double like 7.0)

                // Step 2: Determine the LLVM type of the value we just got from the initializer
                std::string init_type;
                if (attr->initializer->inferredType && attr->initializer->inferredType->getKind() != TypeInfo::Kind::Unknown)
                {
                    init_type = getLLVMType(*attr->initializer->inferredType);
                    std::cout << "[CodeGen] INIT TYPE: " << init_type << std::endl; //[DEBUG]
                }
                else
                {
                    init_type = "%struct.BoxedValue*";
                    std::cout << "[CodeGen] INIT TYPE: " << init_type << std::endl; //[DEBUG]
                }

                std::cout << "[CodeGen] Initializing attribute '" << attr_name << "'. Field type: " << store_type << ", Initializer type: " << init_type << std::endl;

                // Step 3: Check if the initializer's type matches the field's type. If not, convert.
                // This is the key logic to handle boxing and unboxing.
                if (store_type != init_type)
                {
                    // Case A: The field expects a BoxedValue, but the initializer provided a specific type (e.g., double, i1, i8*).
                    // This is the exact case for `new Point(3,2)`. The field is BoxedValue, the initializer is double.
                    // We MUST box the specific type.
                    if (store_type == "%struct.BoxedValue*")
                    {
                        std::cout << "[CodeGen] BOXING required for attribute '" << attr_name << "'. Converting " << init_type << " to BoxedValue." << std::endl;
                        init_value = generateBoxedValue(init_value, *attr->initializer->inferredType);
                    }
                    // Case B: The field expects a specific type, but the initializer provided a BoxedValue.
                    // We MUST unbox the value.
                    else if (init_type == "%struct.BoxedValue*")
                    {
                        std::cout << "[CodeGen] UNBOXING required for attribute '" << attr_name << "'. Converting BoxedValue to " << store_type << "." << std::endl;
                        TypeInfo::Kind target_kind = TypeInfo::Kind::Number; // default
                        if (store_type == "i1")
                            target_kind = TypeInfo::Kind::Boolean;
                        else if (store_type == "i8*")
                            target_kind = TypeInfo::Kind::String;
                        else if (store_type.find("%struct.") == 0)
                            target_kind = TypeInfo::Kind::Object;
                        init_value = generateUnboxedValue(init_value, TypeInfo(target_kind));
                    }
                    // Case C: Mismatch between two different specific types. This is rare but we can handle it by boxing then unboxing.
                    else
                    {
                        std::cout << "[CodeGen] CONVERTING between specific types for attribute '" << attr_name << "' (" << init_type << " -> " << store_type << ")." << std::endl;
                        std::string temp_boxed = generateBoxedValue(init_value, *attr->initializer->inferredType);
                        TypeInfo::Kind target_kind = TypeInfo::Kind::Number; // default
                        if (store_type == "i1")
                            target_kind = TypeInfo::Kind::Boolean;
                        else if (store_type == "i8*")
                            target_kind = TypeInfo::Kind::String;
                        init_value = generateUnboxedValue(temp_boxed, TypeInfo(target_kind));
                    }
                }
                else
                {
                    std::cout << "[CodeGen] No conversion needed for attribute '" << attr_name << "'." << std::endl;
                }

                // Step 4: Store the (potentially converted) value into the struct field.
                function_definitions_ << "  store " << store_type << " " << init_value << ", " << store_type << "* %" << field_ptr << "\n";
            }

            // Exit the constructor scope
            exitScope();
        }
        else
        {
            std::cout << "[CodeGen] Warning: No type information found for " << expr->typeName << ", leaving attributes uninitialized" << std::endl;
        }
    }

    current_value_ = "%" + result_name;
}

void CodeGenerator::visit(GetAttrExpr *expr)
{
    std::cout << "[CodeGen] Processing GetAttrExpr: " << expr->attrName << std::endl;

    // Visit the object to get its value
    expr->object->accept(this);
    std::string object_ptr = current_value_;

    // Get the object type
    std::string object_type_name = "Object"; // default
    if (expr->object->inferredType && !expr->object->inferredType->getTypeName().empty())
    {
        object_type_name = expr->object->inferredType->getTypeName();
    }

    // Look up the struct type
    auto type_it = types_.find(object_type_name);
    if (type_it == types_.end())
    {
        std::cerr << "[CodeGen] Error: Type " << object_type_name << " not found for attribute access" << std::endl;
        current_value_ = "null";
        return;
    }

    std::string struct_type = type_it->second;
    std::string result_name = generateUniqueName("getattr");

    // Get the attribute type from our mapping
    std::string load_type = getAttributeLLVMType(object_type_name, expr->attrName);

    // Find the attribute index using our direct mapping
    int attr_index = 0; // default
    auto attr_indices_it = attribute_indices_.find(object_type_name);
    if (attr_indices_it != attribute_indices_.end())
    {
        auto index_it = attr_indices_it->second.find(expr->attrName);
        if (index_it != attr_indices_it->second.end())
        {
            attr_index = index_it->second;
        }
        else
        {
            std::cout << "[CodeGen] Warning: Attribute " << expr->attrName << " not found in type " << object_type_name << ", using index 0" << std::endl;
        }
    }
    else
    {
        std::cout << "[CodeGen] Warning: No attribute indices found for type " << object_type_name << ", using index 0" << std::endl;
    }

    std::string field_ptr = generateUniqueName("field_ptr");

    getCurrentStream() << "  %" << field_ptr << " = getelementptr " << struct_type << ", " << struct_type << "* " << object_ptr << ", i32 0, i32 " << attr_index << "\n";

    std::cout << "[CodeGen] Loading attribute " << expr->attrName << " at index " << attr_index
              << " with LLVM type: " << load_type << std::endl;

    getCurrentStream() << "  %" << result_name << " = load " << load_type << ", " << load_type << "* %" << field_ptr << "\n";

    // If the expected return type is different from the loaded type, we may need to convert
    if (expr->inferredType && expr->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        std::string expected_type = getLLVMType(*expr->inferredType);
        if (expected_type != load_type)
        {
            std::cout << "[CodeGen] Converting attribute " << expr->attrName << " from " << load_type << " to " << expected_type << std::endl;

            if (load_type == "%struct.BoxedValue*" && expected_type != "%struct.BoxedValue*")
            {
                // Convert from BoxedValue to specific type
                std::string converted = generateUnboxedValue("%" + result_name, *expr->inferredType);
                current_value_ = converted;
                return;
            }
            else if (load_type != "%struct.BoxedValue*" && expected_type == "%struct.BoxedValue*")
            {
                // Convert from specific type to BoxedValue
                std::string converted = generateBoxedValue("%" + result_name, *expr->inferredType);
                current_value_ = converted;
                return;
            }
        }
    }

    current_value_ = "%" + result_name;
}

void CodeGenerator::visit(SetAttrExpr *expr)
{
    std::cout << "[CodeGen] Processing SetAttrExpr: " << expr->attrName << std::endl;

    // Visit the object to get its value
    expr->object->accept(this);
    std::string object_ptr = current_value_;

    // Visit the value to be assigned
    expr->value->accept(this);
    std::string value = current_value_;

    // Get the object type
    std::string object_type_name = "Object"; // default
    if (expr->object->inferredType && !expr->object->inferredType->getTypeName().empty())
    {
        object_type_name = expr->object->inferredType->getTypeName();
    }

    // Look up the struct type
    auto type_it = types_.find(object_type_name);
    if (type_it == types_.end())
    {
        std::cerr << "[CodeGen] Error: Type " << object_type_name << " not found for attribute assignment" << std::endl;
        current_value_ = "null";
        return;
    }

    std::string struct_type = type_it->second;

    // Get the attribute type from our mapping
    std::string store_type = getAttributeLLVMType(object_type_name, expr->attrName);

    // Find the attribute index using our direct mapping
    int attr_index = 0; // default
    auto attr_indices_it = attribute_indices_.find(object_type_name);
    if (attr_indices_it != attribute_indices_.end())
    {
        auto index_it = attr_indices_it->second.find(expr->attrName);
        if (index_it != attr_indices_it->second.end())
        {
            attr_index = index_it->second;
        }
        else
        {
            std::cout << "[CodeGen] Warning: Attribute " << expr->attrName << " not found in type " << object_type_name << ", using index 0" << std::endl;
        }
    }
    else
    {
        std::cout << "[CodeGen] Warning: No attribute indices found for type " << object_type_name << ", using index 0" << std::endl;
    }

    std::string field_ptr = generateUniqueName("field_ptr");
    getCurrentStream() << "  %" << field_ptr << " = getelementptr " << struct_type << ", " << struct_type << "* " << object_ptr << ", i32 0, i32 " << attr_index << "\n";

    std::cout << "[CodeGen] Setting attribute " << expr->attrName << " at index " << attr_index
              << " with LLVM type: " << store_type << std::endl;

    // Check if we need to convert the value to match the expected type
    if (expr->value->inferredType && expr->value->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        std::string value_type = getLLVMType(*expr->value->inferredType);
        if (value_type != store_type)
        {
            std::cout << "[CodeGen] Converting value from " << value_type << " to " << store_type << " for attribute " << expr->attrName << std::endl;

            if (store_type == "%struct.BoxedValue*" && value_type != "%struct.BoxedValue*")
            {
                // Convert from specific type to BoxedValue
                value = generateBoxedValue(value, *expr->value->inferredType);
            }
            else if (store_type != "%struct.BoxedValue*" && value_type == "%struct.BoxedValue*")
            {
                // Convert from BoxedValue to specific type
                value = generateUnboxedValue(value, TypeInfo(TypeInfo::Kind::Number)); // Default to number
            }
        }
    }

    getCurrentStream() << "  store " << store_type << " " << value << ", " << store_type << "* %" << field_ptr << "\n";

    current_value_ = value;
}

void CodeGenerator::visit(MethodCallExpr *expr)
{
    std::cout << "[CodeGen] Processing MethodCallExpr: " << expr->methodName << std::endl;

    // Visit the object to get its value
    expr->object->accept(this);
    std::string object_ptr = current_value_;

    // Get the object type name
    std::string object_type_name = "Object"; // default
    if (expr->object->inferredType && !expr->object->inferredType->getTypeName().empty())
    {
        object_type_name = expr->object->inferredType->getTypeName();
    }

    // Find the method in the inheritance chain
    std::string method_owner_type = object_type_name;
    std::string function_name = object_type_name + "_" + expr->methodName;
    
    // Check if method exists in current type
    auto type_methods_it = type_methods_.find(object_type_name);
    if (type_methods_it == type_methods_.end() || 
        type_methods_it->second.find(expr->methodName) == type_methods_it->second.end())
    {
        // Method not found in current type, search inheritance chain
        std::string inherited_type = findInheritedMethod(object_type_name, expr->methodName);
        if (!inherited_type.empty())
        {
            method_owner_type = inherited_type;
            function_name = inherited_type + "_" + expr->methodName;
            std::cout << "[CodeGen] Found inherited method " << expr->methodName 
                      << " in type " << inherited_type << std::endl;
        }
        else
        {
            std::cout << "[CodeGen] Warning: Method " << expr->methodName 
                      << " not found in type " << object_type_name << " or its inheritance chain" << std::endl;
            // For now, we'll continue with the current type and let LLVM handle the error
        }
    }

    std::string result_name = generateUniqueName("method_call");

    // Prepare arguments
    std::vector<std::string> args;
    std::vector<std::string> arg_types;

    // Add the object as the first argument (self)
    args.push_back(object_ptr);
    arg_types.push_back("%struct." + object_type_name + "*");

    // Add method arguments
    for (auto &arg : expr->args)
    {
        arg->accept(this);
        std::string arg_value = current_value_;
        std::string arg_type = "%struct.BoxedValue*"; // default to BoxedValue for method parameters

        if (arg->inferredType && arg->inferredType->getKind() != TypeInfo::Kind::Unknown)
        {
            // Convert specific types to BoxedValue for method parameters
            arg_type = "%struct.BoxedValue*";
            arg_value = generateBoxedValue(arg_value, *arg->inferredType);
            std::cout << "[CodeGen] Converting method argument to BoxedValue: "
                      << arg->inferredType->toString() << std::endl;
        }

        args.push_back(arg_value);
        arg_types.push_back(arg_type);
    }

    // Build argument list
    std::string arg_list;
    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            arg_list += ", ";
        arg_list += arg_types[i] + " " + args[i];
    }

    // Check the inferred return type of this call expression
    std::string return_type = "%struct.BoxedValue*"; // default to BoxedValue for method returns
    if (expr->inferredType && expr->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        return_type = getLLVMType(*expr->inferredType);
    }

    // Generate the function call with the correct return type
    getCurrentStream() << "  %" << result_name << " = call " << return_type << " @" << function_name << "(" << arg_list << ")\n";

    // If the method returns a BoxedValue but we expect a specific type, convert it
    if (return_type == "%struct.BoxedValue*" &&
        expr->inferredType && expr->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        std::string expected_type = getLLVMType(*expr->inferredType);
        if (expected_type != "%struct.BoxedValue*")
        {
            std::cout << "[CodeGen] Converting method return from BoxedValue to " << expected_type << std::endl;
            std::string converted = generateUnboxedValue("%" + result_name, *expr->inferredType);
            current_value_ = converted;
            return;
        }
    }

    current_value_ = "%" + result_name;
}

void CodeGenerator::visit(SelfExpr *expr)
{
    std::cout << "[CodeGen] Processing SelfExpr" << std::endl;

    // In a method context, self is the first parameter
    // We need to return the self pointer that was passed to the method
    current_value_ = "%self";
}

void CodeGenerator::visit(BaseCallExpr *expr)
{
    std::cout << "[CodeGen] Processing BaseCallExpr" << std::endl;

    // For now, we'll return a placeholder value
    // In a real implementation, we'd need to handle base class calls
    current_value_ = "null";
}

void CodeGenerator::visit(IsExpr *expr)
{
    std::cout << "[CodeGen] Processing IsExpr" << std::endl;

    // Evaluate the expression
    expr->expr->accept(this);
    std::string expr_value = current_value_;

    // For now, we'll return a placeholder boolean value
    // In a real implementation, we'd need to check the type at runtime
    std::string result_name = generateUniqueName("is_check");
    getCurrentStream() << "  %" << result_name << " = add i1 0, 1\n"; // Default to true
    current_value_ = "%" + result_name;
}

void CodeGenerator::visit(AsExpr *expr)
{
    std::cout << "[CodeGen] Processing AsExpr" << std::endl;

    // Evaluate the expression
    expr->expr->accept(this);
    std::string expr_value = current_value_;

    // For now, we'll return the expression value as is
    // In a real implementation, we'd need to perform type casting
    current_value_ = expr_value;
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
    // If we're in constructor context, always use function_definitions_
    if (in_constructor_context_)
    {
        return function_definitions_;
    }

    if (current_function_ == "main" || current_function_.empty())
    {
        return ir_code_;
    }
    else
    {
        return function_definitions_;
    }
}

// Boxed value helper functions
std::string CodeGenerator::generateBoxedValue(const std::string &value, const TypeInfo &type)
{
    std::string result_name = generateUniqueName("boxed");

    switch (type.getKind())
    {
    case TypeInfo::Kind::Number:
        getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxNumber(double " << value << ")\n";
        break;
    case TypeInfo::Kind::String:
        getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxString(i8* " << value << ")\n";
        break;
    case TypeInfo::Kind::Boolean:
        // Handle boolean values that are already i1
        if (value.find("%") == 0)
        {
            // It's already a register, use it directly
            getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxBoolean(i1 " << value << ")\n";
        }
        else if (value == "true" || value == "1" || value == "1.0")
        {
            getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxBoolean(i1 1)\n";
        }
        else if (value == "false" || value == "0" || value == "0.0")
        {
            getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxBoolean(i1 0)\n";
        }
        else
        {
            // Convert double to boolean
            std::string bool_conv = generateUniqueName("bool_conv");
            getCurrentStream() << "  %" << bool_conv << " = fcmp one double " << value << ", 0.0\n";
            getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxBoolean(i1 %" << bool_conv << ")\n";
        }
        break;
    case TypeInfo::Kind::Object:
        getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxObject(i8* " << value << ")\n";
        break;
    case TypeInfo::Kind::Unknown:
        // For unknown types, we need to determine the type at runtime
        // This is a simplified approach - in practice, you'd need more sophisticated type inference
        getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxNumber(double " << value << ")\n";
        break;
    default:
        getCurrentStream() << "  %" << result_name << " = call %struct.BoxedValue* @boxNull()\n";
        break;
    }

    return "%" + result_name;
}

std::string CodeGenerator::generateUnboxedValue(const std::string &boxed_value, const TypeInfo &target_type)
{
    std::string result_name = generateUniqueName("unboxed");

    switch (target_type.getKind())
    {
    case TypeInfo::Kind::Number:
        getCurrentStream() << "  %" << result_name << " = call double @unboxNumber(%struct.BoxedValue* " << boxed_value << ")\n";
        break;
    case TypeInfo::Kind::String:
        getCurrentStream() << "  %" << result_name << " = call i8* @unboxString(%struct.BoxedValue* " << boxed_value << ")\n";
        break;
    case TypeInfo::Kind::Boolean:
        getCurrentStream() << "  %" << result_name << " = call i1 @unboxBoolean(%struct.BoxedValue* " << boxed_value << ")\n";
        break;
    case TypeInfo::Kind::Object:
        getCurrentStream() << "  %" << result_name << " = call i8* @unboxObject(%struct.BoxedValue* " << boxed_value << ")\n";
        break;
    default:
        // For unknown target types, return the boxed value as is
        return boxed_value;
    }

    return "%" + result_name;
}

std::string CodeGenerator::generateBoxedValueType(const TypeInfo &type)
{
    switch (type.getKind())
    {
    case TypeInfo::Kind::Number:
        return "0"; // RuntimeType::NUMBER
    case TypeInfo::Kind::String:
        return "1"; // RuntimeType::STRING
    case TypeInfo::Kind::Boolean:
        return "2"; // RuntimeType::BOOLEAN
    case TypeInfo::Kind::Object:
        return "3"; // RuntimeType::OBJECT
    case TypeInfo::Kind::Unknown:
        return "5"; // RuntimeType::UNKNOWN
    default:
        return "4"; // RuntimeType::NULL_VALUE
    }
}

std::string CodeGenerator::generateBoxedValueAlloca(const std::string &var_name)
{
    std::string alloca_name = generateUniqueName(var_name + "_boxed");
    getCurrentStream() << "  %" << alloca_name << " = alloca %struct.BoxedValue*\n";
    return alloca_name;
}

std::string CodeGenerator::generateBoxedValueStore(const std::string &var_name, const std::string &value, const TypeInfo &type)
{
    std::string boxed_value = generateBoxedValue(value, type);
    getCurrentStream() << "  store %struct.BoxedValue* " << boxed_value << ", %struct.BoxedValue** %" << var_name << "\n";
    return boxed_value;
}

std::string CodeGenerator::generateBoxedValueLoad(const std::string &var_name)
{
    std::string load_name = generateUniqueName("load_boxed");
    getCurrentStream() << "  %" << load_name << " = load %struct.BoxedValue*, %struct.BoxedValue** %" << var_name << "\n";
    return "%" + load_name;
}

std::string CodeGenerator::generateBoxedValueOperation(const std::string &left, const std::string &right, const std::string &operation, const TypeInfo &left_type, const TypeInfo &right_type)
{
    std::string result_name = generateUniqueName("boxed_op");

    // If both operands are known types, we can optimize by doing the operation directly
    if (left_type.getKind() != TypeInfo::Kind::Unknown && right_type.getKind() != TypeInfo::Kind::Unknown)
    {
        // Both types are known, do direct operation
        if (left_type.getKind() == right_type.getKind())
        {
            if (operation == "add" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                std::cout << "[CodeGen] Adding numbers: " << left << " and " << right << std::endl;
                getCurrentStream() << "  %" << result_name << " = fadd double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "sub" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fsub double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "mul" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fmul double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "div" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fdiv double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "mod" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = frem double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "pow" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = call double @pow(double " << left << ", double " << right << ")\n";
                return "%" + result_name;
            }
            else if (operation == "eq" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp ueq double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "neq" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp une double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "lt" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp ult double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "gt" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp ugt double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "and" && left_type.getKind() == TypeInfo::Kind::Boolean)
            {
                // Handle boolean operations - check if operands are already i1 or need conversion
                std::string left_bool, right_bool;

                // Check if left operand is already i1 (starts with % and is a register)
                if (left.find("%") == 0)
                {
                    // Assume it's already i1 if it's a register
                    left_bool = left;
                }
                else
                {
                    // Convert double to i1
                    left_bool = generateUniqueName("left_bool");
                    getCurrentStream() << "  %" << left_bool << " = fcmp one double " << left << ", 0.0\n";
                    left_bool = "%" + left_bool;
                }

                // Check if right operand is already i1
                if (right.find("%") == 0)
                {
                    // Assume it's already i1 if it's a register
                    right_bool = right;
                }
                else
                {
                    // Convert double to i1
                    right_bool = generateUniqueName("right_bool");
                    getCurrentStream() << "  %" << right_bool << " = fcmp one double " << right << ", 0.0\n";
                    right_bool = "%" + right_bool;
                }

                getCurrentStream() << "  %" << result_name << " = and i1 " << left_bool << ", " << right_bool << "\n";
                return "%" + result_name;
            }
            else if (operation == "or" && left_type.getKind() == TypeInfo::Kind::Boolean)
            {
                // Handle boolean operations - check if operands are already i1 or need conversion
                std::string left_bool, right_bool;

                // Check if left operand is already i1 (starts with % and is a register)
                if (left.find("%") == 0)
                {
                    // Assume it's already i1 if it's a register
                    left_bool = left;
                }
                else
                {
                    // Convert double to i1
                    left_bool = generateUniqueName("left_bool");
                    getCurrentStream() << "  %" << left_bool << " = fcmp one double " << left << ", 0.0\n";
                    left_bool = "%" + left_bool;
                }

                // Check if right operand is already i1
                if (right.find("%") == 0)
                {
                    // Assume it's already i1 if it's a register
                    right_bool = right;
                }
                else
                {
                    // Convert double to i1
                    right_bool = generateUniqueName("right_bool");
                    getCurrentStream() << "  %" << right_bool << " = fcmp one double " << right << ", 0.0\n";
                    right_bool = "%" + right_bool;
                }

                getCurrentStream() << "  %" << result_name << " = or i1 " << left_bool << ", " << right_bool << "\n";
                return "%" + result_name;
            }
            else if ((operation == "concat" || operation == "concat_ws") &&
                     left_type.getKind() == TypeInfo::Kind::String &&
                     right_type.getKind() == TypeInfo::Kind::String)
            {
                if (operation == "concat")
                {
                    getCurrentStream() << "  %" << result_name << " = call i8* @concat_strings(i8* " << left << ", i8* " << right << ")\n";
                }
                else
                {
                    getCurrentStream() << "  %" << result_name << " = call i8* @concat_strings_ws(i8* " << left << ", i8* " << right << ")\n";
                }
                return "%" + result_name;
            }
            else if (operation == "gt" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp ugt double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "le" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp ule double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "ge" && left_type.getKind() == TypeInfo::Kind::Number)
            {
                getCurrentStream() << "  %" << result_name << " = fcmp uge double " << left << ", " << right << "\n";
                return "%" + result_name;
            }
            else if (operation == "and" && left_type.getKind() == TypeInfo::Kind::Boolean)
            {
                // Handle boolean operations - check if operands are already i1 or need conversion
                std::string left_bool, right_bool;

                // Check if left operand is already i1 (starts with % and is a register)
                if (left.find("%") == 0)
                {
                    // Assume it's already i1 if it's a register
                    left_bool = left;
                }
                else
                {
                    // Convert double to i1
                    left_bool = generateUniqueName("left_bool");
                    getCurrentStream() << "  %" << left_bool << " = fcmp one double " << left << ", 0.0\n";
                    left_bool = "%" + left_bool;
                }

                // Check if right operand is already i1
                if (right.find("%") == 0)
                {
                    // Assume it's already i1 if it's a register
                    right_bool = right;
                }
                else
                {
                    // Convert double to i1
                    right_bool = generateUniqueName("right_bool");
                    getCurrentStream() << "  %" << right_bool << " = fcmp one double " << right << ", 0.0\n";
                    right_bool = "%" + right_bool;
                }

                getCurrentStream() << "  %" << result_name << " = and i1 " << left_bool << ", " << right_bool << "\n";
                return "%" + result_name;
            }
        }
    }

    // Fallback to boxed value operations
    std::string left_boxed = left;
    std::string right_boxed = right;

    // Box the operands if they're not already boxed
    if (left_type.getKind() != TypeInfo::Kind::Unknown)
    {
        left_boxed = generateBoxedValue(left, left_type);
    }
    if (right_type.getKind() != TypeInfo::Kind::Unknown)
    {
        right_boxed = generateBoxedValue(right, right_type);
    }

    // Call the appropriate boxed operation function
    if (operation == "add")
    {
        getCurrentStream() << "  %" << result_name << " = call double @boxedAdd(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "sub")
    {
        getCurrentStream() << "  %" << result_name << " = call double @boxedSubtract(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "mul")
    {
        getCurrentStream() << "  %" << result_name << " = call double @boxedMultiply(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "div")
    {
        getCurrentStream() << "  %" << result_name << " = call double @boxedDivide(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "mod")
    {
        getCurrentStream() << "  %" << result_name << " = call double @boxedModulo(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "pow")
    {
        getCurrentStream() << "  %" << result_name << " = call double @boxedPower(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "eq")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedEqual(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "neq")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedNotEqual(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "lt")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedLessThan(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "gt")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedGreaterThan(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "and")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedLogicalAnd(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "or")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedLogicalOr(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "concat")
    {
        getCurrentStream() << "  %" << result_name << " = call i8* @boxedConcatenate(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "concat_ws")
    {
        getCurrentStream() << "  %" << result_name << " = call i8* @boxedConcatenateWithSpace(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "le")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedLessEqual(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else if (operation == "ge")
    {
        getCurrentStream() << "  %" << result_name << " = call i1 @boxedGreaterEqual(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }
    else
    {
        // Default to addition for unknown operations
        getCurrentStream() << "  %" << result_name << " = call double @boxedAdd(%struct.BoxedValue* " << left_boxed << ", %struct.BoxedValue* " << right_boxed << ")\n";
    }

    return "%" + result_name;
}

void CodeGenerator::generateConstructorFunction(TypeDecl *typeDecl)
{
    std::cout << "[CodeGen] Generating constructor function for type: " << typeDecl->name << std::endl;

    std::string constructor_name = "construct_" + typeDecl->name;
    std::string struct_type = "%struct." + typeDecl->name;

    // Build parameter list
    std::string param_list;
    std::vector<std::string> param_types;

    for (size_t i = 0; i < typeDecl->params.size(); ++i)
    {
        if (i > 0)
            param_list += ", ";

        // All constructor parameters are treated as BoxedValue for flexibility
        std::string param_type = "%struct.BoxedValue*";
        param_list += param_type + " %" + typeDecl->params[i];
        param_types.push_back(param_type);
    }

    // Generate constructor function
    function_definitions_ << "define " << struct_type << "* @" << constructor_name << "(" << param_list << ") {\n";
    function_definitions_ << "entry:\n";

    // Allocate object using malloc
    std::string obj_name = generateUniqueName("obj");
    int struct_size = calculateStructSize(typeDecl->name);
    std::string malloc_size = std::to_string(struct_size);
    function_definitions_ << "  %" << obj_name << "_raw = call i8* @malloc(i32 " << malloc_size << ")\n";
    function_definitions_ << "  %" << obj_name << " = bitcast i8* %" << obj_name << "_raw to " << struct_type << "*\n";

    // Register parameters in the scope for attribute initializers to access
    for (size_t i = 0; i < typeDecl->params.size(); ++i)
    {
        std::string param_name = typeDecl->params[i];
        current_scope_->variables[param_name] = param_name;
    }

    // Handle inheritance: initialize base object
    if (typeDecl->baseType != "Object")
    {
        std::cout << "[CodeGen] Initializing base object for " << typeDecl->name << std::endl;
        
        // Evaluate base arguments if provided
        std::vector<std::string> base_args;
        if (!typeDecl->baseArgs.empty())
        {
            // Use explicit base arguments if provided
            std::cout << "[CodeGen] Using explicit base arguments for " << typeDecl->name << std::endl;
            for (const auto &baseArg : typeDecl->baseArgs)
            {
                baseArg->accept(this);
                std::string arg_value = current_value_;
                
                // Convert to BoxedValue if needed
                if (baseArg->inferredType && baseArg->inferredType->getKind() != TypeInfo::Kind::Unknown)
                {
                    arg_value = generateBoxedValue(arg_value, *baseArg->inferredType);
                }
                
                base_args.push_back(arg_value);
            }
        }
        else
        {
            // No base arguments provided, pass constructor parameters directly
            std::cout << "[CodeGen] Using constructor parameters as base arguments for " << typeDecl->name << std::endl;
            for (const auto &param : typeDecl->params)
            {
                base_args.push_back("%" + param);
            }
        }

        // Call base constructor
        std::string base_constructor = "construct_" + typeDecl->baseType;
        std::string base_arg_list;
        for (size_t i = 0; i < base_args.size(); ++i)
        {
            if (i > 0) base_arg_list += ", ";
            base_arg_list += "%struct.BoxedValue* " + base_args[i];
        }

        std::string base_obj = generateUniqueName("base_obj");
        function_definitions_ << "  %" << base_obj << " = call %struct." << typeDecl->baseType 
                             << "* @" << base_constructor << "(" << base_arg_list << ")\n";

        // Store base object pointer in the first field
        std::string base_ptr_field = generateUniqueName("base_ptr_field");
        function_definitions_ << "  %" << base_ptr_field << " = getelementptr " << struct_type 
                             << ", " << struct_type << "* %" << obj_name << ", i32 0, i32 0\n";
        function_definitions_ << "  store %struct." << typeDecl->baseType << "* %" << base_obj 
                             << ", %struct." << typeDecl->baseType << "** %" << base_ptr_field << "\n";
    }

    // Get attribute types for this type
    auto attr_types_it = attribute_types_.find(typeDecl->name);
    if (attr_types_it != attribute_types_.end())
    {
        const auto &attr_types = attr_types_it->second;

        // Set context variables for attribute initialization
        current_object_ptr_ = "%" + obj_name;
        current_struct_type_ = struct_type;
        in_constructor_context_ = true; // Set constructor context

        // Initialize attributes by calling visit(AttributeDecl *) for each one
        // Note: attributes start at index 1 if there's inheritance, 0 otherwise
        int attr_offset = (typeDecl->baseType != "Object") ? 1 : 0;
        for (size_t i = 0; i < typeDecl->attributes.size(); ++i)
        {
            current_attr_index_ = i + attr_offset;
            typeDecl->attributes[i]->accept(this);
        }

        in_constructor_context_ = false; // Reset constructor context
    }
    else
    {
        // Fallback: treat all attributes as BoxedValue
        current_object_ptr_ = "%" + obj_name;
        current_struct_type_ = struct_type;
        in_constructor_context_ = true; // Set constructor context

        int attr_offset = (typeDecl->baseType != "Object") ? 1 : 0;
        for (size_t i = 0; i < typeDecl->attributes.size(); ++i)
        {
            current_attr_index_ = i + attr_offset;
            typeDecl->attributes[i]->accept(this);
        }

        in_constructor_context_ = false; // Reset constructor context
    }

    // Return the object
    function_definitions_ << "  ret " << struct_type << "* %" << obj_name << "\n";
    function_definitions_ << "}\n\n";

    // Register the constructor function
    functions_[constructor_name] = struct_type + "*";

    std::cout << "[CodeGen] Generated constructor function: " << constructor_name << std::endl;
}

std::vector<std::string> CodeGenerator::getInheritedAttributes(const std::string &typeName)
{
    std::vector<std::string> all_attributes;

    // Get current type's attributes
    auto attr_indices_it = attribute_indices_.find(typeName);
    if (attr_indices_it != attribute_indices_.end())
    {
        for (const auto &attr_pair : attr_indices_it->second)
        {
            all_attributes.push_back(attr_pair.first);
        }
    }

    // Get parent type's attributes recursively
    auto inheritance_it = type_inheritance_.find(typeName);
    if (inheritance_it != type_inheritance_.end())
    {
        std::string parent_type = inheritance_it->second;
        std::vector<std::string> parent_attributes = getInheritedAttributes(parent_type);
        all_attributes.insert(all_attributes.begin(), parent_attributes.begin(), parent_attributes.end());
    }

    return all_attributes;
}

void CodeGenerator::generateInheritedMethodWrappers(TypeDecl *typeDecl)
{
    std::cout << "[CodeGen] Generating inherited method wrappers for type: " << typeDecl->name << std::endl;

    // Get the base type
    std::string baseType = typeDecl->baseType;
    
    // Get methods defined in the current type
    std::set<std::string> currentMethods;
    for (const auto &method : typeDecl->methods)
    {
        currentMethods.insert(method->name);
    }

    // Get methods from the base type
    auto baseTypeDecl = type_declarations_.find(baseType);
    if (baseTypeDecl == type_declarations_.end())
    {
        std::cout << "[CodeGen] Warning: Base type " << baseType << " not found for " << typeDecl->name << std::endl;
        return;
    }

    // For each method in the base type that is not overridden
    for (const auto &baseMethod : baseTypeDecl->second->methods)
    {
        if (currentMethods.find(baseMethod->name) == currentMethods.end())
        {
            // Method not overridden, generate wrapper
            generateMethodWrapper(typeDecl->name, baseType, baseMethod.get());
        }
        else
        {
            std::cout << "[CodeGen] Method " << baseMethod->name << " is overridden in " << typeDecl->name << std::endl;
        }
    }
}

void CodeGenerator::generateMethodWrapper(const std::string &childType, const std::string &baseType, MethodDecl *baseMethod)
{
    std::cout << "[CodeGen] Generating wrapper for inherited method " << baseMethod->name 
              << " from " << baseType << " to " << childType << std::endl;

    // Determine return type
    std::string return_type = "%struct.BoxedValue*";
    if (baseMethod->inferredType && baseMethod->inferredType->getKind() == TypeInfo::Kind::Void)
    {
        return_type = "void";
    }
    else if (baseMethod->inferredType && baseMethod->inferredType->getKind() != TypeInfo::Kind::Unknown)
    {
        return_type = getLLVMType(*baseMethod->inferredType);
    }

    // Build parameter list starting with self
    std::string param_list = "%struct." + childType + "* %self";

    // Add method parameters
    for (size_t i = 0; i < baseMethod->params.size(); ++i)
    {
        std::string param_type = "%struct.BoxedValue*";
        param_list += ", " + param_type + " %" + baseMethod->params[i];
    }

    // Generate function definition
    std::string function_name = childType + "_" + baseMethod->name;
    function_definitions_ << "define " << return_type << " @" << function_name << "(" << param_list << ") {\n";
    function_definitions_ << "entry:\n";

    // Get the base pointer from the child object
    std::string base_ptr = generateUniqueName("base_ptr");
    function_definitions_ << "  %" << base_ptr << " = getelementptr %struct." << childType 
                         << ", %struct." << childType << "* %self, i32 0, i32 0\n";
    std::string base_obj = generateUniqueName("base_obj");
    function_definitions_ << "  %" << base_obj << " = load %struct." << baseType 
                         << "*, %struct." << baseType << "** %" << base_ptr << "\n";

    // Build argument list for the base method call
    std::string arg_list = "%struct." + baseType + "* %" + base_obj;
    for (size_t i = 0; i < baseMethod->params.size(); ++i)
    {
        arg_list += ", %struct.BoxedValue* %" + baseMethod->params[i];
    }

    // Call the base method
    std::string result_name = generateUniqueName("inherited_call");
    std::string base_function_name = baseType + "_" + baseMethod->name;
    function_definitions_ << "  %" << result_name << " = call " << return_type 
                         << " @" << base_function_name << "(" << arg_list << ")\n";

    // Return the result
    if (return_type != "void")
    {
        function_definitions_ << "  ret " << return_type << " %" << result_name << "\n";
    }
    else
    {
        function_definitions_ << "  ret void\n";
    }

    function_definitions_ << "}\n\n";

    // Register the wrapper function
    functions_[function_name] = return_type;
    
    // Register the method in the inheritance system
    type_methods_[childType].insert(baseMethod->name);
    std::string method_signature = getMethodSignature(baseMethod);
    method_signatures_[childType + "." + baseMethod->name] = method_signature;
    
    std::cout << "[CodeGen] Generated wrapper function: " << function_name << std::endl;
}

std::string CodeGenerator::getMethodSignature(MethodDecl *method)
{
    std::string signature = "(";
    
    // Add parameter types
    for (size_t i = 0; i < method->paramTypes.size(); ++i)
    {
        if (i > 0) signature += ",";
        signature += method->paramTypes[i]->toString();
    }
    
    signature += ")->";
    
    // Add return type
    if (method->inferredType)
    {
        signature += method->inferredType->toString();
    }
    else
    {
        signature += "Unknown";
    }
    
    return signature;
}

bool CodeGenerator::isMethodOverridden(const std::string &typeName, const std::string &methodName)
{
    auto typeDecl = type_declarations_.find(typeName);
    if (typeDecl == type_declarations_.end())
    {
        return false;
    }

    for (const auto &method : typeDecl->second->methods)
    {
        if (method->name == methodName)
        {
            return true;
        }
    }

    return false;
}

std::string CodeGenerator::findInheritedMethod(const std::string &typeName, const std::string &methodName)
{
    auto inheritance_it = inheritance_chain_.find(typeName);
    if (inheritance_it == inheritance_chain_.end())
    {
        return ""; // No inheritance
    }

    std::string currentType = inheritance_it->second;
    
    // Search up the inheritance chain
    while (currentType != "Object")
    {
        if (isMethodOverridden(currentType, methodName))
        {
            return currentType;
        }
        
        auto next_it = inheritance_chain_.find(currentType);
        if (next_it == inheritance_chain_.end())
        {
            break;
        }
        currentType = next_it->second;
    }

    return ""; // Method not found in inheritance chain
}

int CodeGenerator::calculateStructSize(const std::string &typeName)
{
    // Get the attribute types for this type
    auto attr_types_it = attribute_types_.find(typeName);
    if (attr_types_it == attribute_types_.end())
    {
        std::cout << "[CodeGen] Warning: No attribute types found for " << typeName << ", using default size 16" << std::endl;
        return 16; // Default size
    }

    const auto &attr_types = attr_types_it->second;
    int total_size = 0;

    // Calculate size based on field types
    for (const auto &attr_pair : attr_types)
    {
        std::string field_type = attr_pair.second;
        
        if (field_type == "double")
        {
            total_size += 8; // 8 bytes for double
        }
        else if (field_type == "i1")
        {
            total_size += 1; // 1 byte for boolean
        }
        else if (field_type == "i8*")
        {
            total_size += 8; // 8 bytes for pointer
        }
        else if (field_type.find("%struct.") == 0)
        {
            total_size += 8; // 8 bytes for struct pointer
        }
        else if (field_type == "%struct.BoxedValue*")
        {
            total_size += 8; // 8 bytes for BoxedValue pointer
        }
        else
        {
            total_size += 8; // Default to 8 bytes for unknown types
        }
    }

    // Ensure minimum size and alignment
    if (total_size < 8)
    {
        total_size = 8;
    }
    
    // Align to 8-byte boundary
    total_size = ((total_size + 7) / 8) * 8;

    std::cout << "[CodeGen] Calculated size for " << typeName << ": " << total_size << " bytes" << std::endl;
    return total_size;
}