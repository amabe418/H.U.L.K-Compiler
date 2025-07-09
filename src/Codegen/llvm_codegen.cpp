#include "llvm_codegen.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

// Include AST headers
#include "../AST/ast.hpp"

LLVMCodeGenerator::LLVMCodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(nullptr),
      builder_(nullptr),
      current_function_(nullptr),
      current_block_(nullptr),
      current_value_(nullptr),
      var_counter_(0),
      current_scope_(nullptr)
{
    // Create initial scope
    scopes_.push_back(std::make_unique<LLVMScope>());
    current_scope_ = scopes_.back().get();
}

LLVMCodeGenerator::~LLVMCodeGenerator() = default;

void LLVMCodeGenerator::initialize(const std::string& module_name)
{
    std::cout << "[LLVM CodeGen] Initializing with module name: " << module_name << std::endl;
    
    // Create module
    module_ = std::make_unique<llvm::Module>(module_name, *context_);
    
    // Create IR builder
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
    
    // Register built-in functions
    registerBuiltinFunctions();
}

void LLVMCodeGenerator::generateCode(Program *program)
{
    program->accept(this);
}

void LLVMCodeGenerator::printIR()
{
    std::cout << "\n=== Generated LLVM IR ===" << std::endl;
    module_->print(llvm::outs(), nullptr);
    std::cout << "=== End LLVM IR ===" << std::endl;
}

void LLVMCodeGenerator::writeIRToFile(const std::string& filename)
{
    std::error_code ec;
    llvm::raw_fd_ostream file(filename, ec);
    
    if (ec) {
        std::cerr << "[LLVM CodeGen] Error opening file: " << filename << std::endl;
        return;
    }
    
    module_->print(file, nullptr);
    file.close();
    
    std::cout << "[LLVM CodeGen] IR written to file: " << filename << std::endl;
}

std::string LLVMCodeGenerator::getIR() const
{
    std::string result;
    llvm::raw_string_ostream stream(result);
    module_->print(stream, nullptr);
    return result;
}

// StmtVisitor implementations
void LLVMCodeGenerator::visit(Program *stmt)
{
    std::cout << "[LLVM CodeGen] Processing Program with " << stmt->stmts.size() << " statements" << std::endl;
    // Crear función main
    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_), false);
    current_function_ = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", module_.get());
    current_block_ = llvm::BasicBlock::Create(*context_, "entry", current_function_);
    builder_->SetInsertPoint(current_block_);

    // Initialize random number generator with current time
    std::cout << "[LLVM CodeGen] Initializing random number generator with srand(time(NULL))" << std::endl;
    llvm::Function* timeFunc = module_->getFunction("time");
    llvm::Function* srandFunc = module_->getFunction("srand");
    
    if (timeFunc && srandFunc) {
        // Call time(NULL) to get current time
        llvm::Value* nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt64Ty(*context_), 0));
        llvm::Value* currentTime = builder_->CreateCall(timeFunc, {nullPtr});
        
        // Convert time_t (int64) to int32 for srand
        llvm::Value* seed = builder_->CreateTrunc(currentTime, llvm::Type::getInt32Ty(*context_));
        
        // Call srand(seed)
        builder_->CreateCall(srandFunc, {seed});
        std::cout << "[LLVM CodeGen] Random number generator initialized successfully" << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] Warning: Could not initialize random number generator (time or srand not found)" << std::endl;
    }

    // Save the main entry block
    llvm::BasicBlock* mainBlock = current_block_;
    
    // Procesar cada statement
    for (auto &s : stmt->stmts) {
        s->accept(this);
        
        // After each statement, ensure we have a valid insertion point for the next statement
        // Some operations create complex control flow and may leave the builder in a merge block
        // which is fine - we just continue from there
        llvm::BasicBlock* currentBlock = builder_->GetInsertBlock();
        
        if (currentBlock && currentBlock != mainBlock) {
            std::cout << "[LLVM CodeGen] Builder moved to merge block after complex operation" << std::endl;
            // Update mainBlock to be the current block for linear continuation
            mainBlock = currentBlock;
            current_block_ = currentBlock;
        }
        
        // Si current_value_ es un valor, simplemente lo dejamos generado en el IR
        // (no se usa, pero existe como instrucción)
    }

    builder_->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
    current_function_ = nullptr;
    current_block_ = nullptr;
}

void LLVMCodeGenerator::visit(ExprStmt *stmt)
{
    std::cout << "[LLVM CodeGen] Processing ExprStmt" << std::endl;
    stmt->expr->accept(this);
}

void LLVMCodeGenerator::visit(FunctionDecl *stmt)
{
    std::cout << "[LLVM CodeGen] Processing FunctionDecl: " << stmt->name << std::endl;

    // Save current function context
    llvm::Function* savedFunction = current_function_;
    llvm::BasicBlock* savedBlock = current_block_;
    std::cout << "[LLVM CodeGen] Saving function context before processing function declaration" << std::endl;

    // Get return type
    llvm::Type* returnType = getLLVMType(*stmt->returnType);

    // Get parameter types
    std::vector<llvm::Type*> paramTypes;
    for (const auto& paramType : stmt->paramTypes)
    {
        paramTypes.push_back(getLLVMType(*paramType));
    }

    // Create function
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    current_function_ = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        stmt->name,
        module_.get()
    );

    // Create entry block
    current_block_ = llvm::BasicBlock::Create(*context_, "entry", current_function_);
    builder_->SetInsertPoint(current_block_);

    // Set up parameters
    size_t i = 0;
    for (auto &arg : current_function_->args())
    {
        std::string paramName = stmt->params[i];
        llvm::AllocaInst* alloca = builder_->CreateAlloca(arg.getType(), nullptr, paramName);
        builder_->CreateStore(&arg, alloca);
        current_scope_->variables[paramName] = alloca;
        current_scope_->variable_types[paramName] = arg.getType();
        i++;
    }

    // Generate function body
    stmt->body->accept(this);

    // Add return statement if needed
    if (returnType->isVoidTy())
    {
        builder_->CreateRetVoid();
    }
    else if (current_value_)
    {
        // Check if we need to box the return value
        if (returnType->isPointerTy()) {
            llvm::StructType* boxedTy = getBoxedValueType();
            llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
            
            // If the return type is BoxedValue* but current_value_ is not, box it
            if (returnType == boxedPtrTy && current_value_->getType() != boxedPtrTy) {
                std::cout << "[LLVM CodeGen] Boxing return value for BoxedValue* return type" << std::endl;
                if (current_value_->getType()->isDoubleTy()) {
                    current_value_ = createBoxedFromDouble(current_value_);
                } else if (current_value_->getType()->isIntegerTy(32)) {
                    llvm::Value* doubleValue = builder_->CreateSIToFP(current_value_, llvm::Type::getDoubleTy(*context_));
                    current_value_ = createBoxedFromDouble(doubleValue);
                } else if (current_value_->getType()->isIntegerTy(1)) {
                    current_value_ = createBoxedFromBool(current_value_);
                } else if (current_value_->getType()->isPointerTy()) {
                    current_value_ = createBoxedFromString(current_value_);
                }
            }
        }
        builder_->CreateRet(current_value_);
    }
    else
    {
        // Default return value
        if (returnType->isIntegerTy())
        {
            builder_->CreateRet(llvm::ConstantInt::get(returnType, 0));
        }
        else if (returnType->isDoubleTy())
        {
            builder_->CreateRet(llvm::ConstantFP::get(returnType, 0.0));
        }
        else if (returnType->isPointerTy())
        {
            llvm::StructType* boxedTy = getBoxedValueType();
            llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
            
            // If the return type is BoxedValue*, create a default boxed value
            if (returnType == boxedPtrTy) {
                std::cout << "[LLVM CodeGen] Creating default BoxedValue* return value (0.0)" << std::endl;
                llvm::Value* defaultValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
                llvm::Value* boxedDefault = createBoxedFromDouble(defaultValue);
                builder_->CreateRet(boxedDefault);
            } else {
                builder_->CreateRet(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(returnType)));
            }
        }
        else
        {
            builder_->CreateRet(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(returnType)));
        }
    }

    // Restore function context
    current_function_ = savedFunction;
    current_block_ = savedBlock;
    if (current_function_ && current_block_) {
        builder_->SetInsertPoint(current_block_);
        std::cout << "[LLVM CodeGen] Restored function context after processing function declaration" << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] No function context to restore (was processing top-level)" << std::endl;
    }
}

void LLVMCodeGenerator::visit(TypeDecl *stmt)
{
    std::cout << "[LLVM CodeGen] Processing TypeDecl: " << stmt->name << std::endl;

    current_type_ = stmt->name;
    
    // Store type declaration for inheritance processing
    type_declarations_[stmt->name] = stmt;

    // Create struct type with inheritance support
    std::vector<llvm::Type*> fieldTypes;
    std::vector<std::string> attributeNames;
    
    // First field is always vtable pointer
    llvm::PointerType* vtablePtrType = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    fieldTypes.push_back(vtablePtrType);
    attributeNames.push_back("__vtable__");
    
    // Add base type fields directly (flattened inheritance)
    if (stmt->baseType != "Object") {
        addBaseTypeFields(stmt->baseType, fieldTypes, attributeNames);
    }

    // Add own attributes (always BoxedValue* for user-defined types)
    for (const auto& attr : stmt->attributes)
    {
        // All attributes of user-defined types are always BoxedValue* 
        llvm::StructType* boxedTy = getBoxedValueType();
        fieldTypes.push_back(llvm::PointerType::get(boxedTy, 0));
        attributeNames.push_back(attr->name);
    }

    // Create the struct type
    llvm::StructType* structType = llvm::StructType::create(*context_, fieldTypes, "struct." + stmt->name);
    types_[stmt->name] = structType;
    
    // Store attribute metadata
    std::vector<std::string> userAttributeNames;
    for (const auto& attr : stmt->attributes) {
        userAttributeNames.push_back(attr->name);
    }
    storeTypeMetadata(stmt->name, userAttributeNames);

    // Process methods first
    for (const auto &method : stmt->methods)
    {
        method->accept(this);
    }
    
    // Generate vtable for this type (before constructor so it's available)
    generateVTable(stmt);

    // Generate constructor (after vtable so it can initialize it)
    generateConstructorFunction(stmt);

    current_type_ = "";
}

