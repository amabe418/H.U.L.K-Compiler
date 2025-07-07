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

    // Procesar cada statement
    for (auto &s : stmt->stmts) {
        s->accept(this);
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
        else
        {
            builder_->CreateRet(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(returnType)));
        }
    }

    current_function_ = nullptr;
    current_block_ = nullptr;
}

void LLVMCodeGenerator::visit(TypeDecl *stmt)
{
    std::cout << "[LLVM CodeGen] Processing TypeDecl: " << stmt->name << std::endl;

    current_type_ = stmt->name;

    // Create struct type
    std::vector<llvm::Type*> fieldTypes;
    
    // Add base type pointer if inheriting
    if (stmt->baseType != "Object")
    {
        llvm::StructType* baseType = getOrCreateStructType(stmt->baseType);
        fieldTypes.push_back(llvm::PointerType::get(baseType, 0));
    }

    // Add own attributes (all as BoxedValue pointers for now)
    for (size_t i = 0; i < stmt->attributes.size(); ++i)
    {
        // For now, all attributes are BoxedValue pointers
        // In a full implementation, we'd use the actual types
        fieldTypes.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    }

    // Create the struct type
    llvm::StructType* structType = llvm::StructType::create(*context_, fieldTypes, "struct." + stmt->name);
    types_[stmt->name] = structType;

    // Generate constructor
    generateConstructorFunction(stmt);

    // Process methods
    for (const auto &method : stmt->methods)
    {
        method->accept(this);
    }

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
    
    // Create native types for numbers
    bool isInteger = (expr->value == std::floor(expr->value));
    if (isInteger) {
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), static_cast<int64_t>(expr->value));
        std::cout << "[LLVM CodeGen] NumberExpr created as i32: " << expr->value << std::endl;
    } else {
        current_value_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), expr->value);
        std::cout << "[LLVM CodeGen] NumberExpr created as double: " << expr->value << std::endl;
    }
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
            std::cout << "[LLVM CodeGen] Using native addition" << std::endl;
            // Ambos nativos: promoción de tipos tradicional
    bool leftIsDouble = left->getType()->isDoubleTy();
    bool rightIsDouble = right->getType()->isDoubleTy();
            bool needPromotion = leftIsDouble || rightIsDouble;

    if (needPromotion) {
        if (!leftIsDouble && left->getType()->isIntegerTy(32)) {
            left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
        }
        if (!rightIsDouble && right->getType()->isIntegerTy(32)) {
            right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
        }
                current_value_ = builder_->CreateFAdd(left, right);
            } else {
                current_value_ = builder_->CreateAdd(left, right);
            }
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
            std::cout << "[LLVM CodeGen] Using native subtraction" << std::endl;
            bool leftIsDouble = left->getType()->isDoubleTy();
            bool rightIsDouble = right->getType()->isDoubleTy();
            bool needPromotion = leftIsDouble || rightIsDouble;
            
            if (needPromotion) {
                if (!leftIsDouble && left->getType()->isIntegerTy(32)) {
                    left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
                }
                if (!rightIsDouble && right->getType()->isIntegerTy(32)) {
                    right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
                }
                current_value_ = builder_->CreateFSub(left, right);
            } else {
                current_value_ = builder_->CreateSub(left, right);
            }
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
            std::cout << "[LLVM CodeGen] Using native multiplication" << std::endl;
            bool leftIsDouble = left->getType()->isDoubleTy();
            bool rightIsDouble = right->getType()->isDoubleTy();
            bool needPromotion = leftIsDouble || rightIsDouble;
            
            if (needPromotion) {
                if (!leftIsDouble && left->getType()->isIntegerTy(32)) {
                    left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
                }
                if (!rightIsDouble && right->getType()->isIntegerTy(32)) {
                    right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
                }
                current_value_ = builder_->CreateFMul(left, right);
            } else {
                current_value_ = builder_->CreateMul(left, right);
            }
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
            std::cout << "[LLVM CodeGen] Using native modulo" << std::endl;
            // Native modulo depends on type
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                // Integer modulo
                current_value_ = builder_->CreateSRem(left, right);
            } else {
                // Floating point modulo using fmod
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
            }
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
            std::cout << "[LLVM CodeGen] Using native greater than" << std::endl;
            // Both native, use traditional comparison
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                current_value_ = builder_->CreateICmpSGT(left, right);
            } else {
                // Convert to double if needed for mixed int/double comparison
                if (!left->getType()->isDoubleTy()) {
                    left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
                }
                if (!right->getType()->isDoubleTy()) {
                    right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
                }
                current_value_ = builder_->CreateFCmpOGT(left, right);
            }
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
            std::cout << "[LLVM CodeGen] Using native less than" << std::endl;
            // Both native, use traditional comparison
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                current_value_ = builder_->CreateICmpSLT(left, right);
            } else {
                // Convert to double if needed for mixed int/double comparison
                if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
                if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
                current_value_ = builder_->CreateFCmpOLT(left, right);
            }
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
            std::cout << "[LLVM CodeGen] Using native greater than or equal" << std::endl;
            // Both native, use traditional comparison
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                current_value_ = builder_->CreateICmpSGE(left, right);
            } else {
                // Convert to double if needed for mixed int/double comparison
                if (!left->getType()->isDoubleTy()) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
                if (!right->getType()->isDoubleTy()) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
                current_value_ = builder_->CreateFCmpOGE(left, right);
            }
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
            std::cout << "[LLVM CodeGen] Using native less than or equal" << std::endl;
            // Both native, use traditional comparison
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                current_value_ = builder_->CreateICmpSLE(left, right);
            } else {
                // Convert to double if needed for mixed int/double comparison
                if (!left->getType()->isDoubleTy()) {
                    left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
                }
                if (!right->getType()->isDoubleTy()) {
                    right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
                }
                current_value_ = builder_->CreateFCmpOLE(left, right);
            }
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
            // Both native, use traditional comparison
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                current_value_ = builder_->CreateICmpEQ(left, right);
            } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
                current_value_ = builder_->CreateFCmpOEQ(left, right);
            } else if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) {
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
            // Both native, use traditional comparison
            if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(32)) {
                current_value_ = builder_->CreateICmpNE(left, right);
            } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
                current_value_ = builder_->CreateFCmpONE(left, right);
            } else if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) {
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

    // Get function
    llvm::Function* func = module_->getFunction(expr->callee);
    if (!func)
    {
        std::cerr << "Function " << expr->callee << " not found" << std::endl;
        current_value_ = nullptr;
        return;
    }

    // Prepare arguments
    std::vector<llvm::Value*> args;
    for (auto &arg : expr->args)
    {
        arg->accept(this);
        args.push_back(current_value_);
    }

    current_value_ = builder_->CreateCall(func, args);
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
            
            // Cargar el valor de la variable
            current_value_ = builder_->CreateLoad(varType, alloca);
            
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
                // El valor ya es el puntero al BoxedValue, no necesitamos hacer nada más
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

    // Entrar a un nuevo scope antes de evaluar el inicializador (permite shadowing)
    enterScope();

    // Evaluar el inicializador
    expr->initializer->accept(this);
    llvm::Value* initValue = current_value_;

    // Crear el alloca en el entry block de la función actual (mejor práctica LLVM)
    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
    llvm::Type* varType = initValue->getType();
    
    std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " will be created with type: ";
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
        std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " is initialized with BoxedValue* - storing as BoxedValue*" << std::endl;
        
        // Para BoxedValue*, crear un alloca que almacene el puntero al BoxedValue
        llvm::AllocaInst* alloca = entryBuilder.CreateAlloca(boxedPtrTy, nullptr, expr->name);
        builder_->CreateStore(initValue, alloca);
        current_scope_->variables[expr->name] = alloca;
        current_scope_->variable_types[expr->name] = boxedPtrTy;
        
        std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " stored as BoxedValue*" << std::endl;
    } else {
        // Para otros tipos, crear alloca del tipo específico
        std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " is native type - storing as native type" << std::endl;
        
    llvm::AllocaInst* alloca = entryBuilder.CreateAlloca(varType, nullptr, expr->name);
    builder_->CreateStore(initValue, alloca);
    current_scope_->variables[expr->name] = alloca;
    current_scope_->variable_types[expr->name] = varType;
        
        std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " stored as native type" << std::endl;
    }

    // Evaluar el cuerpo; el valor de la expresión let es el valor de la última expresión del cuerpo
    expr->body->accept(this);

    // Salir del scope
    exitScope();
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
                } else {
                    // Variable is BoxedValue*, but value is native type - need to box it
                    std::cout << "[LLVM CodeGen] AssignExpr: Boxing native value for BoxedValue* variable" << std::endl;
                    
                    llvm::Value* newBoxed = nullptr;
                if (valueType->isIntegerTy(32)) {
                        newBoxed = createBoxedFromInt(value);
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
                        newBoxed = createBoxedFromInt(intValue);
                    }
                    
                builder_->CreateStore(newBoxed, alloca);
                current_value_ = newBoxed;
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
                    
                    current_value_ = value;
                } else {
                    // Both are native types
                if (valueType == varType) {
                        // Same types, direct assignment
                        std::cout << "[LLVM CodeGen] AssignExpr: Direct native assignment (same types)" << std::endl;
                    builder_->CreateStore(value, alloca);
            current_value_ = value;
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
            thenBoxed = createBoxedFromInt(thenValue);
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
            thenBoxed = createBoxedFromInt(intValue);
        }

        // Convertir else value a BoxedValue*
        builder_->SetInsertPoint(elseBlock->getTerminator());
        if (elseIsBoxed) {
            elseBoxed = elseValue;
        } else if (elseType->isIntegerTy(32)) {
            elseBoxed = createBoxedFromInt(elseValue);
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
            elseBoxed = createBoxedFromInt(intValue);
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
    
    // Allocate memory
    llvm::Value* size = llvm::ConstantExpr::getSizeOf(structType);
    llvm::Function* mallocFunc = module_->getFunction("malloc");
    if (!mallocFunc)
    {
        // Declare malloc if not already declared
        llvm::FunctionType* mallocType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                                               {llvm::Type::getInt64Ty(*context_)}, false);
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    }
    
    llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {size});
    current_value_ = builder_->CreateBitCast(rawPtr, llvm::PointerType::get(structType, 0));
}

void LLVMCodeGenerator::visit(GetAttrExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing GetAttrExpr: " << expr->attrName << std::endl;
    // Simplified implementation
    current_value_ = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
}

void LLVMCodeGenerator::visit(SetAttrExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing SetAttrExpr: " << expr->attrName << std::endl;
    // Simplified implementation
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
}

void LLVMCodeGenerator::visit(MethodCallExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing MethodCallExpr: " << expr->methodName << std::endl;
    // Simplified implementation
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
}

void LLVMCodeGenerator::visit(SelfExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing SelfExpr" << std::endl;
    // Simplified implementation
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