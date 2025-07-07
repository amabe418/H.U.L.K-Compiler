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
    bool isInteger = (expr->value == std::floor(expr->value));
    if (isInteger) {
        current_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), static_cast<int64_t>(expr->value));
    } else {
        current_value_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), expr->value);
    }
}

void LLVMCodeGenerator::visit(StringExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing StringExpr: " << expr->value << std::endl;
    auto strGlobal = registerStringConstant(expr->value);
    auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    current_value_ = builder_->CreateInBoundsGEP(
        strGlobal->getValueType(),
        strGlobal,
        {zero, zero},
        "str_ptr"
    );
}

void LLVMCodeGenerator::visit(BooleanExpr *expr)
{
    std::cout << "[LLVM CodeGen] Processing BooleanExpr: " << expr->value << std::endl;
    current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), expr->value);
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
    std::cout << "[LLVM CodeGen] Processing BinaryExpr" << std::endl;

    // Evaluar el operando izquierdo
    expr->left->accept(this);
    llvm::Value* left = current_value_;
    // Evaluar el operando derecho
    expr->right->accept(this);
    llvm::Value* right = current_value_;

    // Solo promover si ambos son numéricos (int o double)
    bool leftIsNum = left->getType()->isIntegerTy(32) || left->getType()->isDoubleTy();
    bool rightIsNum = right->getType()->isIntegerTy(32) || right->getType()->isDoubleTy();
    bool leftIsDouble = left->getType()->isDoubleTy();
    bool rightIsDouble = right->getType()->isDoubleTy();
    bool needPromotion = (leftIsNum && rightIsNum) && (leftIsDouble || rightIsDouble);

    if (needPromotion) {
        if (!leftIsDouble && left->getType()->isIntegerTy(32)) {
            left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
        }
        if (!rightIsDouble && right->getType()->isIntegerTy(32)) {
            right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
        }
    }

    // Realizar la operación según el tipo
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
        if (leftIsNum && rightIsNum)
            current_value_ = needPromotion ? builder_->CreateFAdd(left, right) : builder_->CreateAdd(left,right);
        else
            std::cerr << "Error: OP_ADD requiere operandos numéricos" << std::endl;
        break;
    case BinaryExpr::OP_SUB:
        if (leftIsNum && rightIsNum)
            current_value_ = needPromotion ? builder_->CreateFSub(left,right) : builder_->CreateSub(left,right);
        else
            std::cerr << "Error: OP_SUB requiere operandos numéricos" << std::endl;
        break;
    case BinaryExpr::OP_MUL:
        if (leftIsNum && rightIsNum)
            current_value_ = needPromotion ? builder_->CreateFMul(left, right) : builder_->CreateMul(left, right);
        else
            std::cerr << "Error: OP_MUL requiere operandos numéricos" << std::endl;
        break;
    case BinaryExpr::OP_DIV:
        if (leftIsNum && rightIsNum) {
            left = (!leftIsDouble) ? builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_)) : left;
            right = (!rightIsDouble) ? builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_)) : right;
            current_value_ = builder_->CreateFDiv(left, right);
        } else {
            std::cerr << "Error: OP_DIV requiere operandos numéricos" << std::endl;
        }
        break;
    case BinaryExpr::OP_POW: {
        if (leftIsNum && rightIsNum) {
            left = !leftIsDouble ? builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_)) : left;
            right = !rightIsDouble ? builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_)) : right;
            llvm::Function* powFunc = module_->getFunction("pow");
            if (!powFunc) {
                std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
                llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
                powFunc = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
            }
            std::vector<llvm::Value*> args = {left, right};
            current_value_ = builder_->CreateCall(powFunc, args);
        } else {
            std::cerr << "Error: OP_POW requiere operandos numéricos" << std::endl;
        }
        break;
    }
    case BinaryExpr::OP_LT:
        current_value_ = needPromotion ? builder_->CreateFCmpULT(left, right) : builder_->CreateICmpSLT(left, right);
        break;
    case BinaryExpr::OP_GT:
        current_value_ = needPromotion ? builder_->CreateFCmpUGT(left, right) : builder_->CreateICmpSGT(left, right);
        break;
    case BinaryExpr::OP_LE:
        current_value_ = needPromotion ? builder_->CreateFCmpULE(left, right) : builder_->CreateICmpSLE(left, right);
        break;
    case BinaryExpr::OP_GE:
        current_value_ = needPromotion ? builder_->CreateFCmpUGE(left, right) : builder_->CreateICmpSGE(left, right);
        break;
    case BinaryExpr::OP_EQ: {
        // Igualdad: puede ser entre cualquier tipo básico, pero ambos deben ser del mismo tipo
        llvm::Type* leftType = left->getType();
        llvm::Type* rightType = right->getType();
        
        // Double
        if (leftType->isDoubleTy() && rightType->isDoubleTy()) {
            current_value_ = builder_->CreateFCmpUEQ(left, right);
        // Enteros
        } else if (leftType->isIntegerTy(32) && rightType->isIntegerTy(32)) {
            current_value_ = builder_->CreateICmpEQ(left, right);
        // Booleanos
        } else if (leftType->isIntegerTy(1) && rightType->isIntegerTy(1)) {
            current_value_ = builder_->CreateICmpEQ(left, right);
        // Punteros
        } else if (leftType->isPointerTy() && rightType->isPointerTy()) {
            // Para strings y otros punteros
            current_value_ = builder_->CreateICmpEQ(left, right);
        // Enteros a Double
        } else if ((leftType->isIntegerTy(32) && rightType->isDoubleTy()) || 
                   (leftType->isDoubleTy() && rightType->isIntegerTy(32))) {
            // Solo convertir a double si ambos son tipos numéricos
            if (leftType->isIntegerTy(32)) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (rightType->isIntegerTy(32)) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFCmpUEQ(left, right);
        } else {
            // Tipos incompatibles para comparación
            std::cerr << "Error: Cannot compare incompatible types" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;
    }
    case BinaryExpr::OP_NEQ: {
        // Desigualdad: puede ser entre cualquier tipo básico, pero ambos deben ser del mismo tipo
        llvm::Type* leftType = left->getType();
        llvm::Type* rightType = right->getType();
        
        // Double
        if (leftType->isDoubleTy() && rightType->isDoubleTy()) {
            current_value_ = builder_->CreateFCmpUNE(left, right);
        // Enteros
        } else if (leftType->isIntegerTy(32) && rightType->isIntegerTy(32)) {
            current_value_ = builder_->CreateICmpNE(left, right);
        // Booleanos
        } else if (leftType->isIntegerTy(1) && rightType->isIntegerTy(1)) {
            current_value_ = builder_->CreateICmpNE(left, right);
        // Punteros
        } else if (leftType->isPointerTy() && rightType->isPointerTy()) {
            // Para strings y otros punteros
            current_value_ = builder_->CreateICmpNE(left, right);
        // Enteros a Double
        } else if ((leftType->isIntegerTy(32) && rightType->isDoubleTy()) || 
                   (leftType->isDoubleTy() && rightType->isIntegerTy(32))) {
            // Solo convertir a double si ambos son tipos numéricos
            if (leftType->isIntegerTy(32)) {
                left = builder_->CreateSIToFP(left, llvm::Type::getDoubleTy(*context_));
            }
            if (rightType->isIntegerTy(32)) {
                right = builder_->CreateSIToFP(right, llvm::Type::getDoubleTy(*context_));
            }
            current_value_ = builder_->CreateFCmpUNE(left, right);
        } else {
            // Tipos incompatibles para comparación
            std::cerr << "Error: Cannot compare incompatible types" << std::endl;
            current_value_ = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        }
        break;
    }
    case BinaryExpr::OP_AND: {
        // Operación AND lógica
        // Convertir operandos a booleanos si no lo son
        llvm::Value* leftBool = left;
        llvm::Value* rightBool = right;
        
        if (!left->getType()->isIntegerTy(1)) {
            // Convertir a booleano: 0 = false, cualquier otro valor = true
            if (left->getType()->isIntegerTy(32)) {
                auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
                leftBool = builder_->CreateICmpNE(left, zero);
            } else if (left->getType()->isDoubleTy()) {
                auto zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
                leftBool = builder_->CreateFCmpUNE(left, zero);
            } else {
                // Para otros tipos, asumir que es verdadero
                leftBool = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
            }
        }
        
        if (!right->getType()->isIntegerTy(1)) {
            // Convertir a booleano: 0 = false, cualquier otro valor = true
            if (right->getType()->isIntegerTy(32)) {
                auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
                rightBool = builder_->CreateICmpNE(right, zero);
            } else if (right->getType()->isDoubleTy()) {
                auto zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
                rightBool = builder_->CreateFCmpUNE(right, zero);
            } else {
                // Para otros tipos, asumir que es verdadero
                rightBool = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
            }
        }
        
        current_value_ = builder_->CreateAnd(leftBool, rightBool);
        break;
    }
    case BinaryExpr::OP_OR: {
        // Operación OR lógica
        // Convertir operandos a booleanos si no lo son
        llvm::Value* leftBool = left;
        llvm::Value* rightBool = right;
        
        if (!left->getType()->isIntegerTy(1)) {
            // Convertir a booleano: 0 = false, cualquier otro valor = true
            if (left->getType()->isIntegerTy(32)) {
                auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
                leftBool = builder_->CreateICmpNE(left, zero);
            } else if (left->getType()->isDoubleTy()) {
                auto zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
                leftBool = builder_->CreateFCmpUNE(left, zero);
            } else {
                // Para otros tipos, asumir que es verdadero
                leftBool = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
            }
        }
        
        if (!right->getType()->isIntegerTy(1)) {
            // Convertir a booleano: 0 = false, cualquier otro valor = true
            if (right->getType()->isIntegerTy(32)) {
                auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
                rightBool = builder_->CreateICmpNE(right, zero);
            } else if (right->getType()->isDoubleTy()) {
                auto zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
                rightBool = builder_->CreateFCmpUNE(right, zero);
            } else {
                // Para otros tipos, asumir que es verdadero
                rightBool = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
            }
        }
        
        current_value_ = builder_->CreateOr(leftBool, rightBool);
        break;
    }
    case BinaryExpr::OP_CONCAT: {
        std::cout << "[LLVM CodeGen] Processing string concatenation (@)" << std::endl;
        expr->left->accept(this);
        llvm::Value* left = current_value_;
        expr->right->accept(this);
        llvm::Value* right = current_value_;
        auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        // Convertir ambos operandos a string si es necesario
        llvm::Value* leftStr = left;
        if (!left->getType()->isPointerTy()) {
            leftStr = toStringLLVM(left);
        } else if (llvm::GlobalVariable* gv = llvm::dyn_cast<llvm::GlobalVariable>(left)) {
            leftStr = builder_->CreateInBoundsGEP(gv->getValueType(), gv, {zero, zero});
        }
        llvm::Value* rightStr = right;
        if (!right->getType()->isPointerTy()) {
            rightStr = toStringLLVM(right);
        } else if (llvm::GlobalVariable* gv = llvm::dyn_cast<llvm::GlobalVariable>(right)) {
            rightStr = builder_->CreateInBoundsGEP(gv->getValueType(), gv, {zero, zero});
        }
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
        llvm::Value* resultBuffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1024), "concat_result");
        builder_->CreateCall(strcpyFunc, {resultBuffer, leftStr});
        builder_->CreateCall(strcatFunc, {resultBuffer, rightStr});
        current_value_ = resultBuffer;
        break;
    }
    default:
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
            
            // Verificar si la variable es un BoxedValue
            bool isBoxedValue = false;
            if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(varType)) {
                if (ptrType->getNumContainedTypes() > 0) {
                    llvm::Type* elementType = ptrType->getContainedType(0);
                    if (elementType->isStructTy()) {
                        std::string structName = elementType->getStructName().str();
                        isBoxedValue = (structName == "BoxedValue");
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
    
    // Verificar si el valor inicial es un BoxedValue
    bool isBoxedValue = false;
    if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(varType)) {
        if (ptrType->getNumContainedTypes() > 0) {
            llvm::Type* elementType = ptrType->getContainedType(0);
            if (elementType->isStructTy()) {
                std::string structName = elementType->getStructName().str();
                isBoxedValue = (structName == "BoxedValue");
            }
        }
    }
    
    if (isBoxedValue) {
        std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " is initialized with BoxedValue - storing as BoxedValue*" << std::endl;
        
        // Para BoxedValue, crear un alloca que almacene el puntero al BoxedValue
        llvm::StructType* boxedTy = getBoxedValueType();
        llvm::Type* boxedPtrTy = boxedTy->getPointerTo();
        
        llvm::AllocaInst* alloca = entryBuilder.CreateAlloca(boxedPtrTy, nullptr, expr->name);
        builder_->CreateStore(initValue, alloca);
        current_scope_->variables[expr->name] = alloca;
        current_scope_->variable_types[expr->name] = boxedPtrTy;
        
        std::cout << "[LLVM CodeGen] LetExpr: Variable " << expr->name << " stored as BoxedValue*" << std::endl;
    } else {
        // Para tipos normales, crear alloca del tipo específico
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
            
            // Verificar si la variable es un BoxedValue*
            bool isBoxedValue = false;
            if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(varType)) {
                if (ptrType->getNumContainedTypes() > 0) {
                    llvm::Type* elementType = ptrType->getContainedType(0);
                    if (elementType->isStructTy()) {
                        std::string structName = elementType->getStructName().str();
                        isBoxedValue = (structName == "BoxedValue");
                    }
                }
            }
            
            if (isBoxedValue) {
                std::cout << "[LLVM CodeGen] AssignExpr: Variable " << expr->name << " is BoxedValue* - creating new BoxedValue" << std::endl;
                
                // La variable es BoxedValue*, necesitamos crear un nuevo BoxedValue
                llvm::StructType* boxedTy = getBoxedValueType();
                llvm::Value* newBoxed = builder_->CreateAlloca(boxedTy);
                
                // Determinar el tipo del valor y crear el BoxedValue apropiado
                llvm::Type* valueType = value->getType();
                
                if (valueType->isIntegerTy(32)) {
                    // Es un int
                    llvm::Value* tagPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 0);
                    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), tagPtr);
                    
                    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 1);
                    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
                    builder_->CreateStore(value, dataPtr);
                } else if (valueType->isDoubleTy()) {
                    // Es un double
                    llvm::Value* tagPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 0);
                    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), tagPtr);
                    
                    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 1);
                    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
                    builder_->CreateStore(value, dataPtr);
                } else if (valueType->isIntegerTy(1)) {
                    // Es un bool
                    llvm::Value* tagPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 0);
                    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), tagPtr);
                    
                    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 1);
                    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
                    builder_->CreateStore(value, dataPtr);
                } else if (valueType->isPointerTy()) {
                    // Es un string (puntero a char)
                    llvm::Value* tagPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 0);
                    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), tagPtr);
                    
                    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 1);
                    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
                    builder_->CreateStore(value, dataPtr);
                } else {
                    // Tipo desconocido, usar int como fallback
                    llvm::Value* tagPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 0);
                    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), tagPtr);
                    
                    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, newBoxed, 1);
                    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
                    llvm::Value* convertedValue = builder_->CreateBitCast(value, llvm::Type::getInt32Ty(*context_));
                    builder_->CreateStore(convertedValue, dataPtr);
                }
                
                // Almacenar el nuevo BoxedValue en la variable
                builder_->CreateStore(newBoxed, alloca);
                current_value_ = newBoxed;
            } else {
                // Variable de tipo nativo - verificar si necesitamos conversión de tipo
                llvm::Type* valueType = value->getType();
                
                if (valueType == varType) {
                    // Tipos iguales, asignación directa
                    std::cout << "[LLVM CodeGen] AssignExpr: Types match, direct assignment" << std::endl;
                    builder_->CreateStore(value, alloca);
            current_value_ = value;
                } else {
                    // Tipos diferentes, necesitamos cambiar el tipo de la variable al nuevo tipo
                    std::cout << "[LLVM CodeGen] AssignExpr: Types differ, changing variable " << expr->name << " from ";
                    varType->print(llvm::outs());
                    std::cout << " to ";
                    valueType->print(llvm::outs());
                    std::cout << std::endl;
                    
                    // Crear un nuevo alloca con el tipo del valor
                    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
                    llvm::AllocaInst* newAlloca = entryBuilder.CreateAlloca(valueType, nullptr, expr->name);
                    
                    // Almacenar el valor directamente (sin conversión)
                    builder_->CreateStore(value, newAlloca);
                    
                    // Actualizar la variable en el scope con el nuevo alloca y tipo
                    scope->variables[expr->name] = newAlloca;
                    scope->variable_types[expr->name] = valueType;
                    
                    current_value_ = value;
                    
                    std::cout << "[LLVM CodeGen] AssignExpr: Variable type changed successfully" << std::endl;
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

    // Evaluar la condición
    expr->condition->accept(this);
    llvm::Value* condition = current_value_;

    // Convertir la condición a booleano (i1) si no lo es
    if (!condition->getType()->isIntegerTy(1)) {
        if (condition->getType()->isIntegerTy(32)) {
            auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
            condition = builder_->CreateICmpNE(condition, zero);
        } else if (condition->getType()->isDoubleTy()) {
            auto zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
            condition = builder_->CreateFCmpUNE(condition, zero);
        } else {
            // Para otros tipos, asumir true
            condition = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
        }
    }

    // Crear los bloques básicos
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(*context_, "then", current_function_);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(*context_, "else", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "ifcont", current_function_);

    // Crear el branch condicional
    builder_->CreateCondBr(condition, thenBlock, elseBlock);

    // Evaluar tipos de las ramas
    llvm::Value* thenValue = nullptr;
    llvm::Value* elseValue = nullptr;
    llvm::Type* thenTy = nullptr;
    llvm::Type* elseTy = nullptr;

    // THEN: abrir scope y generar código
    builder_->SetInsertPoint(thenBlock);
    enterScope();
    expr->thenBranch->accept(this);
    thenValue = current_value_;
    thenTy = thenValue->getType();
    exitScope();

    // ELSE: abrir scope y generar código
    builder_->SetInsertPoint(elseBlock);
    enterScope();
    expr->elseBranch->accept(this);
    elseValue = current_value_;
    elseTy = elseValue->getType();
    exitScope();

    // Determinar si necesitamos usar BoxedValue
    bool useBoxed = false;
    bool thenIsUnknown = false;
    bool elseIsUnknown = false;
    
    // Verificar si alguno de los tipos es desconocido
    // En LLVM 19, los punteros no tienen element type, así que comparamos directamente
    // Un tipo es desconocido solo si realmente no podemos determinar su tipo específico
    // Los strings (punteros a i8*) son tipos válidos y conocidos
    if (thenTy->isPointerTy()) {
        // Para ahora, consideramos que todos los punteros son tipos válidos
        // Solo serían desconocidos si vienen de una expresión con tipo ambiguo
        // Por ahora, asumimos que si es un puntero, es un tipo válido
        thenIsUnknown = false;
    }
    if (elseTy->isPointerTy()) {
        // Para ahora, consideramos que todos los punteros son tipos válidos
        // Solo serían desconocidos si vienen de una expresión con tipo ambiguo
        // Por ahora, asumimos que si es un puntero, es un tipo válido
        elseIsUnknown = false;
    }
    
    // Usar BoxedValue solo si:
    // 1. Los tipos son diferentes
    // 2. Al menos uno de los tipos es desconocido
    useBoxed = (thenTy != elseTy) || thenIsUnknown || elseIsUnknown;
    
    std::cout << "[LLVM CodeGen] IfExpr analysis:" << std::endl;
    std::cout << "  - Then type: ";
    thenTy->print(llvm::outs());
    std::cout << " (unknown: " << thenIsUnknown << ")" << std::endl;
    std::cout << "  - Else type: ";
    elseTy->print(llvm::outs());
    std::cout << " (unknown: " << elseIsUnknown << ")" << std::endl;
    std::cout << "  - Types equal: " << (thenTy == elseTy) << std::endl;
    std::cout << "  - Using BoxedValue: " << useBoxed << std::endl;
    
    if (!useBoxed) {
        std::cout << "  - Result will be of type: ";
        thenTy->print(llvm::outs());
        std::cout << std::endl;
    } else {
        std::cout << "  - Result will be BoxedValue* (heterogeneous types)" << std::endl;
    }

    llvm::Value* boxedAlloca = nullptr;
    llvm::StructType* boxedTy = getBoxedValueType();

    if (useBoxed) {
        // Crear un único alloca para BoxedValue en el entry block
        llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
        boxedAlloca = entryBuilder.CreateAlloca(boxedTy, nullptr, "if_boxed_result");
        
        // THEN: inicializar boxedAlloca con el valor de thenValue
        builder_->SetInsertPoint(thenBlock);
        if (thenTy->isIntegerTy(1)) {
            // type_tag = 0
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
            builder_->CreateStore(thenValue, dataPtr);
        } else if (thenTy->isIntegerTy(32)) {
            // type_tag = 1
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
            builder_->CreateStore(thenValue, dataPtr);
        } else if (thenTy->isDoubleTy()) {
            // type_tag = 2
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
            builder_->CreateStore(thenValue, dataPtr);
        } else if (thenTy->isPointerTy()) {
            // type_tag = 3
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
            builder_->CreateStore(thenValue, dataPtr);
        }
        
        // ELSE: inicializar boxedAlloca con el valor de elseValue
        builder_->SetInsertPoint(elseBlock);
        if (elseTy->isIntegerTy(1)) {
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
            builder_->CreateStore(elseValue, dataPtr);
        } else if (elseTy->isIntegerTy(32)) {
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
            builder_->CreateStore(elseValue, dataPtr);
        } else if (elseTy->isDoubleTy()) {
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
            builder_->CreateStore(elseValue, dataPtr);
        } else if (elseTy->isPointerTy()) {
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), builder_->CreateStructGEP(boxedTy, boxedAlloca, 0));
            llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxedAlloca, 1);
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
            builder_->CreateStore(elseValue, dataPtr);
        }
    }

    // Generar los saltos al bloque de merge DESPUÉS de toda la inicialización
    if (!thenBlock->getTerminator()) {
        builder_->SetInsertPoint(thenBlock);
        builder_->CreateBr(mergeBlock);
    }
    
    if (!elseBlock->getTerminator()) {
        builder_->SetInsertPoint(elseBlock);
        builder_->CreateBr(mergeBlock);
    }

    // MERGE
    builder_->SetInsertPoint(mergeBlock);
    if (!useBoxed) {

        // Tipos iguales: usar PHI node con el tipo específico
        llvm::PHINode* phi = builder_->CreatePHI(thenTy, 2);
        phi->addIncoming(thenValue, thenBlock);
        phi->addIncoming(elseValue, elseBlock);
        current_value_ = phi;
        
        std::cout << "[LLVM CodeGen] IfExpr result: homogeneous types, using specific type: ";
        current_value_->getType()->print(llvm::outs());
        std::cout << std::endl;
    } else {
        // Tipos distintos o desconocidos: usar BoxedValue
        std::cout << "[LLVM CodeGen] IfExpr result: heterogeneous types, using BoxedValue*" << std::endl;
        
        // El resultado es el puntero al BoxedValue
        current_value_ = boxedAlloca;
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