void LLVMCodeGenerator::visit(MethodDecl *stmt)
{
    std::cout << "[LLVM CodeGen] Processing MethodDecl: " << stmt->name << std::endl;
    generateMethodFunction(stmt);
}

void LLVMCodeGenerator::visit(AttributeDecl *stmt)
{
    std::cout << "[LLVM CodeGen] Processing AttributeDecl: " << stmt->name << std::endl;
    // This is handled during type initialization
}

// ExprVisitor implementations
void LLVMCodeGenerator::visit(NumberExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing NumberExpr: " << expr->value << std::endl;
    
    // All numbers are now always double
    current_value_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), expr->value);
    std::cout << "[LLVM CodeGen] NumberExpr created as double: " << expr->value << std::endl;
}

void LLVMCodeGenerator::visit(StringExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing StringExpr: " << expr->value << std::endl;
    
    // Create string as i8* first
    auto strGlobal = registerStringConstant(expr->value);
    auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    llvm::Value* strPtr = builder_->CreateInBoundsGEP(
        strGlobal->getValueType(),
        strGlobal,
        {zero, zero},
        "str_ptr"
    );
    
    // Wrap in BoxedValue for dynamic typing (strings are dynamic in HULK)
    current_value_ = createBoxedFromString(strPtr);
    
    std::cout << "[LLVM CodeGen] StringExpr created as BoxedValue* (dynamic type)" << std::endl;
}

void LLVMCodeGenerator::visit(BooleanExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing BooleanExpr: " << expr->value << std::endl;
    
    // Create native boolean type
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), expr->value);
    
    std::cout << "[LLVM CodeGen] BooleanExpr created as i1: " << expr->value << std::endl;
}

void LLVMCodeGenerator::visit(UnaryExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing UnaryExpr" << std::endl;
    expr->operand->accept(this);
    llvm::Value* operand = current_value_;
    
    // Si es una variable local, cargar su valor
    // if (llvm::AllocaInst* alloca = llvm::dyn_cast<llvm::AllocaInst>(operand)) {
    //     operand = builder_->CreateLoad(alloca->getAllocatedType(), alloca);
    // }

    switch (expr->op)
    {
    case UnaryExpr::OP_NEG:
        // Determinar el tipo y aplicar la negación apropiada
        if (operand->getType()->isDoubleTy()) {
            current_value_ = builder_->CreateFNeg(operand);
        } else if (operand->getType()->isIntegerTy(32)) {
            current_value_ = builder_->CreateNeg(operand);
        } else {
            // Para otros tipos, intentar convertir a double
            operand = builder_->CreateSIToFP(operand, llvm::Type::getDoubleTy(*context_));
            current_value_ = builder_->CreateFNeg(operand);
        }
        break;
    default:
        current_value_ = operand;
        break;
    }
}

void LLVMCodeGenerator::visit(BinaryExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing BinaryExpr with mixed-type support" << std::endl;

    // Evaluar el operando izquierdo
    expr->left->accept(this);
    llvm::Value* left = current_value_;
    
    // Evaluar el operando derecho
    expr->right->accept(this);
    llvm::Value* right = current_value_;

    // Detectar tipos de operandos
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
    
    bool leftIsBoxed = (left->getType() == boxedPtrTy);
    bool rightIsBoxed = (right->getType() == boxedPtrTy);
    bool leftIsNative = !leftIsBoxed && (left->getType()->isIntegerTy(32) || 
                                         left->getType()->isDoubleTy() || 
                                         left->getType()->isIntegerTy(1));
    bool rightIsNative = !rightIsBoxed && (right->getType()->isIntegerTy(32) || 
                                           right->getType()->isDoubleTy() || 
                                           right->getType()->isIntegerTy(1));

    std::cout << "[LLVM CodeGen] BinaryExpr operand types:" << std::endl;
    std::cout << "  - Left: " << (leftIsBoxed ? "BoxedValue*" : leftIsNative ? "Native" : "Other") << std::endl;
    std::cout << "  - Right: " << (rightIsBoxed ? "BoxedValue*" : rightIsNative ? "Native" : "Other") << std::endl;

    // Manejar operaciones aritméticas con tipos mixtos
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_add(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_add(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_add_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_add_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_add_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_add_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native addition - promoting all to double" << std::endl;
            // Always promote to double
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFAdd(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for addition" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_SUB:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_sub(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_sub(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_sub_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_sub_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_sub_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_sub_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native subtraction - promoting all to double" << std::endl;
            // Always promote to double
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFSub(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for subtraction" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_MUL:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_mul(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_mul(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_mul_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_mul_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_mul_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_mul_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native multiplication - promoting all to double" << std::endl;
            // Always promote to double
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFMul(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for multiplication" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_DIV:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_div(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_div(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_div_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_div_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_div_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_div_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native division" << std::endl;
            // División siempre produce double
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFDiv(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for division" << std::endl;
            current_value_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
        }
        break;

    case BinaryExpr::OP_POW:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_pow(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_pow(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_pow_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_pow_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_pow_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_pow_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native power" << std::endl;
            // Both native, use traditional pow
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            llvm::Function* powFunc = module_->getFunction("pow");
            if (!powFunc) {
                std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
                llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
                powFunc = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
            }
            std::vector<llvm::Value*> args = {left, right};
            current_value_ = builder_->CreateCall(powFunc, args);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for power" << std::endl;
            current_value_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
        }
        break;

    case BinaryExpr::OP_MOD:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_mod(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_mod(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_mod_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_mod_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_mod_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_mod_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native modulo - always floating-point" << std::endl;
            // Always use floating point modulo with fmod
            llvm::Function* fmodFunc = module_->getFunction("fmod");
            if (!fmodFunc) {
                std::vector<llvm::Type*> fmodArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
                llvm::FunctionType* fmodType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), fmodArgs, false);
                fmodFunc = llvm::Function::Create(fmodType, llvm::Function::ExternalLinkage, "fmod", module_.get());
            }
            // Convert to double if needed
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            std::vector<llvm::Value*> args = {left, right};
            current_value_ = builder_->CreateCall(fmodFunc, args);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for modulo" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        }
        break;

    // Comparison operations with BoxedValue support
    case BinaryExpr::OP_GT:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_greater(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_greater(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_greater_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_greater_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_greater_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_greater_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native greater than - always double comparison" << std::endl;
            // Always convert to double for comparison
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFCmpOGT(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for greater than" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_LT:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_lessthan(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_lessthan(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_lessthan_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_lessthan_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_lessthan_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_lessthan_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native less than - always double comparison" << std::endl;
            // Always convert to double for comparison
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFCmpOLT(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for less than" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_GE:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_ge(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_ge(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_ge_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_ge_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_ge_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_ge_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native greater than or equal - always double comparison" << std::endl;
            // Always convert to double for comparison
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFCmpOGE(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for greater than or equal" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_LE:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_le(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_le(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_le_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_le_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_le_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_le_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native less than or equal - always double comparison" << std::endl;
            // Always convert to double for comparison
            if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFCmpOLE(left, right);
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for less than or equal" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    // Equality operations with BoxedValue support
    case BinaryExpr::OP_EQ:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_equals(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_equals(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_equals_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_equals_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_equals_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_equals_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native equality" << std::endl;
            // Handle different types
            if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) {
                // Boolean comparison
                current_value_ = builder_->CreateICmpEQ(left, right);
            } else if (left->getType()->isPointerTy() && right->getType()->isPointerTy()) {
                // String comparison using strcmp
                llvm::Function* strcmpFunc = module_->getFunction("strcmp");
                if (!strcmpFunc) {
                    std::vector<llvm::Type*> strcmpArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                                           llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
                    llvm::FunctionType* strcmpType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), strcmpArgs, false);
                    strcmpFunc = llvm::Function::Create(strcmpType, llvm::Function::ExternalLinkage, "strcmp", module_.get());
                }
                std::vector<llvm::Value*> strcmpArgs = {left, right};
                llvm::Value* strcmpResult = builder_->CreateCall(strcmpFunc, strcmpArgs);
                current_value_ = builder_->CreateICmpEQ(strcmpResult, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
            } else if ((left->getType()->isIntegerTy(32) || left->getType()->isDoubleTy()) && 
                       (right->getType()->isIntegerTy(32) || right->getType()->isDoubleTy())) {
                // Numeric comparison - always convert to double
                if (!left->getType()->isDoubleTy()) {
                    left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
                }
                if (!right->getType()->isDoubleTy()) {
                    right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
                }
                current_value_ = builder_->CreateFCmpOEQ(left, right);
            } else {
                // Mixed types - different types are never equal
                current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
            }
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for equality" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_NEQ:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_neq(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_neq(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_neq_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_neq_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_neq_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_neq_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native not equal" << std::endl;
            // Handle different types
            if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) {
                // Boolean comparison
                current_value_ = builder_->CreateICmpNE(left, right);
            } else if (left->getType()->isPointerTy() && right->getType()->isPointerTy()) {
                // String comparison using strcmp
                llvm::Function* strcmpFunc = module_->getFunction("strcmp");
                if (!strcmpFunc) {
                    std::vector<llvm::Type*> strcmpArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                                           llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
                    llvm::FunctionType* strcmpType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), strcmpArgs, false);
                    strcmpFunc = llvm::Function::Create(strcmpType, llvm::Function::ExternalLinkage, "strcmp", module_.get());
                }
                std::vector<llvm::Value*> strcmpArgs = {left, right};
                llvm::Value* strcmpResult = builder_->CreateCall(strcmpFunc, strcmpArgs);
                current_value_ = builder_->CreateICmpNE(strcmpResult, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
            } else if ((left->getType()->isIntegerTy(32) || left->getType()->isDoubleTy()) && 
                       (right->getType()->isIntegerTy(32) || right->getType()->isDoubleTy())) {
                // Numeric comparison - always convert to double
                if (!left->getType()->isDoubleTy()) {
                    left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
                }
                if (!right->getType()->isDoubleTy()) {
                    right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
                }
                current_value_ = builder_->CreateFCmpONE(left, right);
            } else {
                // Mixed types - different types are always not equal
                current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
            }
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for not equal" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    // Logical operations with BoxedValue support
    case BinaryExpr::OP_AND:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_and(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_and(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_and_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_and_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_and_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_and_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native logical AND" << std::endl;
            // Both native, use traditional logical AND with short-circuit
            llvm::Value* leftBool = convertToBool(left);
            
            // Create basic blocks for short-circuit evaluation
            llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_and_native", current_function_);
            llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_and_native", current_function_);
            llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_and_native", current_function_);
            
            // If left is false, short-circuit to false; otherwise evaluate right
            builder_->CreateCondBr(leftBool, evalRightBlock, shortCircuitBlock);
            
            // Evaluate right operand
            builder_->SetInsertPoint(evalRightBlock);
            llvm::Value* rightBool = convertToBool(right);
            builder_->CreateBr(mergeBlock);
            
            // Short-circuit case (left was false)
            builder_->SetInsertPoint(shortCircuitBlock);
            llvm::Value* falseResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
            builder_->CreateBr(mergeBlock);
            
            // Merge results
            builder_->SetInsertPoint(mergeBlock);
            llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "and_result");
            resultPhi->addIncoming(rightBool, evalRightBlock);
            resultPhi->addIncoming(falseResult, shortCircuitBlock);
            current_value_ = resultPhi;
            } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for logical AND" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_OR:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_or(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_or(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_or_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_or_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_or_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_or_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native logical OR" << std::endl;
            // Both native, use traditional logical OR with short-circuit
            llvm::Value* leftBool = convertToBool(left);
            
            // Create basic blocks for short-circuit evaluation
            llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_or_native", current_function_);
            llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_or_native", current_function_);
            llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_or_native", current_function_);
            
            // If left is true, short-circuit to true; otherwise evaluate right
            builder_->CreateCondBr(leftBool, shortCircuitBlock, evalRightBlock);
            
            // Evaluate right operand
            builder_->SetInsertPoint(evalRightBlock);
            llvm::Value* rightBool = convertToBool(right);
            builder_->CreateBr(mergeBlock);
            
            // Short-circuit case (left was true)
            builder_->SetInsertPoint(shortCircuitBlock);
            llvm::Value* trueResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
            builder_->CreateBr(mergeBlock);
            
            // Merge results
            builder_->SetInsertPoint(mergeBlock);
            llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "or_result");
            resultPhi->addIncoming(rightBool, evalRightBlock);
            resultPhi->addIncoming(trueResult, shortCircuitBlock);
            current_value_ = resultPhi;
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for logical OR" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;

    case BinaryExpr::OP_CONCAT:
        if (leftIsBoxed && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_concat(BoxedValue*, BoxedValue*)" << std::endl;
            current_value_ = boxed_concat(left, right);
        } else if (leftIsNative && rightIsBoxed) {
            std::cout << "[LLVM CodeGen] Using boxed_concat_native_left(native, BoxedValue*)" << std::endl;
            current_value_ = boxed_concat_native_left(left, right);
        } else if (leftIsBoxed && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using boxed_concat_native_right(BoxedValue*, native)" << std::endl;
            current_value_ = boxed_concat_native_right(left, right);
        } else if (leftIsNative && rightIsNative) {
            std::cout << "[LLVM CodeGen] Using native concatenation (any types)" << std::endl;
            // Both native values, convert to strings first
            llvm::Value* leftStr = nativeValueToString(left);
            llvm::Value* rightStr = nativeValueToString(right);
            
            // Get string lengths to allocate proper buffer size
            llvm::Function* strlenFunc = module_->getFunction("strlen");
            if (!strlenFunc) {
                std::vector<llvm::Type*> strlenArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
                llvm::FunctionType* strlenType = llvm::FunctionType::get(llvm::Type::getInt64Ty(*context_), strlenArgs, false);
                strlenFunc = llvm::Function::Create(strlenType, llvm::Function::ExternalLinkage, "strlen", module_.get());
            }
            
            llvm::Value* leftLen = builder_->CreateCall(strlenFunc, {leftStr});
            llvm::Value* rightLen = builder_->CreateCall(strlenFunc, {rightStr});
            llvm::Value* totalLen = builder_->CreateAdd(leftLen, rightLen);
            llvm::Value* bufferSize = builder_->CreateAdd(totalLen, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1));
            
            // Allocate result buffer
            llvm::Value* resultBuffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), bufferSize, "concat_result");
            
        llvm::Function* strcpyFunc = module_->getFunction("strcpy");
        if (!strcpyFunc) {
            std::vector<llvm::Type*> strcpyArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
            llvm::FunctionType* strcpyType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strcpyArgs, false);
            strcpyFunc = llvm::Function::Create(strcpyType, llvm::Function::ExternalLinkage, "strcpy", module_.get());
        }
        llvm::Function* strcatFunc = module_->getFunction("strcat");
        if (!strcatFunc) {
            std::vector<llvm::Type*> strcatArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                          llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
            llvm::FunctionType* strcatType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strcatArgs, false);
            strcatFunc = llvm::Function::Create(strcatType, llvm::Function::ExternalLinkage, "strcat", module_.get());
        }
            
                    builder_->CreateCall(strcpyFunc, {resultBuffer, leftStr});
            builder_->CreateCall(strcatFunc, {resultBuffer, rightStr});
            
            // Create BoxedValue from the concatenated string
            llvm::Value* boxedResult = createBoxedFromString(resultBuffer);
            current_value_ = boxedResult;
        } else {
            std::cerr << "[LLVM CodeGen] Error: Unsupported operand types for concatenation" << std::endl;
            current_value_ = registerStringConstant("");
    }
        break;

    default:
        std::cerr << "[LLVM CodeGen] Error: Unsupported binary operation" << std::endl;
        current_value_ = left;
        break;
    }
}

void LLVMCodeGenerator::visit(CallExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing CallExpr: " << expr->callee << std::endl;

    // Special handling for print function
    if (expr->callee == "print" && expr->args.size() == 1)
    {
        handlePrintFunction(expr);
        return;
    }
    
    // Special handling for rand function - convert int to normalized double
    if (expr->callee == "rand" && expr->args.size() == 0)
    {
        handleRandFunction(expr);
        return;
    }

    // Get function
    llvm::Function* func = module_->getFunction(expr->callee);
    if (!func)
    {
        std::cerr << "Function " << expr->callee << " not found" << std::endl;
        current_value_ = nullptr;
        return;
    }

    // Prepare arguments with type conversion if needed
    std::vector<llvm::Value*> args;
    llvm::FunctionType* funcType = func->getFunctionType();
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
    
    std::cout << "[LLVM CodeGen] Function " << expr->callee << " expects " << funcType->getNumParams() << " parameters" << std::endl;
    
    for (size_t i = 0; i < expr->args.size() && i < funcType->getNumParams(); ++i)
    {
        expr->args[i]->accept(this);
        llvm::Value* argValue = current_value_;
        llvm::Type* expectedType = funcType->getParamType(i);
        llvm::Type* actualType = argValue->getType();
        
        std::cout << "[LLVM CodeGen] Parameter " << i << ":" << std::endl;
        std::cout << "  - Expected type: ";
        expectedType->print(llvm::outs());
        std::cout << std::endl;
        std::cout << "  - Actual type: ";
        actualType->print(llvm::outs());
        std::cout << std::endl;
        
        // Check if we need to convert the argument
        if (expectedType == actualType) {
            // Types match, no conversion needed
            std::cout << "  - No conversion needed" << std::endl;
            args.push_back(argValue);
        }
        else if (expectedType == boxedPtrTy && actualType != boxedPtrTy) {
            // Function expects BoxedValue* but we have a native type - box it
            std::cout << "  - Converting native type to BoxedValue*" << std::endl;
            llvm::Value* boxedArg = nullptr;
            
            if (actualType->isDoubleTy()) {
                boxedArg = createBoxedFromDouble(argValue);
            } else if (actualType->isIntegerTy(32)) {
                llvm::Value* doubleValue = builder_->CreateSIToFP(argValue, llvm::Type::getDoubleTy(*context_));
                boxedArg = createBoxedFromDouble(doubleValue);
            } else if (actualType->isIntegerTy(1)) {
                boxedArg = createBoxedFromBool(argValue);
            } else if (actualType->isPointerTy()) {
                boxedArg = createBoxedFromString(argValue);
            } else {
                std::cout << "  - Unknown type, converting to int and boxing" << std::endl;
                llvm::Value* intValue = builder_->CreateBitCast(argValue, llvm::Type::getInt32Ty(*context_));
                llvm::Value* doubleValue = builder_->CreateSIToFP(intValue, llvm::Type::getDoubleTy(*context_));
                boxedArg = createBoxedFromDouble(doubleValue);
            }
            
            args.push_back(boxedArg);
        }
        else if (expectedType != boxedPtrTy && actualType == boxedPtrTy) {
            // Function expects native type but we have BoxedValue* - unbox it
            std::cout << "  - Converting BoxedValue* to native type" << std::endl;
            llvm::Value* unboxedArg = nullptr;
            
            if (expectedType->isDoubleTy()) {
                unboxedArg = extractFromBoxed(argValue, 1); // tag 1 = double
            } else if (expectedType->isIntegerTy(32)) {
                llvm::Value* doubleValue = extractFromBoxed(argValue, 1); // tag 1 = double
                unboxedArg = builder_->CreateFPToSI(doubleValue, llvm::Type::getInt32Ty(*context_));
            } else if (expectedType->isIntegerTy(1)) {
                unboxedArg = extractFromBoxed(argValue, 0); // tag 0 = bool
            } else if (expectedType->isPointerTy()) {
                unboxedArg = extractFromBoxed(argValue, 2); // tag 2 = string
            } else {
                std::cout << "  - Cannot convert BoxedValue* to expected type, using as-is" << std::endl;
                unboxedArg = argValue;
            }
            
            args.push_back(unboxedArg);
        }
        else {
            // Types are different but neither is BoxedValue* - try basic conversions
            std::cout << "  - Attempting basic type conversion" << std::endl;
            
            if (expectedType->isDoubleTy() && actualType->isIntegerTy(32)) {
                args.push_back(builder_->CreateSIToFP(argValue, llvm::Type::getDoubleTy(*context_)));
            } else if (expectedType->isIntegerTy(32) && actualType->isDoubleTy()) {
                args.push_back(builder_->CreateFPToSI(argValue, llvm::Type::getInt32Ty(*context_)));
            } else {
                std::cout << "  - No suitable conversion found, using as-is" << std::endl;
                args.push_back(argValue);
            }
        }
    }
    
    // Handle case where we have more arguments than parameters (should not happen in well-formed code)
    for (size_t i = funcType->getNumParams(); i < expr->args.size(); ++i) {
        expr->args[i]->accept(this);
        args.push_back(current_value_);
    }

    current_value_ = builder_->CreateCall(func, args);
    
    std::cout << "[LLVM CodeGen] Function call " << expr->callee << " completed" << std::endl;
}

void LLVMCodeGenerator::visit(VariableExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing VariableExpr: " << expr->name << std::endl;

    // Look up variable in current scope
    LLVMScope* scope = current_scope_;
    while (scope)
    {
        auto it = scope->variables.find(expr->name);
        if (it != scope->variables.end())
        {
            llvm::AllocaInst* alloca = it->second;
            llvm::Type* varType = scope->variable_types[expr->name];
            
            // Load the value from the variable
            current_value_ = builder_->CreateLoad(varType, alloca);
            
            // Give the loaded value a meaningful name for debugging and track type
            if (auto inst = llvm::dyn_cast<llvm::Instruction>(current_value_)) {
                // Check if we have object type information
                auto objTypeIt = scope->variable_object_types.find(expr->name);
                if (objTypeIt != scope->variable_object_types.end()) {
                    std::string objType = objTypeIt->second;
                    inst->setName(objType + "_obj_" + expr->name);
                    trackValueType(current_value_, objType);
                    std::cout << "[LLVM CodeGen] Variable " << expr->name << " loaded as object of type " << objType << std::endl;
                } else {
                    inst->setName("var_" + expr->name);
                    // Check if we have tracked type info
                    std::string trackedType = getTrackedVariableType(expr->name);
                    if (!trackedType.empty()) {
                        trackValueType(current_value_, trackedType);
                        std::cout << "[LLVM CodeGen] Variable " << expr->name << " loaded with tracked type " << trackedType << std::endl;
                    } else {
                        std::cout << "[LLVM CodeGen] Variable " << expr->name << " loaded (no object type info)" << std::endl;
                    }
                }
            }
            
            std::cout << "[LLVM CodeGen] VariableExpr: Variable " << expr->name << " has type: ";
            varType->print(llvm::outs());
            std::cout << std::endl;
            
            // Check if this is a BoxedValue* by comparing with our known BoxedValue type
            llvm::StructType* boxedTy = getBoxedValueType();
            llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
            bool isBoxedValue = (varType == boxedPtrTy);
            
            // Alternative check: if it's a pointer to a struct named "BoxedValue"
            if (!isBoxedValue && varType->isPointerTy()) {
                if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(varType)) {
                    if (ptrType->getNumContainedTypes() > 0) {
                        llvm::Type* elementType = ptrType->getContainedType(0);
                        if (elementType->isStructTy()) {
                            std::string structName = elementType->getStructName().str();
                            isBoxedValue = (structName == "BoxedValue");
                        }
                    }
                }
            }
            
            if (isBoxedValue) {
                std::cout << "[LLVM CodeGen] VariableExpr: " << expr->name << " is BoxedValue*" << std::endl;
            } else {
                std::cout << "[LLVM CodeGen] VariableExpr: " << expr->name << " is native type" << std::endl;
            }
            
            return;
        }
        scope = scope->parent;
    }

    std::cerr << "Variable " << expr->name << " not found" << std::endl;
    current_value_ = nullptr;
}

void LLVMCodeGenerator::visit(LetExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing LetExpr: " << expr->name << std::endl;
    
    // Generate code for the initializer
    if (expr->initializer) {
        expr->initializer->accept(this);
    } else {
        // Default initialization with null
        current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    }
    
    llvm::Value* initValue = current_value_;
    
    // Create an alloca for the variable
    llvm::Type* varType = initValue->getType();
    llvm::AllocaInst* alloca = builder_->CreateAlloca(varType, nullptr, expr->name);
    
    // Store the initial value
    builder_->CreateStore(initValue, alloca);
    
    // Store variable information in current scope
    if (current_scope_) {
        current_scope_->variables[expr->name] = alloca;
        current_scope_->variable_types[expr->name] = varType;
        
        // Try to track the object type if this is an object
        if (varType->isPointerTy()) {
            std::string objectType = getObjectTypeName(initValue);
            if (!objectType.empty()) {
                current_scope_->variable_object_types[expr->name] = objectType;
                trackVariableType(expr->name, objectType);
                std::cout << "[LLVM CodeGen] Stored and tracked object type " << objectType << " for variable " << expr->name << std::endl;
            }
        }
    }
    
    std::cout << "[LLVM CodeGen] LetExpr: Declared variable " << expr->name << std::endl;
    
    // CRITICAL: Process the body of the let expression
    if (expr->body) {
        std::cout << "[LLVM CodeGen] Processing LetExpr body..." << std::endl;
        expr->body->accept(this);
        std::cout << "[LLVM CodeGen] LetExpr body processed successfully" << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] LetExpr has no body" << std::endl;
        // Set the current value to the alloca for potential chaining
        current_value_ = alloca;
    }
}

void LLVMCodeGenerator::visit(AssignExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing AssignExpr: " << expr->name << std::endl;

    // Evaluate value
    expr->value->accept(this);
    llvm::Value* value = current_value_;

    // Find variable in scope
    LLVMScope* scope = current_scope_;
    while (scope)
    {
        auto it = scope->variables.find(expr->name);
        if (it != scope->variables.end())
        {
            llvm::AllocaInst* alloca = it->second;
            llvm::Type* varType = scope->variable_types[expr->name];
            
            std::cout << "[LLVM CodeGen] AssignExpr: Variable " << expr->name << " has type: ";
            varType->print(llvm::outs());
            std::cout << std::endl;
            std::cout << "[LLVM CodeGen] AssignExpr: Value has type: ";
            value->getType()->print(llvm::outs());
            std::cout << std::endl;
            
            // Check if this is a BoxedValue* by comparing with our known BoxedValue type
            llvm::StructType* boxedTy = getBoxedValueType();
            llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
            bool isBoxedVariable = (varType == boxedPtrTy);
            
            // Alternative check: if it's a pointer to a struct named "BoxedValue"
            if (!isBoxedVariable && varType->isPointerTy()) {
            if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(varType)) {
                    if (ptrType->getNumContainedTypes() > 0) {
                        llvm::Type* elementType = ptrType->getContainedType(0);
                        if (elementType->isStructTy()) {
                            std::string structName = elementType->getStructName().str();
                            isBoxedVariable = (structName == "BoxedValue");
                        }
                    }
                }
            }
            
            // Check if the incoming value is already a BoxedValue*
            llvm::Type* valueType = value->getType();
            bool isBoxedValue = (valueType == boxedPtrTy);
            if (!isBoxedValue && valueType->isPointerTy()) {
                if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(valueType)) {
                if (ptrType->getNumContainedTypes() > 0) {
                    llvm::Type* elementType = ptrType->getContainedType(0);
                    if (elementType->isStructTy()) {
                        std::string structName = elementType->getStructName().str();
                        isBoxedValue = (structName == "BoxedValue");
                        }
                    }
                }
            }
            
            if (isBoxedVariable) {
                std::cout << "[LLVM CodeGen] AssignExpr: Variable " << expr->name << " is BoxedValue*" << std::endl;
            
                if (isBoxedValue) {
                    // Both variable and value are BoxedValue*, direct assignment
                    std::cout << "[LLVM CodeGen] AssignExpr: Direct BoxedValue* assignment" << std::endl;
                    builder_->CreateStore(value, alloca);
                    current_value_ = value;
                    
                    // Update object type information if we can determine it
                    std::string objectType = getObjectTypeName(value);
                    if (!objectType.empty()) {
                        scope->variable_object_types[expr->name] = objectType;
                        trackVariableType(expr->name, objectType);
                        std::cout << "[LLVM CodeGen] Updated and tracked object type for " << expr->name << " to " << objectType << std::endl;
                    }
                } else {
                    // Variable is BoxedValue*, but value is native type - need to box it
                    std::cout << "[LLVM CodeGen] AssignExpr: Boxing native value for BoxedValue* variable" << std::endl;
                    
                    llvm::Value* newBoxed = nullptr;
                    if (valueType->isIntegerTy(32)) {
                        // Convert int to double since all numbers are now double
                        llvm::Value* doubleValue = builder_->CreateSIToFP(value, llvm::Type::getDoubleTy(*context_));
                        newBoxed = createBoxedFromDouble(doubleValue);
                    } else if (valueType->isDoubleTy()) {
                        newBoxed = createBoxedFromDouble(value);
                    } else if (valueType->isIntegerTy(1)) {
                        newBoxed = createBoxedFromBool(value);
                    } else if (valueType->isPointerTy()) {
                        newBoxed = createBoxedFromString(value);
                    } else {
                        // Unknown type, try to convert to int and box
                        std::cout << "[LLVM CodeGen] AssignExpr: Unknown type, converting to int" << std::endl;
                        llvm::Value* intValue = builder_->CreateBitCast(value, llvm::Type::getInt32Ty(*context_));
                        // Convert int to double since all numbers are now double
                        llvm::Value* doubleValue = builder_->CreateSIToFP(intValue, llvm::Type::getDoubleTy(*context_));
                        newBoxed = createBoxedFromDouble(doubleValue);
                    }
                    
                    builder_->CreateStore(newBoxed, alloca);
                    current_value_ = newBoxed;
                    
                    // Clear object type information since we're boxing a native value
                    scope->variable_object_types.erase(expr->name);
                }
            } else {
                // Variable is native type
                if (isBoxedValue) {
                    std::cout << "[LLVM CodeGen] AssignExpr: Cannot assign BoxedValue* to native variable " << expr->name << std::endl;
                    std::cout << "[LLVM CodeGen] AssignExpr: Need to unbox or change variable type" << std::endl;
                    
                    // For destructive assignment, we change the variable type to BoxedValue*
                    std::cout << "[LLVM CodeGen] AssignExpr: Changing variable " << expr->name << " from native to BoxedValue*" << std::endl;
                    
                    // Create new alloca with BoxedValue* type
                    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
                    llvm::AllocaInst* newAlloca = entryBuilder.CreateAlloca(boxedPtrTy, nullptr, expr->name);
                    
                    // Store the BoxedValue
                    builder_->CreateStore(value, newAlloca);
                    
                    // Update the variable in the scope
                    scope->variables[expr->name] = newAlloca;
                    scope->variable_types[expr->name] = boxedPtrTy;
                    
                    // Try to determine object type
                    std::string objectType = getObjectTypeName(value);
                    if (!objectType.empty()) {
                        scope->variable_object_types[expr->name] = objectType;
                        trackVariableType(expr->name, objectType);
                        std::cout << "[LLVM CodeGen] Set and tracked object type for " << expr->name << " to " << objectType << std::endl;
                    }
                    
                    current_value_ = value;
                } else {
                    // Both are native types
                    if (valueType == varType) {
                        // Same types, direct assignment
                        std::cout << "[LLVM CodeGen] AssignExpr: Direct native assignment (same types)" << std::endl;
                        builder_->CreateStore(value, alloca);
                        current_value_ = value;
                        
                        // Clear object type information since we're assigning a native value
                        scope->variable_object_types.erase(expr->name);
                    } else {
                        // Different types, change variable type (destructive assignment)
                        std::cout << "[LLVM CodeGen] AssignExpr: Changing variable " << expr->name << " type from ";
                        varType->print(llvm::outs());
                        std::cout << " to ";
                        valueType->print(llvm::outs());
                        std::cout << std::endl;
                        
                        // Create new alloca with the new type
                        llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
                        llvm::AllocaInst* newAlloca = entryBuilder.CreateAlloca(valueType, nullptr, expr->name);
                        
                        // Store the value
                        builder_->CreateStore(value, newAlloca);
                        
                        // Update the variable in the scope
                        scope->variables[expr->name] = newAlloca;
                        scope->variable_types[expr->name] = valueType;
                        
                        // Try to determine if this is an object type
                        if (valueType->isPointerTy()) {
                            std::string objectType = getObjectTypeName(value);
                            if (!objectType.empty()) {
                                scope->variable_object_types[expr->name] = objectType;
                                trackVariableType(expr->name, objectType);
                                std::cout << "[LLVM CodeGen] Set and tracked object type for " << expr->name << " to " << objectType << std::endl;
                            }
                        } else {
                            // Clear object type information since we're assigning a native value
                            scope->variable_object_types.erase(expr->name);
                            variable_type_map_.erase(expr->name);
                        }
                        
                        current_value_ = value;
                    }
                }
            }
            
            return;
        }
        scope = scope->parent;
    }

    std::cerr << "Variable " << expr->name << " not found for assignment" << std::endl;
    current_value_ = nullptr;
}

void LLVMCodeGenerator::visit(IfExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing IfExpr" << std::endl;

    // 1. Evaluar la condición
    expr->condition->accept(this);
    llvm::Value* condition = current_value_;

    // Convertir la condición a booleano si no lo es
    if (!condition->getType()->isIntegerTy(1)) {
        condition = convertToBool(condition);
    }

    // 2. Crear los bloques básicos
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(*context_, "then", current_function_);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(*context_, "else", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge", current_function_);

    // 3. Branch condicional
    builder_->CreateCondBr(condition, thenBlock, elseBlock);

    // 4. Evaluar rama THEN
    builder_->SetInsertPoint(thenBlock);
    enterScope();
    expr->thenBranch->accept(this);
    llvm::Value* thenValue = current_value_;
    llvm::Type* thenType = thenValue->getType();
    exitScope();
    builder_->CreateBr(mergeBlock);

    // 5. Evaluar rama ELSE  
    builder_->SetInsertPoint(elseBlock);
    enterScope();
    expr->elseBranch->accept(this);
    llvm::Value* elseValue = current_value_;
    llvm::Type* elseType = elseValue->getType();
    exitScope();
    builder_->CreateBr(mergeBlock);

    // 6. Ir al bloque merge
    builder_->SetInsertPoint(mergeBlock);

    // 7. Analizar tipos para decidir estrategia
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = boxedTy->getPointerTo();

    // Detectar si los tipos son BoxedValue*
    bool thenIsBoxed = (thenType == boxedPtrTy);
    bool elseIsBoxed = (elseType == boxedPtrTy);
    
    // Detectar si los tipos son primitivos conocidos (i32, double, i1, i8*)
    auto isKnownPrimitive = [](llvm::Type* type) -> bool {
        return type->isIntegerTy(32) ||    // int
               type->isDoubleTy() ||       // double
               type->isIntegerTy(1) ||     // bool
               (type->isPointerTy() && type->getPointerAddressSpace() == 0); // string (i8* pointer)
    };
    
    bool thenIsKnownPrimitive = isKnownPrimitive(thenType);
    bool elseIsKnownPrimitive = isKnownPrimitive(elseType);
    
    // Un tipo es desconocido si es BoxedValue* o no es un primitivo conocido
    bool thenIsUnknown = thenIsBoxed || !thenIsKnownPrimitive;
    bool elseIsUnknown = elseIsBoxed || !elseIsKnownPrimitive;
    bool hasUnknownType = thenIsUnknown || elseIsUnknown;
    
    bool typesAreDifferent = (thenType != elseType);

    std::cout << "[LLVM CodeGen] IfExpr Analysis:" << std::endl;
    std::cout << "  - Then type: ";
    thenType->print(llvm::outs());
    std::cout << " (boxed: " << thenIsBoxed << ", known primitive: " << thenIsKnownPrimitive << ", unknown: " << thenIsUnknown << ")" << std::endl;
    std::cout << "  - Else type: ";
    elseType->print(llvm::outs());
    std::cout << " (boxed: " << elseIsBoxed << ", known primitive: " << elseIsKnownPrimitive << ", unknown: " << elseIsUnknown << ")" << std::endl;
    std::cout << "  - Types different: " << typesAreDifferent << std::endl;
    std::cout << "  - Has unknown type: " << hasUnknownType << std::endl;

    // 8. Aplicar la regla de decisión:
    // - Si los tipos no coinciden → BoxedValue*
    // - Si al menos uno es desconocido → BoxedValue*
    // - Solo si ambos son primitivos conocidos y del mismo tipo → tipo nativo
    if (typesAreDifferent || hasUnknownType) {
        // Caso 1: Usar BoxedValue (tipos diferentes o al menos uno desconocido)
        std::cout << "  - Decision: Using BoxedValue* (types differ or at least one unknown)" << std::endl;

        // Convertir ambos valores a BoxedValue* antes del PHI
        llvm::Value* thenBoxed = nullptr;
        llvm::Value* elseBoxed = nullptr;

        // Convertir then value a BoxedValue*
        builder_->SetInsertPoint(thenBlock->getTerminator());
        if (thenIsBoxed) {
            thenBoxed = thenValue;
        } else if (thenType->isIntegerTy(32)) {
            // Convert int to double since all numbers are now double
            llvm::Value* doubleValue = builder_->CreateSIToFP(thenValue, llvm::Type::getDoubleTy(*context_));
            thenBoxed = createBoxedFromDouble(doubleValue);
        } else if (thenType->isDoubleTy()) {
            thenBoxed = createBoxedFromDouble(thenValue);
        } else if (thenType->isIntegerTy(1)) {
            thenBoxed = createBoxedFromBool(thenValue);
        } else if (thenType->isPointerTy()) {
            thenBoxed = createBoxedFromString(thenValue);
        } else {
            // Tipo desconocido, intentar convertir a int y boxear
            std::cout << "[LLVM CodeGen] IfExpr: Unknown then type, converting to int and boxing" << std::endl;
            llvm::Value* intValue;
            if (thenType->isIntegerTy()) {
                // Extender o truncar entero a i32
                if (thenType->getIntegerBitWidth() < 32) {
                    intValue = builder_->CreateZExt(thenValue, llvm::Type::getInt32Ty(*context_));
                } else if (thenType->getIntegerBitWidth() > 32) {
                    intValue = builder_->CreateTrunc(thenValue, llvm::Type::getInt32Ty(*context_));
                } else {
                    intValue = thenValue;
                }
            } else if (thenType->isPointerTy()) {
                intValue = builder_->CreatePtrToInt(thenValue, llvm::Type::getInt32Ty(*context_));
            } else {
                // Último recurso: usar bitcast
                intValue = builder_->CreateBitCast(thenValue, llvm::Type::getInt32Ty(*context_));
            }
            // Convert int to double since all numbers are now double
            llvm::Value* doubleValue = builder_->CreateSIToFP(intValue, llvm::Type::getDoubleTy(*context_));
            thenBoxed = createBoxedFromDouble(doubleValue);
        }

        // Convertir else value a BoxedValue*
        builder_->SetInsertPoint(elseBlock->getTerminator());
        if (elseIsBoxed) {
            elseBoxed = elseValue;
        } else if (elseType->isIntegerTy(32)) {
            // Convert int to double since all numbers are now double
            llvm::Value* doubleValue = builder_->CreateSIToFP(elseValue, llvm::Type::getDoubleTy(*context_));
            elseBoxed = createBoxedFromDouble(doubleValue);
        } else if (elseType->isDoubleTy()) {
            elseBoxed = createBoxedFromDouble(elseValue);
        } else if (elseType->isIntegerTy(1)) {
            elseBoxed = createBoxedFromBool(elseValue);
        } else if (elseType->isPointerTy()) {
            elseBoxed = createBoxedFromString(elseValue);
        } else {
            // Tipo desconocido, intentar convertir a int y boxear
            std::cout << "[LLVM CodeGen] IfExpr: Unknown else type, converting to int and boxing" << std::endl;
            llvm::Value* intValue;
            if (elseType->isIntegerTy()) {
                // Extender o truncar entero a i32
                if (elseType->getIntegerBitWidth() < 32) {
                    intValue = builder_->CreateZExt(elseValue, llvm::Type::getInt32Ty(*context_));
                } else if (elseType->getIntegerBitWidth() > 32) {
                    intValue = builder_->CreateTrunc(elseValue, llvm::Type::getInt32Ty(*context_));
                } else {
                    intValue = elseValue;
                }
            } else if (elseType->isPointerTy()) {
                intValue = builder_->CreatePtrToInt(elseValue, llvm::Type::getInt32Ty(*context_));
            } else {
                // Último recurso: usar bitcast
                intValue = builder_->CreateBitCast(elseValue, llvm::Type::getInt32Ty(*context_));
            }
            // Convert int to double since all numbers are now double
            llvm::Value* doubleValue = builder_->CreateSIToFP(intValue, llvm::Type::getDoubleTy(*context_));
            elseBoxed = createBoxedFromDouble(doubleValue);
        }

        // Volver al merge block y crear PHI con BoxedValue*
    builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* phi = builder_->CreatePHI(boxedPtrTy, 2, "if_boxed_result");
        phi->addIncoming(thenBoxed, thenBlock);
        phi->addIncoming(elseBoxed, elseBlock);
        current_value_ = phi;

        std::cout << "  - Result: BoxedValue*" << std::endl;
    } else {
        // Caso 2: Ambos tipos son primitivos conocidos e iguales - usar el tipo nativo
        std::cout << "  - Decision: Using native type (both branches have same known primitive type)" << std::endl;

        llvm::PHINode* phi = builder_->CreatePHI(thenType, 2, "if_same_result");
        phi->addIncoming(thenValue, thenBlock);
        phi->addIncoming(elseValue, elseBlock);
        current_value_ = phi;
        
        std::cout << "  - Result: ";
        thenType->print(llvm::outs());
        std::cout << std::endl;
    }
}

void LLVMCodeGenerator::visit(ExprBlock *expr)
{
    std::cout << "[LLVM CodeGen] Processing ExprBlock" << std::endl;

    enterScope();

    llvm::Value* lastValue = nullptr;
    for (auto &stmt : expr->stmts)
    {
        stmt->accept(this);
        lastValue = current_value_;
    }

    current_value_ = lastValue;
    exitScope();
}

void LLVMCodeGenerator::visit(WhileExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing WhileExpr" << std::endl;

    // Create basic blocks
    llvm::BasicBlock* loopHeader = llvm::BasicBlock::Create(*context_, "while_header", current_function_);
    llvm::BasicBlock* loopBody = llvm::BasicBlock::Create(*context_, "while_body", current_function_);
    llvm::BasicBlock* loopExit = llvm::BasicBlock::Create(*context_, "while_exit", current_function_);

    // Branch to loop header
    builder_->CreateBr(loopHeader);

    // Loop header: evaluate condition
    builder_->SetInsertPoint(loopHeader);
    expr->condition->accept(this);
    llvm::Value* condition = current_value_;
    builder_->CreateCondBr(condition, loopBody, loopExit);

    // Loop body
    builder_->SetInsertPoint(loopBody);
    expr->body->accept(this);
    builder_->CreateBr(loopHeader);

    // Loop exit
    builder_->SetInsertPoint(loopExit);
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
}

void LLVMCodeGenerator::visit(ForExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing ForExpr" << std::endl;
    // Simplified implementation for now
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
}

void LLVMCodeGenerator::visit(NewExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing NewExpr: " << expr->typeName << std::endl;
    
    // Get struct type
    llvm::StructType* structType = getOrCreateStructType(expr->typeName);
    
    // Get constructor function
    std::string constructorName = expr->typeName + "_constructor";
    llvm::Function* constructor = module_->getFunction(constructorName);
    
    if (!constructor) {
        std::cerr << "[LLVM CodeGen] Error: Constructor function " << constructorName << " not found" << std::endl;
        
        // Fallback: simple memory allocation
        llvm::Value* size = llvm::ConstantExpr::getSizeOf(structType);
        llvm::Function* mallocFunc = module_->getFunction("malloc");
        if (!mallocFunc) {
            llvm::FunctionType* mallocType = llvm::FunctionType::get(
                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                {llvm::Type::getInt64Ty(*context_)}, 
                false
            );
            mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
        }
        
        llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {size});
        current_value_ = builder_->CreateBitCast(rawPtr, llvm::PointerType::get(structType, 0));
        
        // Store type information for later use
        if (current_value_) {
            if (auto inst = llvm::dyn_cast<llvm::Instruction>(current_value_)) {
                inst->setName(expr->typeName + "_obj");
            }
            // Track the type for LLVM 19 compatibility
            trackValueType(current_value_, expr->typeName);
            std::cout << "[LLVM CodeGen] Tracked fallback NewExpr result type: " << expr->typeName << std::endl;
        }
        
        return;
    }
    
    // Prepare constructor arguments
    std::vector<llvm::Value*> args;
    
    // Process constructor arguments - all must be BoxedValue*
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    
    for (auto& arg : expr->args) {
        arg->accept(this);
        llvm::Value* argValue = current_value_;
        
        // Convert argument to BoxedValue* if needed (all constructor params are BoxedValue*)
        llvm::Value* boxedArg = argValue;
        if (argValue->getType() != boxedPtrTy) {
            llvm::Type* valueType = argValue->getType();
            if (valueType->isDoubleTy()) {
                boxedArg = createBoxedFromDouble(argValue);
            } else if (valueType->isIntegerTy(1)) {
                boxedArg = createBoxedFromBool(argValue);
            } else if (valueType->isPointerTy() && !valueType->getPointerAddressSpace()) {
                // Assume it's a string (i8*)
                boxedArg = createBoxedFromString(argValue);
            } else {
                std::cerr << "[LLVM CodeGen] Warning: Unknown argument type for constructor, using as-is" << std::endl;
                boxedArg = builder_->CreateBitCast(argValue, boxedPtrTy);
            }
        }
        
        args.push_back(boxedArg);
    }
    
    // Call constructor
    current_value_ = builder_->CreateCall(constructor, args);
    
    // Set instruction name to help with type identification
    if (current_value_) {
        if (auto inst = llvm::dyn_cast<llvm::Instruction>(current_value_)) {
            inst->setName(expr->typeName + "_obj");
        }
        // Track the type for LLVM 19 compatibility
        trackValueType(current_value_, expr->typeName);
        std::cout << "[LLVM CodeGen] Tracked NewExpr result type: " << expr->typeName << std::endl;
    }
    
    std::cout << "[LLVM CodeGen] NewExpr: Called constructor " << constructorName << " with " << args.size() << " arguments" << std::endl;
}

void LLVMCodeGenerator::visit(GetAttrExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing GetAttrExpr: " << expr->attrName << std::endl;
    
    // Get the object
    expr->object->accept(this);
    llvm::Value* objectPtr = current_value_;
    
    if (!objectPtr) {
        std::cerr << "[LLVM CodeGen] Error: Object pointer is null in GetAttrExpr" << std::endl;
        current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
        return;
    }
    
    // Get the object type name
    std::string objectTypeName = getObjectTypeName(objectPtr);
    if (objectTypeName.empty()) {
        std::cerr << "[LLVM CodeGen] Error: Could not determine object type in GetAttrExpr" << std::endl;
        current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
        return;
    }
    
    // Get the struct type from our types map
    auto typeIt = types_.find(objectTypeName);
    if (typeIt == types_.end()) {
        std::cerr << "[LLVM CodeGen] Error: Type " << objectTypeName << " not found in types map" << std::endl;
        current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
        return;
    }
    
    llvm::StructType* structType = typeIt->second;
    
    // Find the attribute index using our metadata
    int attributeIndex = getAttributeIndex(structType, expr->attrName);
    
    if (attributeIndex < 0) {
        std::cerr << "[LLVM CodeGen] Error: Attribute " << expr->attrName << " not found in type " << objectTypeName << std::endl;
        current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
        return;
    }
    
    // Get pointer to the attribute
    llvm::Value* attrPtr = builder_->CreateStructGEP(structType, objectPtr, attributeIndex);
    
    // Load the attribute value - all attributes are BoxedValue*
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    current_value_ = builder_->CreateLoad(boxedPtrTy, attrPtr);
    
    std::cout << "[LLVM CodeGen] GetAttrExpr: Loaded attribute " << expr->attrName << " at index " << attributeIndex << " from type " << objectTypeName << std::endl;
}

void LLVMCodeGenerator::visit(SetAttrExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing SetAttrExpr: " << expr->attrName << std::endl;
    
    // Get the object
    expr->object->accept(this);
    llvm::Value* objectPtr = current_value_;
    
    if (!objectPtr) {
        std::cerr << "[LLVM CodeGen] Error: Object pointer is null in SetAttrExpr" << std::endl;
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return;
    }
    
    // Get the new value
    expr->value->accept(this);
    llvm::Value* newValue = current_value_;
    
    if (!newValue) {
        std::cerr << "[LLVM CodeGen] Error: New value is null in SetAttrExpr" << std::endl;
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return;
    }
    
    // Get the object type name
    std::string objectTypeName = getObjectTypeName(objectPtr);
    if (objectTypeName.empty()) {
        std::cerr << "[LLVM CodeGen] Error: Could not determine object type in SetAttrExpr" << std::endl;
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return;
    }
    
    // Get the struct type from our types map
    auto typeIt = types_.find(objectTypeName);
    if (typeIt == types_.end()) {
        std::cerr << "[LLVM CodeGen] Error: Type " << objectTypeName << " not found in types map" << std::endl;
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return;
    }
    
    llvm::StructType* structType = typeIt->second;
    
    // Find the attribute index using our metadata
    int attributeIndex = getAttributeIndex(structType, expr->attrName);
    
    if (attributeIndex < 0) {
        std::cerr << "[LLVM CodeGen] Error: Attribute " << expr->attrName << " not found in type " << objectTypeName << std::endl;
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return;
    }
    
    // Get pointer to the attribute
    llvm::Value* attrPtr = builder_->CreateStructGEP(structType, objectPtr, attributeIndex);
    
    // Convert new value to BoxedValue* if needed (all attributes are BoxedValue*)
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    llvm::Value* boxedValue = newValue;
    
    if (newValue->getType() != boxedPtrTy) {
        // Box the value based on its type
        llvm::Type* valueType = newValue->getType();
        if (valueType->isDoubleTy()) {
            boxedValue = createBoxedFromDouble(newValue);
        } else if (valueType->isIntegerTy(1)) {
            boxedValue = createBoxedFromBool(newValue);
        } else if (valueType->isPointerTy() && !valueType->getPointerAddressSpace()) {
            // Assume it's a string (i8*)
            boxedValue = createBoxedFromString(newValue);
        } else {
            std::cerr << "[LLVM CodeGen] Warning: Unknown type for attribute assignment, using as-is" << std::endl;
            boxedValue = builder_->CreateBitCast(newValue, boxedPtrTy);
        }
    }
    
    // Store the boxed value
    builder_->CreateStore(boxedValue, attrPtr);
    
    // Return the new value (assignment returns the assigned value)
    current_value_ = newValue;
    
    std::cout << "[LLVM CodeGen] SetAttrExpr: Set attribute " << expr->attrName << " at index " << attributeIndex << " in type " << objectTypeName << std::endl;
}

void LLVMCodeGenerator::visit(MethodCallExpr *expr)
{
    std::cout << "[LLVM CodeGen] ========================================" << std::endl;
    std::cout << "[LLVM CodeGen] Processing MethodCallExpr: " << expr->methodName << std::endl;
    std::cout << "[LLVM CodeGen] ========================================" << std::endl;
    
    // Get the object
    std::cout << "[LLVM CodeGen] Evaluating object expression..." << std::endl;
    expr->object->accept(this);
    llvm::Value* objectPtr = current_value_;
    
    if (!objectPtr) {
        std::cerr << "[LLVM CodeGen] Error: Object pointer is null in MethodCallExpr" << std::endl;
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return;
    }
    
    std::cout << "[LLVM CodeGen] Object ptr obtained: " << objectPtr << std::endl;
    std::cout << "[LLVM CodeGen] Object type: ";
    objectPtr->getType()->print(llvm::outs());
    std::cout << std::endl;
    
    // Debug: print object instruction details
    if (auto inst = llvm::dyn_cast<llvm::Instruction>(objectPtr)) {
        std::cout << "[LLVM CodeGen] Object is instruction: " << inst->getOpcodeName() << std::endl;
        std::cout << "[LLVM CodeGen] Object instruction name: " << inst->getName().str() << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] Object is not an instruction" << std::endl;
    }
    
    // Try to determine object type through various means
    std::cout << "[LLVM CodeGen] Attempting to determine object type..." << std::endl;
    std::string objectTypeName = getObjectTypeName(objectPtr);
    
    std::cout << "[LLVM CodeGen] getObjectTypeName returned: '" << objectTypeName << "'" << std::endl;
    
    // If getObjectTypeName failed, try to use the semantic type information
    if (objectTypeName.empty()) {
        std::cout << "[LLVM CodeGen] getObjectTypeName failed, trying semantic type information..." << std::endl;
        
        // Check if the object expression has type information from semantic analysis
        if (expr->object && expr->object->inferredType && expr->object->inferredType->isObject()) {
            objectTypeName = expr->object->inferredType->getTypeName();
            std::cout << "[LLVM CodeGen] Found semantic type: '" << objectTypeName << "'" << std::endl;
            
            // Track this value type for future reference
            trackValueType(objectPtr, objectTypeName);
        }
    }
    
    // If we can't determine the type statically, we need to add runtime type information
    if (objectTypeName.empty()) {
        std::cerr << "[LLVM CodeGen] Warning: Cannot determine object type for method call to " << expr->methodName << std::endl;
        
        // List all known types for debugging
        std::cout << "[LLVM CodeGen] Known types in module:" << std::endl;
        for (const auto& typePair : types_) {
            std::cout << "[LLVM CodeGen]   - " << typePair.first << std::endl;
        }
        
        // Try to use a default type (this is a fallback approach)
        objectTypeName = "Object"; // Default fallback
        std::cout << "[LLVM CodeGen] Using fallback type: " << objectTypeName << std::endl;
    }
    
    std::cout << "[LLVM CodeGen] Final object type: " << objectTypeName << std::endl;
    
    // Check if we need dynamic dispatch (for polymorphic calls)
    bool needsDynamicDispatch = false;
    
    // CRITICAL: Always use dynamic dispatch when object type is not statically known
    // This happens when the object comes from a function that can return multiple types
    if (!objectTypeName.empty()) {
        // Check if this type or any derived types exist (indicating potential polymorphism)
        bool hasPolymorphism = false;
        
        // Check if there are any types that inherit from this objectTypeName
        for (const auto& typePair : type_declarations_) {
            if (typePair.second && typePair.second->baseType == objectTypeName) {
                hasPolymorphism = true;
                std::cout << "[LLVM CodeGen] Found derived type " << typePair.first << " from " << objectTypeName << std::endl;
                break;
            }
        }
        
        // Also check if this type itself has inheritance
        auto typeIt = type_declarations_.find(objectTypeName);
        if (typeIt != type_declarations_.end() && typeIt->second->baseType != "Object") {
            hasPolymorphism = true;
            std::cout << "[LLVM CodeGen] Type " << objectTypeName << " has base type " << typeIt->second->baseType << std::endl;
        }
        
        // FORCE dynamic dispatch for any polymorphic hierarchy
        if (hasPolymorphism) {
            needsDynamicDispatch = true;
            std::cout << "[LLVM CodeGen] FORCING dynamic dispatch for polymorphic type hierarchy involving " << objectTypeName << std::endl;
        }
    }
    
    if (needsDynamicDispatch && !objectTypeName.empty()) {
        std::cout << "[LLVM CodeGen] Using dynamic dispatch via vtable for method: " << expr->methodName << std::endl;
        
        // Find the base type that defines this method to get consistent vtable indices
        std::string baseTypeForMethod = objectTypeName;
        
        // Find which type in the hierarchy first defines this method
        std::string current = objectTypeName;
        while (current != "Object") {
            auto methodsIt = type_methods_.find(current);
            if (methodsIt != type_methods_.end()) {
                auto& methods = methodsIt->second;
                bool hasMethod = false;
                for (const auto& method : methods) {
                    if (method == expr->methodName) {
                        hasMethod = true;
                        break;
                    }
                }
                if (hasMethod) {
                    baseTypeForMethod = current;
                }
            }
            
            // Move to base type
            auto typeIt = type_declarations_.find(current);
            if (typeIt != type_declarations_.end()) {
                current = typeIt->second->baseType;
            } else {
                break;
            }
        }
        
        std::cout << "[LLVM CodeGen] Using base type " << baseTypeForMethod << " for vtable index lookup" << std::endl;
        
        // Get method index in vtable using the base type
        auto methodsIt = type_methods_.find(baseTypeForMethod);
        if (methodsIt != type_methods_.end()) {
            auto& methods = methodsIt->second;
            int methodIndex = -1;
            for (size_t i = 0; i < methods.size(); i++) {
                if (methods[i] == expr->methodName) {
                    methodIndex = static_cast<int>(i);
                    break;
                }
            }
            
            if (methodIndex >= 0) {
                std::cout << "[LLVM CodeGen] Found method " << expr->methodName << " at vtable index " << methodIndex << std::endl;
                
                // Get the struct type for this object
                auto typeIt = types_.find(objectTypeName);
                if (typeIt == types_.end()) {
                    std::cout << "[LLVM CodeGen] Error: Could not find struct type for " << objectTypeName << std::endl;
                    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
                    return;
                }
                llvm::StructType* structType = typeIt->second;
                
                // Load vtable from object (first field)
                llvm::Value* vtablePtr = builder_->CreateStructGEP(structType, objectPtr, 0, "vtable_ptr");
                llvm::Value* vtable = builder_->CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), vtablePtr, "vtable");
                
                // Get the vtable global to know its structure
                auto vtableIt = vtables_.find(objectTypeName);
                if (vtableIt == vtables_.end()) {
                    std::cout << "[LLVM CodeGen] Error: Could not find vtable for " << objectTypeName << std::endl;
                    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
                    return;
                }
                llvm::GlobalVariable* vtableGlobal = vtableIt->second;
                llvm::Type* vtableType = vtableGlobal->getValueType();
                
                // Cast vtable pointer to correct type
                llvm::Value* typedVtable = builder_->CreateBitCast(vtable, llvm::PointerType::get(vtableType, 0));
                
                // Get function pointer from vtable array
                std::vector<llvm::Value*> indices = {
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0),        // array base
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), methodIndex) // method index
                };
                llvm::Value* funcPtrPtr = builder_->CreateGEP(vtableType, typedVtable, indices, "method_ptr_ptr");
                llvm::Value* funcPtr = builder_->CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), funcPtrPtr, "method_ptr");
                
                // Create function type for the method (BoxedValue* methodName(ObjectType* self))
                std::vector<llvm::Type*> paramTypes = {llvm::PointerType::get(structType, 0)}; // typed self pointer
                llvm::FunctionType* methodType = llvm::FunctionType::get(
                    llvm::PointerType::get(getBoxedValueType(), 0), // return BoxedValue*
                    paramTypes, false
                );
                
                // Cast function pointer to correct type
                llvm::Value* typedFuncPtr = builder_->CreateBitCast(funcPtr, llvm::PointerType::get(methodType, 0));
                
                // Prepare arguments
                std::vector<llvm::Value*> args = {objectPtr};
                
                // Make dynamic call
                current_value_ = builder_->CreateCall(methodType, typedFuncPtr, args, "dynamic_call_result");
                
                std::cout << "[LLVM CodeGen] Dynamic method call completed successfully" << std::endl;
                return;
            }
        }
        
        std::cout << "[LLVM CodeGen] Failed to find method in vtable, falling back to static dispatch" << std::endl;
    }
    
    // Fallback to static dispatch
    std::string methodFunctionName = objectTypeName + "_" + expr->methodName;
    std::cout << "[LLVM CodeGen] Using static dispatch for method function: " << methodFunctionName << std::endl;
    
    llvm::Function* methodFunc = module_->getFunction(methodFunctionName);
    
    if (!methodFunc) {
        std::cerr << "[LLVM CodeGen] Error: Method function " << methodFunctionName << " not found" << std::endl;
        
        // List all functions in the module for debugging
        std::cout << "[LLVM CodeGen] Available functions in module:" << std::endl;
        for (auto& func : *module_) {
            std::cout << "[LLVM CodeGen]   - " << func.getName().str() << std::endl;
        }
        
        // Try to find the method in base classes
        if (objectTypeName != "Object") {
            std::cout << "[LLVM CodeGen] Searching in base classes..." << std::endl;
            auto typeIt = type_declarations_.find(objectTypeName);
            if (typeIt != type_declarations_.end()) {
                std::string baseType = typeIt->second->baseType;
                std::cout << "[LLVM CodeGen] Base type: " << baseType << std::endl;
                
                while (baseType != "Object") {
                    std::string baseMethodName = baseType + "_" + expr->methodName;
                    std::cout << "[LLVM CodeGen] Trying base method: " << baseMethodName << std::endl;
                    methodFunc = module_->getFunction(baseMethodName);
                    if (methodFunc) {
                        methodFunctionName = baseMethodName;
                        std::cout << "[LLVM CodeGen] Found method in base class: " << baseMethodName << std::endl;
                        break;
                    }
                    
                    auto baseIt = type_declarations_.find(baseType);
                    if (baseIt != type_declarations_.end()) {
                        baseType = baseIt->second->baseType;
                        std::cout << "[LLVM CodeGen] Moving to next base: " << baseType << std::endl;
                    } else {
                        std::cout << "[LLVM CodeGen] Base type " << baseType << " not found in declarations" << std::endl;
                        break;
                    }
                }
            } else {
                std::cout << "[LLVM CodeGen] Type " << objectTypeName << " not found in type_declarations_" << std::endl;
            }
        }
        
        if (!methodFunc) {
            std::cerr << "[LLVM CodeGen] Error: Method " << expr->methodName << " not found in type hierarchy" << std::endl;
            std::cout << "[LLVM CodeGen] Available type declarations:" << std::endl;
            for (const auto& typePair : type_declarations_) {
                std::cout << "[LLVM CodeGen]   - Type: " << typePair.first << std::endl;
                if (typePair.second) {
                    std::cout << "[LLVM CodeGen]     Methods:" << std::endl;
                    for (const auto& method : typePair.second->methods) {
                        std::cout << "[LLVM CodeGen]       - " << method->name << std::endl;
                    }
                }
            }
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
            return;
        }
    }
    
    std::cout << "[LLVM CodeGen] Using method function: " << methodFunctionName << std::endl;
    
    // Prepare arguments for method call
    std::vector<llvm::Value*> args;
    args.push_back(objectPtr); // self parameter
    std::cout << "[LLVM CodeGen] Added self parameter" << std::endl;
    
    // Process method arguments
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    
    std::cout << "[LLVM CodeGen] Processing " << expr->args.size() << " method arguments..." << std::endl;
    
    for (size_t i = 0; i < expr->args.size(); i++) {
        std::cout << "[LLVM CodeGen] Processing argument " << i << "..." << std::endl;
        expr->args[i]->accept(this);
        llvm::Value* argValue = current_value_;
        
        if (!argValue) {
            std::cerr << "[LLVM CodeGen] Error: Argument " << i << " is null" << std::endl;
            continue;
        }
        
        std::cout << "[LLVM CodeGen] Argument " << i << " type: ";
        argValue->getType()->print(llvm::outs());
        std::cout << std::endl;
        
        // Check the expected parameter type of the method
        llvm::FunctionType* funcType = methodFunc->getFunctionType();
        llvm::Type* expectedType = nullptr;
        
        if (i + 1 < funcType->getNumParams()) { // +1 because first param is self
            expectedType = funcType->getParamType(i + 1);
            std::cout << "[LLVM CodeGen] Expected argument " << i << " type: ";
            expectedType->print(llvm::outs());
            std::cout << std::endl;
        } else {
            // Default to BoxedValue* for variadic or unknown parameters
            expectedType = boxedPtrTy;
            std::cout << "[LLVM CodeGen] Using default BoxedValue* for argument " << i << std::endl;
        }
        
        // Convert argument to expected type if necessary
        llvm::Value* finalArg = argValue;
        if (argValue->getType() != expectedType) {
            std::cout << "[LLVM CodeGen] Converting argument " << i << " from actual to expected type" << std::endl;
            if (expectedType == boxedPtrTy) {
                // Convert to BoxedValue*
                llvm::Type* valueType = argValue->getType();
                if (valueType->isDoubleTy()) {
                    finalArg = createBoxedFromDouble(argValue);
                } else if (valueType->isIntegerTy(1)) {
                    finalArg = createBoxedFromBool(argValue);
                } else if (valueType->isPointerTy()) {
                    // Check if it's already a BoxedValue*
                    if (valueType == boxedPtrTy) {
                        finalArg = argValue;
                    } else {
                        // Assume it's a string
                        finalArg = createBoxedFromString(argValue);
                    }
                } else {
                    std::cerr << "[LLVM CodeGen] Warning: Cannot convert argument type to BoxedValue*" << std::endl;
                    finalArg = builder_->CreateBitCast(argValue, expectedType);
                }
            } else {
                // Convert from BoxedValue* to expected type
                if (argValue->getType() == boxedPtrTy) {
                    if (expectedType->isDoubleTy()) {
                        finalArg = unboxDouble(argValue);
                    } else if (expectedType->isIntegerTy(1)) {
                        finalArg = unboxBool(argValue);
                    } else if (expectedType->isPointerTy()) {
                        finalArg = unboxString(argValue);
                    } else {
                        finalArg = builder_->CreateBitCast(argValue, expectedType);
                    }
                } else {
                    // Direct type conversion
                    finalArg = builder_->CreateBitCast(argValue, expectedType);
                }
            }
        } else {
            std::cout << "[LLVM CodeGen] Argument " << i << " types match, no conversion needed" << std::endl;
        }
        
        args.push_back(finalArg);
        std::cout << "[LLVM CodeGen] Added argument " << i << " to method call" << std::endl;
    }
    
    // Make the method call
    std::cout << "[LLVM CodeGen] Making method call with " << args.size() << " arguments..." << std::endl;
    current_value_ = builder_->CreateCall(methodFunc, args);
    
    if (current_value_) {
        std::cout << "[LLVM CodeGen] Method call successful, result type: ";
        current_value_->getType()->print(llvm::outs());
        std::cout << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] Method call returned null" << std::endl;
    }
    
    std::cout << "[LLVM CodeGen] ========================================" << std::endl;
    std::cout << "[LLVM CodeGen] Method call to " << methodFunctionName << " completed" << std::endl;
    std::cout << "[LLVM CodeGen] ========================================" << std::endl;
}

void LLVMCodeGenerator::visit(SelfExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing SelfExpr" << std::endl;
    
    // Look up 'self' in the current scope
    LLVMScope* scope = current_scope_;
    while (scope) {
        auto it = scope->variables.find("self");
        if (it != scope->variables.end()) {
            llvm::AllocaInst* selfAlloca = it->second;
            llvm::Type* selfType = scope->variable_types["self"];
            
            // Load the self pointer
            current_value_ = builder_->CreateLoad(selfType, selfAlloca);
            
            std::cout << "[LLVM CodeGen] SelfExpr: Found self variable in scope" << std::endl;
            return;
        }
        scope = scope->parent;
    }
    
    // If we reach here, 'self' was not found in any scope
    std::cerr << "[LLVM CodeGen] Error: 'self' not found in current scope" << std::endl;
    current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
}

void LLVMCodeGenerator::visit(BaseCallExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing BaseCallExpr" << std::endl;
    // Simplified implementation
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
}

void LLVMCodeGenerator::visit(IsExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing IsExpr" << std::endl;
    // Simplified implementation
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
}

void LLVMCodeGenerator::visit(AsExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing AsExpr" << std::endl;
    expr->expr->accept(this);
    // Return the expression value as is
}