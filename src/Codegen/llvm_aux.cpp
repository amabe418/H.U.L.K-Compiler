#include "llvm_codegen.hpp"
// Helper methods
std::string LLVMCodeGenerator::generateUniqueName(const std::string &base)
{
    return base + "_" + std::to_string(var_counter_++);
}

llvm::Type* LLVMCodeGenerator::getLLVMType(const TypeInfo &type)
{
    switch (type.getKind())
    {
    case TypeInfo::Kind::Number:
        // Por defecto usamos double para números, pero en NumberExpr se decide dinámicamente
        return llvm::Type::getDoubleTy(*context_);
    case TypeInfo::Kind::String:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    case TypeInfo::Kind::Boolean:
        return llvm::Type::getInt1Ty(*context_);
    case TypeInfo::Kind::Void:
        return llvm::Type::getVoidTy(*context_);
    case TypeInfo::Kind::Object:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0); // Simplified for now
    case TypeInfo::Kind::Unknown:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0); // Simplified for now
    default:
        return llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    }
}

llvm::Value* LLVMCodeGenerator::createConstant(const TypeInfo &type, const std::string &value)
{
    switch (type.getKind())
    {
    case TypeInfo::Kind::Number:
        return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), std::stod(value));
    case TypeInfo::Kind::String:
        return registerStringConstant(value);
    case TypeInfo::Kind::Boolean:
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), value == "true" ? 1 : 0);
    default:
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    }
}

void LLVMCodeGenerator::enterScope()
{
    scopes_.push_back(std::make_unique<LLVMScope>(current_scope_));
    current_scope_ = scopes_.back().get();
}

void LLVMCodeGenerator::exitScope()
{
    if (current_scope_->parent)
    {
        current_scope_ = current_scope_->parent;
        scopes_.pop_back();
    }
}

llvm::GlobalVariable* LLVMCodeGenerator::registerStringConstant(const std::string &value)
{
    // Check if already registered
    auto it = string_constants_.find(value);
    if (it != string_constants_.end())
    {
        return it->second;
    }

    // Create new string constant
    llvm::Constant* strConstant = llvm::ConstantDataArray::getString(*context_, value, true);
    llvm::GlobalVariable* globalVar = new llvm::GlobalVariable(
        *module_,
        strConstant->getType(),
        true,  // isConstant
        llvm::GlobalValue::PrivateLinkage,
        strConstant,
        ".str." + std::to_string(var_counter_++)
    );

    string_constants_[value] = globalVar;
    return globalVar;
}

llvm::Function* LLVMCodeGenerator::getOrCreateFunction(const std::string &name, llvm::Type* returnType, const std::vector<llvm::Type*>& paramTypes)
{
    llvm::Function* func = module_->getFunction(name);
    if (func)
    {
        return func;
    }

    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module_.get());
}

llvm::StructType* LLVMCodeGenerator::getOrCreateStructType(const std::string &name)
{
    auto it = types_.find(name);
    if (it != types_.end())
    {
        return it->second;
    }

    // Create a simple struct type for now
    std::vector<llvm::Type*> fields = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::StructType* structType = llvm::StructType::create(*context_, fields, "struct." + name);
    types_[name] = structType;
    return structType;
}

void LLVMCodeGenerator::generateConstructorFunction(TypeDecl *typeDecl)
{
    std::cout << "[LLVM CodeGen] Generating constructor function for type: " << typeDecl->name << std::endl;
    // Simplified implementation for now
}

void LLVMCodeGenerator::generateMethodFunction(MethodDecl *methodDecl)
{
    std::cout << "[LLVM CodeGen] Generating method function: " << methodDecl->name << std::endl;
    // Simplified implementation for now
}

void LLVMCodeGenerator::registerBuiltinFunctions()
{
    // Register printf
    std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), printfArgs, true);
    llvm::Function* printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module_.get());

    // Register malloc
    llvm::FunctionType* mallocType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
        {llvm::Type::getInt64Ty(*context_)}, false);
    llvm::Function* mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    
    // Register pow (math library)
    std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
    llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
    llvm::Function* powFunc = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
    
    // Register sprintf (for string formatting)
    std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                           llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
    llvm::Function* sprintfFunc = llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    
    // Register strcpy (for string copying)
    std::vector<llvm::Type*> strcpyArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                          llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* strcpyType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strcpyArgs, false);
    llvm::Function* strcpyFunc = llvm::Function::Create(strcpyType, llvm::Function::ExternalLinkage, "strcpy", module_.get());
    
    // Register strcat (for string concatenation)
    std::vector<llvm::Type*> strcatArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                          llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* strcatType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strcatArgs, false);
    llvm::Function* strcatFunc = llvm::Function::Create(strcatType, llvm::Function::ExternalLinkage, "strcat", module_.get());
    
    // Create BoxedValue helper functions
    std::cout << "[LLVM CodeGen] Creating BoxedValue helper functions" << std::endl;
    createPrintBoxedFunction();
    createUnboxFunction();
    createTypeCheckFunctions();
    createTypeSpecificUnboxFunctions();
    
    std::cout << "[LLVM CodeGen] All built-in and BoxedValue functions registered" << std::endl;
}

// Convierte cualquier valor LLVM a string (puntero a i8) usando sprintf y buffer temporal
llvm::Value* LLVMCodeGenerator::toStringLLVM(llvm::Value* value) {
    auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    llvm::Function* sprintfFunc = module_->getFunction("sprintf");
    if (!sprintfFunc) {
        std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
        sprintfFunc = llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    }
    llvm::Value* buffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 256), "str_temp");
    llvm::Value* format = nullptr;
    std::vector<llvm::Value*> args;
    if (value->getType()->isPointerTy()) {
        format = registerStringConstant("%s");
        args = {buffer, nullptr, value};
    } else if (value->getType()->isIntegerTy(32)) {
        format = registerStringConstant("%d");
        args = {buffer, nullptr, value};
    } else if (value->getType()->isDoubleTy()) {
        format = registerStringConstant("%f");
        args = {buffer, nullptr, value};
    } else if (value->getType()->isIntegerTy(1)) {
        format = registerStringConstant("%s");
        auto trueStr = registerStringConstant("true");
        auto falseStr = registerStringConstant("false");
        auto truePtr = builder_->CreateInBoundsGEP(trueStr->getValueType(), trueStr, {zero, zero});
        auto falsePtr = builder_->CreateInBoundsGEP(falseStr->getValueType(), falseStr, {zero, zero});
        value = builder_->CreateSelect(value, truePtr, falsePtr);
        args = {buffer, nullptr, value};
    } else {
        format = registerStringConstant("%p");
        args = {buffer, nullptr, value};
    }
    auto formatPtr = builder_->CreateInBoundsGEP(
        llvm::cast<llvm::GlobalVariable>(format)->getValueType(),
        llvm::cast<llvm::GlobalVariable>(format),
        {zero, zero}
    );
    args[1] = formatPtr; // buffer, format, value
    builder_->CreateCall(sprintfFunc, args);
    return buffer;
}

// === BoxedValue helpers ===
llvm::StructType* LLVMCodeGenerator::getBoxedValueType() {
    // Check if already created in this module
    auto it = types_.find("BoxedValue");
    if (it != types_.end()) {
        return it->second;
    }
    
    // Create new BoxedValue type
    std::vector<llvm::Type*> fields;
    fields.push_back(llvm::Type::getInt32Ty(*context_)); // type_tag
    fields.push_back(llvm::ArrayType::get(llvm::Type::getInt8Ty(*context_), 8)); // data (8 bytes)
    llvm::StructType* boxedTy = llvm::StructType::create(*context_, fields, "BoxedValue");
    types_["BoxedValue"] = boxedTy;
    return boxedTy;
}

llvm::Value* LLVMCodeGenerator::createBoxedFromInt(llvm::Value* intVal) {
    auto boxedTy = getBoxedValueType();
    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
    llvm::Value* boxed = entryBuilder.CreateAlloca(boxedTy, nullptr, "boxed_int");
    // type_tag = 1
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), builder_->CreateStructGEP(boxedTy, boxed, 0));
    // Guardar el int en los primeros 4 bytes de data
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
    builder_->CreateStore(intVal, dataPtr);
    return boxed;
}

llvm::Value* LLVMCodeGenerator::createBoxedFromDouble(llvm::Value* doubleVal) {
    auto boxedTy = getBoxedValueType();
    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
    llvm::Value* boxed = entryBuilder.CreateAlloca(boxedTy, nullptr, "boxed_double");
    // type_tag = 2
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), builder_->CreateStructGEP(boxedTy, boxed, 0));
    // Guardar el double en los primeros 8 bytes de data
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    builder_->CreateStore(doubleVal, dataPtr);
    return boxed;
}

llvm::Value* LLVMCodeGenerator::createBoxedFromBool(llvm::Value* boolVal) {
    auto boxedTy = getBoxedValueType();
    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
    llvm::Value* boxed = entryBuilder.CreateAlloca(boxedTy, nullptr, "boxed_bool");
    // type_tag = 0
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), builder_->CreateStructGEP(boxedTy, boxed, 0));
    // Guardar el bool en los primeros 1 bytes de data
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
    builder_->CreateStore(boolVal, dataPtr);
    return boxed;
}

llvm::Value* LLVMCodeGenerator::createBoxedFromString(llvm::Value* strVal) {
    auto boxedTy = getBoxedValueType();
    llvm::IRBuilder<> entryBuilder(&current_function_->getEntryBlock(), current_function_->getEntryBlock().begin());
    llvm::Value* boxed = entryBuilder.CreateAlloca(boxedTy, nullptr, "boxed_str");
    // type_tag = 3
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), builder_->CreateStructGEP(boxedTy, boxed, 0));
    // Guardar el puntero en los primeros 8 bytes de data
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    builder_->CreateStore(strVal, dataPtr);
    return boxed;
}

llvm::Value* LLVMCodeGenerator::extractFromBoxed(llvm::Value* boxed, int type_tag) {
    auto boxedTy = getBoxedValueType();
    // Leer el type_tag
    llvm::Value* tagPtr = builder_->CreateStructGEP(boxedTy, boxed, 0);
    llvm::Value* tag = builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    // Comparar con el esperado (eliminado cmp porque no se usa)
    // Extraer el valor según el tipo
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    switch (type_tag) {
        case 0: // bool
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
            return builder_->CreateLoad(llvm::Type::getInt1Ty(*context_), dataPtr);
        case 1: // int
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
            return builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), dataPtr);
        case 2: // double
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
            return builder_->CreateLoad(llvm::Type::getDoubleTy(*context_), dataPtr);
        case 3: // string
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
            return builder_->CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), dataPtr);
        default:
            // Valor desconocido
            return llvm::Constant::getNullValue(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    }
}

llvm::Value* LLVMCodeGenerator::unbox(llvm::Value* boxed) {
    // Placeholder - las funciones is* ya están disponibles como built-in functions
    // Esta función puede ser usada para llamar a las funciones específicas según el contexto
    std::cout << "[LLVM CodeGen] Unbox function called - use specific is* functions instead" << std::endl;
    return boxed; // Return the same value for now
}

void LLVMCodeGenerator::handlePrintFunction(CallExpr *expr)
{
    std::cout << "[LLVM CodeGen] Handling print function" << std::endl;
    
    expr->args[0]->accept(this);
    llvm::Value* arg = current_value_;
    
    // Determinar el tipo y generar el printf apropiado
    llvm::Type* argType = arg->getType();
    
    std::cout << "[LLVM CodeGen] Print argument type: ";
    argType->print(llvm::outs());
    std::cout << std::endl;
    
    // Check if the argument is a BoxedValue* using more robust type checking
    llvm::StructType* boxedTy = getBoxedValueType();
    
    std::cout << "[LLVM CodeGen] BoxedValue type: ";
    boxedTy->print(llvm::outs());
    std::cout << std::endl;
    
    // Check if the argument is a BoxedValue* or a string (i8*) using robust type checking
    bool isBoxedValue = false;
    bool isString = false;
    
    std::cout << "[LLVM CodeGen] Detailed type analysis:" << std::endl;
    std::cout << "  - Is pointer: " << (argType->isPointerTy() ? "yes" : "no") << std::endl;
    
    if (argType->isPointerTy()) {
        auto ptrType = llvm::dyn_cast<llvm::PointerType>(argType);
        
        // In LLVM 19, we need to use a different approach to get the element type
        llvm::Type* elementType = nullptr;
        
        // Try to get element type using the new LLVM 19 API
        if (ptrType->getNumContainedTypes() > 0) {
            elementType = ptrType->getContainedType(0);
            std::cout << "  - Number of contained types: " << ptrType->getNumContainedTypes() << std::endl;
        } else {
            // Fallback: try to get element type using pointer arithmetic
            // This is a workaround for LLVM 19 where getNumContainedTypes() might return 0
            std::cout << "  - getNumContainedTypes() returned 0, trying alternative detection" << std::endl;
            
            // Try to determine the type by checking if it's a known struct type
            // We'll compare the pointer type directly with known types
            if (argType == llvm::PointerType::get(boxedTy, 0)) {
                elementType = boxedTy;
                std::cout << "  - Detected as BoxedValue* by direct type comparison" << std::endl;
            } else if (argType == llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)) {
                elementType = llvm::Type::getInt8Ty(*context_);
                std::cout << "  - Detected as i8* by direct type comparison" << std::endl;
            }
        }
        
        if (elementType) {
            std::cout << "  - Element type: ";
            elementType->print(llvm::outs());
            std::cout << std::endl;
            
            // Strategy 1: Check if it's a string (i8*)
            if (elementType->isIntegerTy(8)) {
                isString = true;
                std::cout << "[LLVM CodeGen] Detected as string (i8*)" << std::endl;
            }
            // Strategy 2: Check if it's a BoxedValue struct by name
            else if (elementType->isStructTy()) {
                std::string structName = elementType->getStructName().str();
                std::cout << "  - Struct name: '" << structName << "'" << std::endl;
                isBoxedValue = (structName == "BoxedValue");
                std::cout << "[LLVM CodeGen] Detected as struct: " << structName << std::endl;
            }
            // Strategy 3: Check if it's a BoxedValue struct by comparing with known type
            else if (elementType->isStructTy()) {
                // Compare with the known BoxedValue type
                std::cout << "  - Comparing with BoxedValue type..." << std::endl;
                if (elementType == boxedTy) {
                    isBoxedValue = true;
                    std::cout << "[LLVM CodeGen] Detected as BoxedValue by type comparison" << std::endl;
                } else {
                    std::cout << "  - Type comparison failed" << std::endl;
                }
            }
        } else {
            std::cout << "  - Could not determine element type" << std::endl;
        }
    }
    
    // Strategy 4: Fallback - if it's a pointer but we couldn't determine the type,
    // assume it's a string (most common case for pointers)
    if (argType->isPointerTy() && !isBoxedValue && !isString) {
        isString = true;
        std::cout << "[LLVM CodeGen] Fallback: treating pointer as string" << std::endl;
    }
    
    std::cout << "[LLVM CodeGen] Print argument type: ";
    argType->print(llvm::outs());
    std::cout << std::endl;
    std::cout << "[LLVM CodeGen] Is string: " << isString << std::endl;
    std::cout << "[LLVM CodeGen] Is BoxedValue: " << isBoxedValue << std::endl;
    
    if (isBoxedValue) {
        std::cout << "[LLVM CodeGen] Will use print_boxed function" << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] Will use regular printf" << std::endl;
    }
    
    if (isBoxedValue) {
        // It's a BoxedValue*, use print_boxed function
        llvm::Function* printBoxedFunc = module_->getFunction("print_boxed");
        if (!printBoxedFunc) {
            std::vector<llvm::Type*> printBoxedArgs = {llvm::PointerType::get(boxedTy, 0)};
            llvm::FunctionType* printBoxedType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), printBoxedArgs, false);
            printBoxedFunc = llvm::Function::Create(printBoxedType, llvm::Function::ExternalLinkage, "print_boxed", module_.get());
        }
        
        std::vector<llvm::Value*> args = {arg};
        builder_->CreateCall(printBoxedFunc, args);
        current_value_ = nullptr; // print_boxed returns void
        return;
    }
    
    llvm::Value* formatStr = nullptr;
    
    if (argType->isIntegerTy(32)) {
        // Entero: usar %d
        formatStr = registerStringConstant("%d\n");
    } else if (argType->isDoubleTy()) {
        // Double: usar %f
        formatStr = registerStringConstant("%f\n");
    } else if (argType->isIntegerTy(1)) {
        // Booleano: usar %s con "true"/"false"
        llvm::Value* trueStr = registerStringConstant("true\n");
        llvm::Value* falseStr = registerStringConstant("false\n");
        
        // Crear un select para elegir entre "true" y "false"
        auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        
        auto truePtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
            llvm::cast<llvm::GlobalVariable>(trueStr)->getValueType(), 
            llvm::cast<llvm::GlobalVariable>(trueStr), 
            llvm::ArrayRef<llvm::Constant*>{zero, zero}
        );
        auto falsePtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
            llvm::cast<llvm::GlobalVariable>(falseStr)->getValueType(), 
            llvm::cast<llvm::GlobalVariable>(falseStr), 
            llvm::ArrayRef<llvm::Constant*>{zero, zero}
        );
        
        formatStr = builder_->CreateSelect(arg, truePtr, falsePtr);
        
        // Llamar printf con el string seleccionado
        std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), printfArgs, true);
        llvm::Function* printfFunc = module_->getFunction("printf");
        if (!printfFunc) {
            printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module_.get());
        }
        
        std::vector<llvm::Value*> args = {formatStr};
        current_value_ = builder_->CreateCall(printfFunc, args);
        return;
    } else if (argType->isPointerTy()) {
        // String: usar %s (asumimos que es un string si es un puntero)
        formatStr = registerStringConstant("%s\n");
    } else {
        // Tipo desconocido: usar %p (puntero)
        formatStr = registerStringConstant("%p\n");
    }
    
    // Obtener el puntero al string de formato
    auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    auto formatPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(formatStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(formatStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    
    // Llamar printf
    std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), printfArgs, true);
    llvm::Function* printfFunc = module_->getFunction("printf");
    if (!printfFunc) {
        printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module_.get());
    }
    
    std::vector<llvm::Value*> args;
    args.push_back(formatPtr);
    args.push_back(arg);
    current_value_ = builder_->CreateCall(printfFunc, args);
}

void LLVMCodeGenerator::createPrintBoxedFunction()
{
    std::cout << "[LLVM CodeGen] Creating print_boxed function" << std::endl;
    
    // Get printf function
    std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), printfArgs, true);
    llvm::Function* printfFunc = module_->getFunction("printf");
    if (!printfFunc) {
        printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module_.get());
    }
    
    // Create print_boxed function
    llvm::StructType* boxedTy = getBoxedValueType();
    std::vector<llvm::Type*> printBoxedArgs = {llvm::PointerType::get(boxedTy, 0)};
    llvm::FunctionType* printBoxedType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), printBoxedArgs, false);
    llvm::Function* printBoxedFunc = llvm::Function::Create(printBoxedType, llvm::Function::InternalLinkage, "print_boxed", module_.get());
    
    // Implement print_boxed function
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(*context_, "entry", printBoxedFunc);
    llvm::IRBuilder<> printBuilder(entryBlock);
    
    // Get the BoxedValue parameter
    llvm::Value* boxedValue = printBoxedFunc->getArg(0);
    
    // Get type tag
    llvm::Value* typeTagPtr = printBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    llvm::Value* typeTag = printBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), typeTagPtr);
    
    // Create basic blocks for different types
    llvm::BasicBlock* intBlock = llvm::BasicBlock::Create(*context_, "print_int", printBoxedFunc);
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "print_double", printBoxedFunc);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "print_string", printBoxedFunc);
    llvm::BasicBlock* boolBlock = llvm::BasicBlock::Create(*context_, "print_bool", printBoxedFunc);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge", printBoxedFunc);
    
    // Switch on type tag (similar to script.hulk.ll)
    llvm::SwitchInst* switchInst = printBuilder.CreateSwitch(typeTag, intBlock, 4);
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);   // bool
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), intBlock);    // int
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), doubleBlock); // double
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), stringBlock); // string
    
    // Print boolean
    printBuilder.SetInsertPoint(boolBlock);
    llvm::Value* boolDataPtr = printBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    boolDataPtr = printBuilder.CreatePointerCast(boolDataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
    llvm::Value* boolValue = printBuilder.CreateLoad(llvm::Type::getInt1Ty(*context_), boolDataPtr);
    
    llvm::Value* trueStr = registerStringConstant("true\n");
    llvm::Value* falseStr = registerStringConstant("false\n");
    auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    
    auto truePtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(trueStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(trueStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    auto falsePtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(falseStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(falseStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    
    llvm::Value* boolFormatStr = printBuilder.CreateSelect(boolValue, truePtr, falsePtr);
    std::vector<llvm::Value*> boolArgs = {boolFormatStr};
    printBuilder.CreateCall(printfFunc, boolArgs);
    printBuilder.CreateBr(mergeBlock);
    
    // Print int
    printBuilder.SetInsertPoint(intBlock);
    llvm::Value* intDataPtr = printBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    intDataPtr = printBuilder.CreatePointerCast(intDataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
    llvm::Value* intValue = printBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), intDataPtr);
    
    llvm::Value* intFormatStr = registerStringConstant("%d\n");
    auto intFormatPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(intFormatStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(intFormatStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    std::vector<llvm::Value*> intArgs = {intFormatPtr, intValue};
    printBuilder.CreateCall(printfFunc, intArgs);
    printBuilder.CreateBr(mergeBlock);
    
    // Print double
    printBuilder.SetInsertPoint(doubleBlock);
    llvm::Value* doubleDataPtr = printBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    doubleDataPtr = printBuilder.CreatePointerCast(doubleDataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    llvm::Value* doubleValue = printBuilder.CreateLoad(llvm::Type::getDoubleTy(*context_), doubleDataPtr);
    
    llvm::Value* doubleFormatStr = registerStringConstant("%f\n");
    auto doubleFormatPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(doubleFormatStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(doubleFormatStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    std::vector<llvm::Value*> doubleArgs = {doubleFormatPtr, doubleValue};
    printBuilder.CreateCall(printfFunc, doubleArgs);
    printBuilder.CreateBr(mergeBlock);
    
    // Print string
    printBuilder.SetInsertPoint(stringBlock);
    llvm::Value* strDataPtr = printBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    strDataPtr = printBuilder.CreatePointerCast(strDataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    llvm::Value* strValue = printBuilder.CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strDataPtr);
    
    llvm::Value* strFormatStr = registerStringConstant("%s\n");
    auto strFormatPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(strFormatStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(strFormatStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    std::vector<llvm::Value*> strArgs = {strFormatPtr, strValue};
    printBuilder.CreateCall(printfFunc, strArgs);
    printBuilder.CreateBr(mergeBlock);
    
    // Merge block
    printBuilder.SetInsertPoint(mergeBlock);
    printBuilder.CreateRetVoid();
}

void LLVMCodeGenerator::createUnboxFunction()
{
    std::cout << "[LLVM CodeGen] Creating unbox function" << std::endl;
    
    // Create unbox function
    llvm::StructType* boxedTy = getBoxedValueType();
    std::vector<llvm::Type*> unboxArgs = {llvm::PointerType::get(boxedTy, 0)};
    llvm::FunctionType* unboxType = llvm::FunctionType::get(llvm::Type::getInt8Ty(*context_)->getPointerTo(), unboxArgs, false);
    llvm::Function* unboxFunc = llvm::Function::Create(unboxType, llvm::Function::InternalLinkage, "unbox", module_.get());
    
    // Implement unbox function
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(*context_, "entry", unboxFunc);
    llvm::IRBuilder<> unboxBuilder(entryBlock);
    
    // Get the BoxedValue parameter
    llvm::Value* boxedValue = unboxFunc->getArg(0);
    
    // Get type tag
    llvm::Value* typeTagPtr = unboxBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    llvm::Value* typeTag = unboxBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), typeTagPtr);
    
    // Create basic blocks for different types
    llvm::BasicBlock* boolBlock = llvm::BasicBlock::Create(*context_, "unbox_bool", unboxFunc);
    llvm::BasicBlock* intBlock = llvm::BasicBlock::Create(*context_, "unbox_int", unboxFunc);
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "unbox_double", unboxFunc);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "unbox_string", unboxFunc);
    
    // Switch on type tag
    llvm::SwitchInst* switchInst = unboxBuilder.CreateSwitch(typeTag, boolBlock, 4);
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);   // bool
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), intBlock);    // int
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), doubleBlock); // double
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), stringBlock); // string
    
    // Unbox boolean
    unboxBuilder.SetInsertPoint(boolBlock);
    llvm::Value* boolDataPtr = unboxBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    boolDataPtr = unboxBuilder.CreatePointerCast(boolDataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
    llvm::Value* boolValue = unboxBuilder.CreateLoad(llvm::Type::getInt1Ty(*context_), boolDataPtr);
    llvm::Value* boolResult = unboxBuilder.CreateIntToPtr(boolValue, llvm::Type::getInt8Ty(*context_)->getPointerTo());
    unboxBuilder.CreateRet(boolResult);
    
    // Unbox int
    unboxBuilder.SetInsertPoint(intBlock);
    llvm::Value* intDataPtr = unboxBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    intDataPtr = unboxBuilder.CreatePointerCast(intDataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
    llvm::Value* intValue = unboxBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), intDataPtr);
    llvm::Value* intResult = unboxBuilder.CreateIntToPtr(intValue, llvm::Type::getInt8Ty(*context_)->getPointerTo());
    unboxBuilder.CreateRet(intResult);
    
    // Unbox double
    unboxBuilder.SetInsertPoint(doubleBlock);
    llvm::Value* doubleDataPtr = unboxBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    doubleDataPtr = unboxBuilder.CreatePointerCast(doubleDataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    llvm::Value* doubleValue = unboxBuilder.CreateLoad(llvm::Type::getDoubleTy(*context_), doubleDataPtr);
    // Convert double to i64 first, then to ptr
    llvm::Value* doubleAsInt = unboxBuilder.CreateBitCast(doubleValue, llvm::Type::getInt64Ty(*context_));
    llvm::Value* doubleResult = unboxBuilder.CreateIntToPtr(doubleAsInt, llvm::Type::getInt8Ty(*context_)->getPointerTo());
    unboxBuilder.CreateRet(doubleResult);
    
    // Unbox string
    unboxBuilder.SetInsertPoint(stringBlock);
    llvm::Value* stringDataPtr = unboxBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    stringDataPtr = unboxBuilder.CreatePointerCast(stringDataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    llvm::Value* stringValue = unboxBuilder.CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), stringDataPtr);
    unboxBuilder.CreateRet(stringValue);
}

void LLVMCodeGenerator::createTypeCheckFunctions()
{
    std::cout << "[LLVM CodeGen] Creating type check functions" << std::endl;
    
    llvm::StructType* boxedTy = getBoxedValueType();
    std::vector<llvm::Type*> funcArgs = {llvm::PointerType::get(boxedTy, 0)};
    llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getInt1Ty(*context_), funcArgs, false);
    
    // isInt
    llvm::Function* isIntFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "isInt", module_.get());
    llvm::BasicBlock* isIntEntry = llvm::BasicBlock::Create(*context_, "entry", isIntFunc);
    llvm::IRBuilder<> isIntBuilder(isIntEntry);
    llvm::Value* boxedValue = isIntFunc->getArg(0);
    llvm::Value* tagPtr = isIntBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    llvm::Value* tag = isIntBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    llvm::Value* cmp = isIntBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1));
    isIntBuilder.CreateRet(cmp);
    
    // isDouble
    llvm::Function* isDoubleFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "isDouble", module_.get());
    llvm::BasicBlock* isDoubleEntry = llvm::BasicBlock::Create(*context_, "entry", isDoubleFunc);
    llvm::IRBuilder<> isDoubleBuilder(isDoubleEntry);
    boxedValue = isDoubleFunc->getArg(0);
    tagPtr = isDoubleBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    tag = isDoubleBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    cmp = isDoubleBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2));
    isDoubleBuilder.CreateRet(cmp);
    
    // isBool
    llvm::Function* isBoolFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "isBool", module_.get());
    llvm::BasicBlock* isBoolEntry = llvm::BasicBlock::Create(*context_, "entry", isBoolFunc);
    llvm::IRBuilder<> isBoolBuilder(isBoolEntry);
    boxedValue = isBoolFunc->getArg(0);
    tagPtr = isBoolBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    tag = isBoolBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    cmp = isBoolBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
    isBoolBuilder.CreateRet(cmp);
    
    // isString
    llvm::Function* isStringFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "isString", module_.get());
    llvm::BasicBlock* isStringEntry = llvm::BasicBlock::Create(*context_, "entry", isStringFunc);
    llvm::IRBuilder<> isStringBuilder(isStringEntry);
    boxedValue = isStringFunc->getArg(0);
    tagPtr = isStringBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    tag = isStringBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    cmp = isStringBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3));
    isStringBuilder.CreateRet(cmp);
}

void LLVMCodeGenerator::createTypeSpecificUnboxFunctions()
{
    std::cout << "[LLVM CodeGen] Creating type-specific unbox functions" << std::endl;
    
    llvm::StructType* boxedTy = getBoxedValueType();
    std::vector<llvm::Type*> funcArgs = {llvm::PointerType::get(boxedTy, 0)};
    
    // Create unboxInt function
    llvm::FunctionType* unboxIntType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), funcArgs, false);
    llvm::Function* unboxIntFunc = llvm::Function::Create(unboxIntType, llvm::Function::InternalLinkage, "unboxInt", module_.get());
    llvm::BasicBlock* unboxIntEntry = llvm::BasicBlock::Create(*context_, "entry", unboxIntFunc);
    llvm::IRBuilder<> unboxIntBuilder(unboxIntEntry);
    
    llvm::Value* boxedValue = unboxIntFunc->getArg(0);
    llvm::Value* intDataPtr = unboxIntBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    intDataPtr = unboxIntBuilder.CreatePointerCast(intDataPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(*context_), 0));
    llvm::Value* intValue = unboxIntBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), intDataPtr);
    unboxIntBuilder.CreateRet(intValue);
    
    // Create unboxDouble function
    llvm::FunctionType* unboxDoubleType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), funcArgs, false);
    llvm::Function* unboxDoubleFunc = llvm::Function::Create(unboxDoubleType, llvm::Function::InternalLinkage, "unboxDouble", module_.get());
    llvm::BasicBlock* unboxDoubleEntry = llvm::BasicBlock::Create(*context_, "entry", unboxDoubleFunc);
    llvm::IRBuilder<> unboxDoubleBuilder(unboxDoubleEntry);
    
    boxedValue = unboxDoubleFunc->getArg(0);
    llvm::Value* doubleDataPtr = unboxDoubleBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    doubleDataPtr = unboxDoubleBuilder.CreatePointerCast(doubleDataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    llvm::Value* doubleValue = unboxDoubleBuilder.CreateLoad(llvm::Type::getDoubleTy(*context_), doubleDataPtr);
    unboxDoubleBuilder.CreateRet(doubleValue);
    
    // Create unboxBool function
    llvm::FunctionType* unboxBoolType = llvm::FunctionType::get(llvm::Type::getInt1Ty(*context_), funcArgs, false);
    llvm::Function* unboxBoolFunc = llvm::Function::Create(unboxBoolType, llvm::Function::InternalLinkage, "unboxBool", module_.get());
    llvm::BasicBlock* unboxBoolEntry = llvm::BasicBlock::Create(*context_, "entry", unboxBoolFunc);
    llvm::IRBuilder<> unboxBoolBuilder(unboxBoolEntry);
    
    boxedValue = unboxBoolFunc->getArg(0);
    llvm::Value* boolDataPtr = unboxBoolBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    boolDataPtr = unboxBoolBuilder.CreatePointerCast(boolDataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
    llvm::Value* boolValue = unboxBoolBuilder.CreateLoad(llvm::Type::getInt1Ty(*context_), boolDataPtr);
    unboxBoolBuilder.CreateRet(boolValue);
    
    // Create unboxString function
    llvm::FunctionType* unboxStringType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), funcArgs, false);
    llvm::Function* unboxStringFunc = llvm::Function::Create(unboxStringType, llvm::Function::InternalLinkage, "unboxString", module_.get());
    llvm::BasicBlock* unboxStringEntry = llvm::BasicBlock::Create(*context_, "entry", unboxStringFunc);
    llvm::IRBuilder<> unboxStringBuilder(unboxStringEntry);
    
    boxedValue = unboxStringFunc->getArg(0);
    llvm::Value* stringDataPtr = unboxStringBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    stringDataPtr = unboxStringBuilder.CreatePointerCast(stringDataPtr, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    llvm::Value* stringValue = unboxStringBuilder.CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), stringDataPtr);
    unboxStringBuilder.CreateRet(stringValue);
}

// Type checking functions
llvm::Value* LLVMCodeGenerator::isInt(llvm::Value* boxed) {
    llvm::Function* isIntFunc = module_->getFunction("isInt");
    if (!isIntFunc) {
        createTypeCheckFunctions();
        isIntFunc = module_->getFunction("isInt");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(isIntFunc, args);
}

llvm::Value* LLVMCodeGenerator::isDouble(llvm::Value* boxed) {
    llvm::Function* isDoubleFunc = module_->getFunction("isDouble");
    if (!isDoubleFunc) {
        createTypeCheckFunctions();
        isDoubleFunc = module_->getFunction("isDouble");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(isDoubleFunc, args);
}

llvm::Value* LLVMCodeGenerator::isBool(llvm::Value* boxed) {
    llvm::Function* isBoolFunc = module_->getFunction("isBool");
    if (!isBoolFunc) {
        createTypeCheckFunctions();
        isBoolFunc = module_->getFunction("isBool");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(isBoolFunc, args);
}

llvm::Value* LLVMCodeGenerator::isString(llvm::Value* boxed) {
    llvm::Function* isStringFunc = module_->getFunction("isString");
    if (!isStringFunc) {
        createTypeCheckFunctions();
        isStringFunc = module_->getFunction("isString");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(isStringFunc, args);
}

// Type-specific unbox functions
llvm::Value* LLVMCodeGenerator::unboxInt(llvm::Value* boxed) {
    llvm::Function* unboxIntFunc = module_->getFunction("unboxInt");
    if (!unboxIntFunc) {
        createTypeSpecificUnboxFunctions();
        unboxIntFunc = module_->getFunction("unboxInt");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(unboxIntFunc, args);
}

llvm::Value* LLVMCodeGenerator::unboxDouble(llvm::Value* boxed) {
    llvm::Function* unboxDoubleFunc = module_->getFunction("unboxDouble");
    if (!unboxDoubleFunc) {
        createTypeSpecificUnboxFunctions();
        unboxDoubleFunc = module_->getFunction("unboxDouble");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(unboxDoubleFunc, args);
}

llvm::Value* LLVMCodeGenerator::unboxBool(llvm::Value* boxed) {
    llvm::Function* unboxBoolFunc = module_->getFunction("unboxBool");
    if (!unboxBoolFunc) {
        createTypeSpecificUnboxFunctions();
        unboxBoolFunc = module_->getFunction("unboxBool");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(unboxBoolFunc, args);
}

llvm::Value* LLVMCodeGenerator::unboxString(llvm::Value* boxed) {
    llvm::Function* unboxStringFunc = module_->getFunction("unboxString");
    if (!unboxStringFunc) {
        createTypeSpecificUnboxFunctions();
        unboxStringFunc = module_->getFunction("unboxString");
    }
    std::vector<llvm::Value*> args = {boxed};
    return builder_->CreateCall(unboxStringFunc, args);
}

// === Type conversion functions for destructive assignments ===

llvm::Value* LLVMCodeGenerator::convertToInt(llvm::Value* value) {
    llvm::Type* valueType = value->getType();
    
    if (valueType->isIntegerTy(32)) {
        return value; // Ya es int
    } else if (valueType->isDoubleTy()) {
        return builder_->CreateFPToSI(value, llvm::Type::getInt32Ty(*context_));
    } else if (valueType->isIntegerTy(1)) {
        return builder_->CreateZExt(value, llvm::Type::getInt32Ty(*context_));
    } else if (valueType->isPointerTy()) {
        // Para strings, intentar convertir a int (usar atoi)
        llvm::Function* atoiFunc = module_->getFunction("atoi");
        if (!atoiFunc) {
            std::vector<llvm::Type*> atoiArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
            llvm::FunctionType* atoiType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), atoiArgs, false);
            atoiFunc = llvm::Function::Create(atoiType, llvm::Function::ExternalLinkage, "atoi", module_.get());
        }
        std::vector<llvm::Value*> args = {value};
        return builder_->CreateCall(atoiFunc, args);
    } else {
        // Tipo desconocido, intentar conversión por defecto
        return builder_->CreateBitCast(value, llvm::Type::getInt32Ty(*context_));
    }
}

llvm::Value* LLVMCodeGenerator::convertToDouble(llvm::Value* value) {
    llvm::Type* valueType = value->getType();
    
    if (valueType->isDoubleTy()) {
        return value; // Ya es double
    } else if (valueType->isIntegerTy(32)) {
        return builder_->CreateSIToFP(value, llvm::Type::getDoubleTy(*context_));
    } else if (valueType->isIntegerTy(1)) {
        llvm::Value* intVal = builder_->CreateZExt(value, llvm::Type::getInt32Ty(*context_));
        return builder_->CreateSIToFP(intVal, llvm::Type::getDoubleTy(*context_));
    } else if (valueType->isPointerTy()) {
        // Para strings, intentar convertir a double (usar atof)
        llvm::Function* atofFunc = module_->getFunction("atof");
        if (!atofFunc) {
            std::vector<llvm::Type*> atofArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
            llvm::FunctionType* atofType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), atofArgs, false);
            atofFunc = llvm::Function::Create(atofType, llvm::Function::ExternalLinkage, "atof", module_.get());
        }
        std::vector<llvm::Value*> args = {value};
        return builder_->CreateCall(atofFunc, args);
    } else {
        // Tipo desconocido, intentar conversión por defecto
        return builder_->CreateBitCast(value, llvm::Type::getDoubleTy(*context_));
    }
}

llvm::Value* LLVMCodeGenerator::convertToString(llvm::Value* value) {
    llvm::Type* valueType = value->getType();
    
    if (valueType->isPointerTy()) {
        return value; // Ya es string (puntero)
    } else {
        // Para cualquier otro tipo, usar toStringLLVM
        return toStringLLVM(value);
    }
}

llvm::Value* LLVMCodeGenerator::convertToBool(llvm::Value* value) {
    llvm::Type* valueType = value->getType();
    
    if (valueType->isIntegerTy(1)) {
        return value; // Ya es bool
    } else if (valueType->isIntegerTy(32)) {
        auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        return builder_->CreateICmpNE(value, zero);
    } else if (valueType->isDoubleTy()) {
        auto zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
        return builder_->CreateFCmpUNE(value, zero);
    } else if (valueType->isPointerTy()) {
        // Para strings, verificar si no es null
        auto nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
        return builder_->CreateICmpNE(value, nullPtr);
    } else {
        // Para otros tipos, asumir true
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 1);
    }
}

llvm::Value* LLVMCodeGenerator::convertValue(llvm::Value* value, llvm::Type* targetType) {
    if (value->getType() == targetType) {
        return value; // No necesita conversión
    }
    
    if (targetType->isIntegerTy(32)) {
        return convertToInt(value);
    } else if (targetType->isDoubleTy()) {
        return convertToDouble(value);
    } else if (targetType->isIntegerTy(1)) {
        return convertToBool(value);
    } else if (targetType->isPointerTy()) {
        return convertToString(value);
    } else {
        // Para tipos desconocidos, intentar conversión por defecto
        return builder_->CreateBitCast(value, targetType);
    }
}

// ===== Mixed-type arithmetic operations =====

llvm::Value* LLVMCodeGenerator::boxed_add(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_add(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Check types of both operands and extract values
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* leftIsDouble = isDouble(left);
    llvm::Value* rightIsInt = isInt(right);
    llvm::Value* rightIsDouble = isDouble(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double", current_function_);
    llvm::BasicBlock* doubleIntBlock = llvm::BasicBlock::Create(*context_, "double_int", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock); // If left is not int, assume double
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int + int → int
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateAdd(leftIntVal, rightIntVal);
    builder_->CreateBr(mergeBlock);
    
    // int + double → double
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFAdd(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double + double (also handles double + int case for simplicity)
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFAdd(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Convert int result to double in the int block before branching
    builder_->SetInsertPoint(intIntBlock->getTerminator());
    llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
    
    // Merge results with PHI
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 3, "add_result");
    resultPhi->addIncoming(intResultAsDouble, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_add_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_add_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int + BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateAdd(nativeVal, boxedIntVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFAdd(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        // Convert int result to double in the int block before branching
        builder_->SetInsertPoint(intCaseBlock->getTerminator());
        llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "add_result");
        resultPhi->addIncoming(intResultAsDouble, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double + BoxedValue → always double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFAdd(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFAdd(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "add_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_add_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_add_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_add_native_right(BoxedValue*, native)" << std::endl;
    // Addition is commutative, so we can just swap the arguments
    return boxed_add_native_left(nativeVal, boxed);
}

llvm::Value* LLVMCodeGenerator::boxed_sub(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_sub(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Similar to boxed_add but with subtraction
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_sub", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_sub", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_sub", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_sub", current_function_);
    
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_sub", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int - int → int
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateSub(leftIntVal, rightIntVal);
    builder_->CreateBr(mergeBlock);
    
    // int - double → double
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFSub(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double - ? → double
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFSub(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Convert int result to double in the int block before branching
    builder_->SetInsertPoint(intIntBlock->getTerminator());
    llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
    
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 3, "sub_result");
    resultPhi->addIncoming(intResultAsDouble, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_sub_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_sub_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_sub", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_sub", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_sub", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateSub(nativeVal, boxedIntVal);
        // Convert int result to double in the int block before branching
        llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFSub(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "sub_result");
        resultPhi->addIncoming(intResultAsDouble, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFSub(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFSub(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "sub_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        return boxed_sub_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_sub_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_sub_native_right(BoxedValue*, native)" << std::endl;
    
    // Subtraction is NOT commutative, so we need separate implementation
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_sub_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_sub_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_sub_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateSub(boxedIntVal, nativeVal);
        // Convert int result to double in the int block before branching
        llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFSub(boxedDoubleVal, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "sub_result");
        resultPhi->addIncoming(intResultAsDouble, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFSub(boxedAsDouble, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFSub(boxedDoubleVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "sub_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        return boxed_sub_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

// TODO: Implement boxed_mul and boxed_div functions following the same pattern 

llvm::Value* LLVMCodeGenerator::boxed_mul(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_mul(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Check types of both operands
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_mul", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_mul", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_mul", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_mul", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_mul", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int * int → int (promoted to double for consistency)
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateMul(leftIntVal, rightIntVal);
    // Convert int result to double in the int block before branching
    llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
    builder_->CreateBr(mergeBlock);
    
    // int * double → double
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFMul(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double * ? → double
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFMul(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 3, "mul_result");
    resultPhi->addIncoming(intResultAsDouble, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_mul_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_mul_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_mul", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_mul", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_mul", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int * BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateMul(nativeVal, boxedIntVal);
        // Convert int result to double in the int block before branching
        llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFMul(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "mul_result");
        resultPhi->addIncoming(intResultAsDouble, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double * BoxedValue → always double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFMul(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFMul(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "mul_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_mul_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_mul_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_mul_native_right(BoxedValue*, native)" << std::endl;
    // Multiplication is commutative, so we can just swap the arguments
    return boxed_mul_native_left(nativeVal, boxed);
}

llvm::Value* LLVMCodeGenerator::boxed_div(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_div(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Division always returns double (to handle fractional results)
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_div", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_div", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_div", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_div", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_div", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int / int → double (always promote to avoid integer division)
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* rightAsDouble = builder_->CreateSIToFP(rightIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFDiv(leftAsDouble, rightAsDouble);
    builder_->CreateBr(mergeBlock);
    
    // int / double → double
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult2 = builder_->CreateFDiv(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double / ? → double
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult3 = builder_->CreateFDiv(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are double)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 3, "div_result");
    resultPhi->addIncoming(doubleResult1, intIntBlock);
    resultPhi->addIncoming(doubleResult2, intDoubleBlock);
    resultPhi->addIncoming(doubleResult3, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_div_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_div_native_left(native, BoxedValue*)" << std::endl;
    
    // Division always returns double
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_div", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_div", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_div", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int / BoxedValue → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFDiv(nativeAsDouble, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult2 = builder_->CreateFDiv(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "div_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double / BoxedValue → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFDiv(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFDiv(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "div_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_div_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_div_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_div_native_right(BoxedValue*, native)" << std::endl;
    
    // Division is NOT commutative, so we need separate implementation
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_div_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_div_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_div_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue / Native int → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFDiv(boxedAsDouble, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult2 = builder_->CreateFDiv(boxedDoubleVal, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "div_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue / Native double → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFDiv(boxedAsDouble, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFDiv(boxedDoubleVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "div_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_div_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

// Power operation functions
llvm::Value* LLVMCodeGenerator::boxed_pow(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_pow(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Power operation always returns double
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_pow", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_pow", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_pow", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_pow", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_pow", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // Create pow function declaration
    llvm::Function* powFunc = module_->getFunction("pow");
    if (!powFunc) {
        std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
        llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
        powFunc = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
    }
    
    // int ^ int → double
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* rightAsDouble = builder_->CreateSIToFP(rightIntVal, llvm::Type::getDoubleTy(*context_));
    std::vector<llvm::Value*> args1 = {leftAsDouble, rightAsDouble};
    llvm::Value* doubleResult1 = builder_->CreateCall(powFunc, args1);
    builder_->CreateBr(mergeBlock);
    
    // int ^ double → double
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    std::vector<llvm::Value*> args2 = {leftAsDouble, rightDoubleVal};
    llvm::Value* doubleResult2 = builder_->CreateCall(powFunc, args2);
    builder_->CreateBr(mergeBlock);
    
    // double ^ ? → double
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    std::vector<llvm::Value*> args3 = {leftDoubleVal, rightVal};
    llvm::Value* doubleResult3 = builder_->CreateCall(powFunc, args3);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are double)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 3, "pow_result");
    resultPhi->addIncoming(doubleResult1, intIntBlock);
    resultPhi->addIncoming(doubleResult2, intDoubleBlock);
    resultPhi->addIncoming(doubleResult3, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_pow_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_pow_native_left(native, BoxedValue*)" << std::endl;
    
    // Power operation always returns double
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_pow", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_pow", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_pow", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    // Create pow function declaration
    llvm::Function* powFunc = module_->getFunction("pow");
    if (!powFunc) {
        std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
        llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
        powFunc = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
    }
    
    if (nativeType->isIntegerTy(32)) {
        // Native int ^ BoxedValue → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args1 = {nativeAsDouble, boxedAsDouble};
        llvm::Value* doubleResult1 = builder_->CreateCall(powFunc, args1);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args2 = {nativeAsDouble, boxedDoubleVal};
        llvm::Value* doubleResult2 = builder_->CreateCall(powFunc, args2);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "pow_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double ^ BoxedValue → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args1 = {nativeVal, boxedAsDouble};
        llvm::Value* doubleResult1 = builder_->CreateCall(powFunc, args1);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        std::vector<llvm::Value*> args2 = {nativeVal, boxedDoubleVal};
        llvm::Value* doubleResult2 = builder_->CreateCall(powFunc, args2);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "pow_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_pow_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_pow_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_pow_native_right(BoxedValue*, native)" << std::endl;
    
    // Power operation is NOT commutative, so we need separate implementation
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_pow_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_pow_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_pow_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    // Create pow function declaration
    llvm::Function* powFunc = module_->getFunction("pow");
    if (!powFunc) {
        std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
        llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
        powFunc = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
    }
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue ^ Native int → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args1 = {boxedAsDouble, nativeAsDouble};
        llvm::Value* doubleResult1 = builder_->CreateCall(powFunc, args1);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args2 = {boxedDoubleVal, nativeAsDouble};
        llvm::Value* doubleResult2 = builder_->CreateCall(powFunc, args2);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "pow_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue ^ Native double → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args1 = {boxedAsDouble, nativeVal};
        llvm::Value* doubleResult1 = builder_->CreateCall(powFunc, args1);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        std::vector<llvm::Value*> args2 = {boxedDoubleVal, nativeVal};
        llvm::Value* doubleResult2 = builder_->CreateCall(powFunc, args2);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "pow_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_pow_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

// Modulo operation functions
llvm::Value* LLVMCodeGenerator::boxed_mod(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_mod(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Modulo operation handling depends on types
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_mod", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_mod", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_mod", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_mod", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_mod", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // Create fmod function declaration for floating point modulo
    llvm::Function* fmodFunc = module_->getFunction("fmod");
    if (!fmodFunc) {
        std::vector<llvm::Type*> fmodArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
        llvm::FunctionType* fmodType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), fmodArgs, false);
        fmodFunc = llvm::Function::Create(fmodType, llvm::Function::ExternalLinkage, "fmod", module_.get());
    }
    
    // int % int → int (promoted to double for consistency)
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateSRem(leftIntVal, rightIntVal);
    // Convert int result to double in the int block before branching
    llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
    builder_->CreateBr(mergeBlock);
    
    // int % double → double (use fmod)
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    std::vector<llvm::Value*> args1 = {leftAsDouble, rightDoubleVal};
    llvm::Value* doubleResult1 = builder_->CreateCall(fmodFunc, args1);
    builder_->CreateBr(mergeBlock);
    
    // double % ? → double (use fmod)
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    std::vector<llvm::Value*> args2 = {leftDoubleVal, rightVal};
    llvm::Value* doubleResult2 = builder_->CreateCall(fmodFunc, args2);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 3, "mod_result");
    resultPhi->addIncoming(intResultAsDouble, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_mod_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_mod_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_mod", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_mod", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_mod", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    // Create fmod function declaration for floating point modulo
    llvm::Function* fmodFunc = module_->getFunction("fmod");
    if (!fmodFunc) {
        std::vector<llvm::Type*> fmodArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
        llvm::FunctionType* fmodType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), fmodArgs, false);
        fmodFunc = llvm::Function::Create(fmodType, llvm::Function::ExternalLinkage, "fmod", module_.get());
    }
    
    if (nativeType->isIntegerTy(32)) {
        // Native int % BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateSRem(nativeVal, boxedIntVal);
        // Convert int result to double in the int block before branching
        llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args = {nativeAsDouble, boxedDoubleVal};
        llvm::Value* doubleResult = builder_->CreateCall(fmodFunc, args);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "mod_result");
        resultPhi->addIncoming(intResultAsDouble, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double % BoxedValue → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args1 = {nativeVal, boxedAsDouble};
        llvm::Value* doubleResult1 = builder_->CreateCall(fmodFunc, args1);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        std::vector<llvm::Value*> args2 = {nativeVal, boxedDoubleVal};
        llvm::Value* doubleResult2 = builder_->CreateCall(fmodFunc, args2);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "mod_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_mod_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_mod_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_mod_native_right(BoxedValue*, native)" << std::endl;
    
    // Modulo operation is NOT commutative, so we need separate implementation
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_mod_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_mod_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_mod_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    // Create fmod function declaration for floating point modulo
    llvm::Function* fmodFunc = module_->getFunction("fmod");
    if (!fmodFunc) {
        std::vector<llvm::Type*> fmodArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
        llvm::FunctionType* fmodType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), fmodArgs, false);
        fmodFunc = llvm::Function::Create(fmodType, llvm::Function::ExternalLinkage, "fmod", module_.get());
    }
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue % Native int
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateSRem(boxedIntVal, nativeVal);
        // Convert int result to double in the int block before branching
        llvm::Value* intResultAsDouble = builder_->CreateSIToFP(intResult, llvm::Type::getDoubleTy(*context_));
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args = {boxedDoubleVal, nativeAsDouble};
        llvm::Value* doubleResult = builder_->CreateCall(fmodFunc, args);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "mod_result");
        resultPhi->addIncoming(intResultAsDouble, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue % Native double → double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        std::vector<llvm::Value*> args1 = {boxedAsDouble, nativeVal};
        llvm::Value* doubleResult1 = builder_->CreateCall(fmodFunc, args1);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        std::vector<llvm::Value*> args2 = {boxedDoubleVal, nativeVal};
        llvm::Value* doubleResult2 = builder_->CreateCall(fmodFunc, args2);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "mod_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_mod_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

// Comparison operation functions
llvm::Value* LLVMCodeGenerator::boxed_greater(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_greater(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Greater than comparison returns i1 (boolean)
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_gt", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_gt", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_gt", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_gt", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_gt", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int > int → i1
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateICmpSGT(leftIntVal, rightIntVal);
    builder_->CreateBr(mergeBlock);
    
    // int > double → i1
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFCmpOGT(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double > ? → i1
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFCmpOGT(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are i1)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 3, "gt_result");
    resultPhi->addIncoming(intResult, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_lessthan(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_lessthan(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Less than comparison returns i1 (boolean)
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_lt", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_lt", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_lt", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_lt", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_lt", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int < int → i1
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateICmpSLT(leftIntVal, rightIntVal);
    builder_->CreateBr(mergeBlock);
    
    // int < double → i1
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFCmpOLT(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double < ? → i1
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFCmpOLT(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are i1)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 3, "lt_result");
    resultPhi->addIncoming(intResult, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_ge(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_ge(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Greater than or equal comparison returns i1 (boolean)
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_ge", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_ge", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_ge", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_ge", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_ge", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int >= int → i1
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateICmpSGE(leftIntVal, rightIntVal);
    builder_->CreateBr(mergeBlock);
    
    // int >= double → i1
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFCmpOGE(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double >= ? → i1
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFCmpOGE(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are i1)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 3, "ge_result");
    resultPhi->addIncoming(intResult, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_le(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_le(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Less than or equal comparison returns i1 (boolean)
    llvm::Value* leftIsInt = isInt(left);
    llvm::Value* rightIsInt = isInt(right);
    
    // Create basic blocks for different type combinations
    llvm::BasicBlock* intIntBlock = llvm::BasicBlock::Create(*context_, "int_int_le", current_function_);
    llvm::BasicBlock* intDoubleBlock = llvm::BasicBlock::Create(*context_, "int_double_le", current_function_);
    llvm::BasicBlock* doubleDoubleBlock = llvm::BasicBlock::Create(*context_, "double_double_le", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_le", current_function_);
    
    // Check left type first
    llvm::BasicBlock* checkRightBlock = llvm::BasicBlock::Create(*context_, "check_right_le", current_function_);
    builder_->CreateCondBr(leftIsInt, checkRightBlock, doubleDoubleBlock);
    
    // Check right type when left is int
    builder_->SetInsertPoint(checkRightBlock);
    builder_->CreateCondBr(rightIsInt, intIntBlock, intDoubleBlock);
    
    // int <= int → i1
    builder_->SetInsertPoint(intIntBlock);
    llvm::Value* leftIntVal = unboxInt(left);
    llvm::Value* rightIntVal = unboxInt(right);
    llvm::Value* intResult = builder_->CreateICmpSLE(leftIntVal, rightIntVal);
    builder_->CreateBr(mergeBlock);
    
    // int <= double → i1
    builder_->SetInsertPoint(intDoubleBlock);
    leftIntVal = unboxInt(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* leftAsDouble = builder_->CreateSIToFP(leftIntVal, llvm::Type::getDoubleTy(*context_));
    llvm::Value* doubleResult1 = builder_->CreateFCmpOLE(leftAsDouble, rightDoubleVal);
    builder_->CreateBr(mergeBlock);
    
    // double <= ? → i1
    builder_->SetInsertPoint(doubleDoubleBlock);
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightVal = builder_->CreateSelect(rightIsInt, 
        builder_->CreateSIToFP(unboxInt(right), llvm::Type::getDoubleTy(*context_)),
        unboxDouble(right));
    llvm::Value* doubleResult2 = builder_->CreateFCmpOLE(leftDoubleVal, rightVal);
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are i1)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 3, "le_result");
    resultPhi->addIncoming(intResult, intIntBlock);
    resultPhi->addIncoming(doubleResult1, intDoubleBlock);
    resultPhi->addIncoming(doubleResult2, doubleDoubleBlock);
    
    return resultPhi;
}

// Native left comparison functions
llvm::Value* LLVMCodeGenerator::boxed_greater_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_greater_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_gt", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_gt", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_gt", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int > BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSGT(nativeVal, boxedIntVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOGT(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "gt_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double > BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOGT(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOGT(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "gt_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_greater_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_lessthan_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_lessthan_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_lt", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_lt", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_lt", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int < BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSLT(nativeVal, boxedIntVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOLT(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "lt_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double < BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOLT(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOLT(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "lt_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_lessthan_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_ge_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_ge_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_ge", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_ge", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_ge", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int >= BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSGE(nativeVal, boxedIntVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOGE(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "ge_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double >= BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOGE(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOGE(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "ge_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_ge_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_le_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_le_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_le", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_le", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_le", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // Native int <= BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSLE(nativeVal, boxedIntVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOLE(nativeAsDouble, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "le_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // Native double <= BoxedValue
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOLE(nativeVal, boxedAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOLE(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "le_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_le_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

// Native right comparison functions
llvm::Value* LLVMCodeGenerator::boxed_greater_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_greater_native_right(BoxedValue*, native)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_gt_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_gt_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_gt_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue > Native int
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSGT(boxedIntVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOGT(boxedDoubleVal, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "gt_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue > Native double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOGT(boxedAsDouble, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOGT(boxedDoubleVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "gt_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_greater_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

llvm::Value* LLVMCodeGenerator::boxed_lessthan_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_lessthan_native_right(BoxedValue*, native)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_lt_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_lt_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_lt_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue < Native int
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSLT(boxedIntVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOLT(boxedDoubleVal, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "lt_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue < Native double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOLT(boxedAsDouble, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOLT(boxedDoubleVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "lt_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_lessthan_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

llvm::Value* LLVMCodeGenerator::boxed_ge_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_ge_native_right(BoxedValue*, native)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_ge_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_ge_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_ge_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue >= Native int
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSGE(boxedIntVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOGE(boxedDoubleVal, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "ge_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue >= Native double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOGE(boxedAsDouble, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOGE(boxedDoubleVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "ge_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_ge_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

llvm::Value* LLVMCodeGenerator::boxed_le_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_le_native_right(BoxedValue*, native)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    llvm::Value* boxedIsInt = isInt(boxed);
    
    // Create blocks for different cases
    llvm::BasicBlock* intCaseBlock = llvm::BasicBlock::Create(*context_, "int_case_le_right", current_function_);
    llvm::BasicBlock* doubleCaseBlock = llvm::BasicBlock::Create(*context_, "double_case_le_right", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_le_right", current_function_);
    
    builder_->CreateCondBr(boxedIsInt, intCaseBlock, doubleCaseBlock);
    
    if (nativeType->isIntegerTy(32)) {
        // BoxedValue <= Native int
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intResult = builder_->CreateICmpSLE(boxedIntVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult = builder_->CreateFCmpOLE(boxedDoubleVal, nativeAsDouble);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "le_result");
        resultPhi->addIncoming(intResult, intCaseBlock);
        resultPhi->addIncoming(doubleResult, doubleCaseBlock);
        return resultPhi;
    } else if (nativeType->isDoubleTy()) {
        // BoxedValue <= Native double
        builder_->SetInsertPoint(intCaseBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* boxedAsDouble = builder_->CreateSIToFP(boxedIntVal, llvm::Type::getDoubleTy(*context_));
        llvm::Value* doubleResult1 = builder_->CreateFCmpOLE(boxedAsDouble, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(doubleCaseBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleResult2 = builder_->CreateFCmpOLE(boxedDoubleVal, nativeVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "le_result");
        resultPhi->addIncoming(doubleResult1, intCaseBlock);
        resultPhi->addIncoming(doubleResult2, doubleCaseBlock);
        return resultPhi;
    } else {
        // Fallback: treat as int
        return boxed_le_native_right(boxed, builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)));
    }
}

// Equality operation functions
llvm::Value* LLVMCodeGenerator::boxed_equals(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_equals(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Equality comparison returns i1 (boolean)
    // First compare type tags, then values if types match
    
    // Get type tags
    llvm::Value* leftTypePtr = builder_->CreateStructGEP(getBoxedValueType(), left, 0, "left_type_ptr");
    llvm::Value* leftType = builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), leftTypePtr, "left_type");
    llvm::Value* rightTypePtr = builder_->CreateStructGEP(getBoxedValueType(), right, 0, "right_type_ptr");
    llvm::Value* rightType = builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), rightTypePtr, "right_type");
    
    // Compare type tags
    llvm::Value* sameType = builder_->CreateICmpEQ(leftType, rightType, "same_type");
    
    // Create basic blocks
    llvm::BasicBlock* sameTypeBlock = llvm::BasicBlock::Create(*context_, "same_type_eq", current_function_);
    llvm::BasicBlock* diffTypeBlock = llvm::BasicBlock::Create(*context_, "diff_type_eq", current_function_);
    llvm::BasicBlock* intEqBlock = llvm::BasicBlock::Create(*context_, "int_eq", current_function_);
    llvm::BasicBlock* doubleEqBlock = llvm::BasicBlock::Create(*context_, "double_eq", current_function_);
    llvm::BasicBlock* boolEqBlock = llvm::BasicBlock::Create(*context_, "bool_eq", current_function_);
    llvm::BasicBlock* stringEqBlock = llvm::BasicBlock::Create(*context_, "string_eq", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_eq", current_function_);
    
    // Branch based on type comparison
    builder_->CreateCondBr(sameType, sameTypeBlock, diffTypeBlock);
    
    // Different types → false
    builder_->SetInsertPoint(diffTypeBlock);
    llvm::Value* falseResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
    builder_->CreateBr(mergeBlock);
    
    // Same types → compare values based on type
    builder_->SetInsertPoint(sameTypeBlock);
    
    // Create default block for unknown types
    llvm::BasicBlock* unknownTypeBlock = llvm::BasicBlock::Create(*context_, "unknown_type_eq", current_function_);
    
    // Create switch on type (0=bool, 1=int, 2=double, 3=string)
    llvm::SwitchInst* typeSwitch = builder_->CreateSwitch(leftType, unknownTypeBlock, 4);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolEqBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), intEqBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), doubleEqBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), stringEqBlock);
    
    // Unknown type → false (should not happen with valid BoxedValues)
    builder_->SetInsertPoint(unknownTypeBlock);
    llvm::Value* unknownResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
    builder_->CreateBr(mergeBlock);
    
    // Boolean equality
    builder_->SetInsertPoint(boolEqBlock);
    llvm::Value* leftBool = unboxBool(left);
    llvm::Value* rightBool = unboxBool(right);
    llvm::Value* boolEqResult = builder_->CreateICmpEQ(leftBool, rightBool);
    builder_->CreateBr(mergeBlock);
    
    // Integer equality
    builder_->SetInsertPoint(intEqBlock);
    llvm::Value* leftInt = unboxInt(left);
    llvm::Value* rightInt = unboxInt(right);
    llvm::Value* intEqResult = builder_->CreateICmpEQ(leftInt, rightInt);
    builder_->CreateBr(mergeBlock);
    
    // Double equality
    builder_->SetInsertPoint(doubleEqBlock);
    llvm::Value* leftDouble = unboxDouble(left);
    llvm::Value* rightDouble = unboxDouble(right);
    llvm::Value* doubleEqResult = builder_->CreateFCmpOEQ(leftDouble, rightDouble);
    builder_->CreateBr(mergeBlock);
    
    // String equality
    builder_->SetInsertPoint(stringEqBlock);
    llvm::Value* leftString = unboxString(left);
    llvm::Value* rightString = unboxString(right);
    
    // Create strcmp function declaration
    llvm::Function* strcmpFunc = module_->getFunction("strcmp");
    if (!strcmpFunc) {
        std::vector<llvm::Type*> strcmpArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                               llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* strcmpType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), strcmpArgs, false);
        strcmpFunc = llvm::Function::Create(strcmpType, llvm::Function::ExternalLinkage, "strcmp", module_.get());
    }
    
    std::vector<llvm::Value*> strcmpArgs = {leftString, rightString};
    llvm::Value* strcmpResult = builder_->CreateCall(strcmpFunc, strcmpArgs);
    llvm::Value* stringEqResult = builder_->CreateICmpEQ(strcmpResult, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
    builder_->CreateBr(mergeBlock);
    
    // Merge results with PHI (all are i1)
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 6, "eq_result");
    resultPhi->addIncoming(falseResult, diffTypeBlock);
    resultPhi->addIncoming(unknownResult, unknownTypeBlock);
    resultPhi->addIncoming(boolEqResult, boolEqBlock);
    resultPhi->addIncoming(intEqResult, intEqBlock);
    resultPhi->addIncoming(doubleEqResult, doubleEqBlock);
    resultPhi->addIncoming(stringEqResult, stringEqBlock);
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_neq(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_neq(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Not equal is simply the negation of equals
    llvm::Value* equalsResult = boxed_equals(left, right);
    return builder_->CreateNot(equalsResult, "neq_result");
}

// Native left equality functions
llvm::Value* LLVMCodeGenerator::boxed_equals_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_equals_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    
    if (nativeType->isIntegerTy(32)) {
        // Native int == BoxedValue: only true if BoxedValue is int with same value
        llvm::Value* boxedIsInt = isInt(boxed);
        llvm::BasicBlock* isIntBlock = llvm::BasicBlock::Create(*context_, "is_int_eq", current_function_);
        llvm::BasicBlock* notIntBlock = llvm::BasicBlock::Create(*context_, "not_int_eq", current_function_);
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_eq", current_function_);
        
        builder_->CreateCondBr(boxedIsInt, isIntBlock, notIntBlock);
        
        builder_->SetInsertPoint(isIntBlock);
        llvm::Value* boxedIntVal = unboxInt(boxed);
        llvm::Value* intEqResult = builder_->CreateICmpEQ(nativeVal, boxedIntVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(notIntBlock);
        llvm::Value* falseResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "eq_result");
        resultPhi->addIncoming(intEqResult, isIntBlock);
        resultPhi->addIncoming(falseResult, notIntBlock);
        return resultPhi;
        
    } else if (nativeType->isDoubleTy()) {
        // Native double == BoxedValue: only true if BoxedValue is double with same value
        llvm::Value* boxedIsDouble = isDouble(boxed);
        llvm::BasicBlock* isDoubleBlock = llvm::BasicBlock::Create(*context_, "is_double_eq", current_function_);
        llvm::BasicBlock* notDoubleBlock = llvm::BasicBlock::Create(*context_, "not_double_eq", current_function_);
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_eq", current_function_);
        
        builder_->CreateCondBr(boxedIsDouble, isDoubleBlock, notDoubleBlock);
        
        builder_->SetInsertPoint(isDoubleBlock);
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
        llvm::Value* doubleEqResult = builder_->CreateFCmpOEQ(nativeVal, boxedDoubleVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(notDoubleBlock);
        llvm::Value* falseResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "eq_result");
        resultPhi->addIncoming(doubleEqResult, isDoubleBlock);
        resultPhi->addIncoming(falseResult, notDoubleBlock);
        return resultPhi;
        
    } else if (nativeType->isIntegerTy(1)) {
        // Native bool == BoxedValue: only true if BoxedValue is bool with same value
        llvm::Value* boxedIsBool = isBool(boxed);
        llvm::BasicBlock* isBoolBlock = llvm::BasicBlock::Create(*context_, "is_bool_eq", current_function_);
        llvm::BasicBlock* notBoolBlock = llvm::BasicBlock::Create(*context_, "not_bool_eq", current_function_);
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_eq", current_function_);
        
        builder_->CreateCondBr(boxedIsBool, isBoolBlock, notBoolBlock);
        
        builder_->SetInsertPoint(isBoolBlock);
        llvm::Value* boxedBoolVal = unboxBool(boxed);
        llvm::Value* boolEqResult = builder_->CreateICmpEQ(nativeVal, boxedBoolVal);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(notBoolBlock);
        llvm::Value* falseResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "eq_result");
        resultPhi->addIncoming(boolEqResult, isBoolBlock);
        resultPhi->addIncoming(falseResult, notBoolBlock);
        return resultPhi;
        
    } else if (nativeType->isPointerTy()) {
        // Native string (i8*) == BoxedValue: only true if BoxedValue is string with same content
        llvm::Value* boxedIsString = isString(boxed);
        llvm::BasicBlock* isStringBlock = llvm::BasicBlock::Create(*context_, "is_string_eq", current_function_);
        llvm::BasicBlock* notStringBlock = llvm::BasicBlock::Create(*context_, "not_string_eq", current_function_);
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_eq", current_function_);
        
        builder_->CreateCondBr(boxedIsString, isStringBlock, notStringBlock);
        
        builder_->SetInsertPoint(isStringBlock);
        llvm::Value* boxedStringVal = unboxString(boxed);
        
        // Create strcmp function declaration
        llvm::Function* strcmpFunc = module_->getFunction("strcmp");
        if (!strcmpFunc) {
            std::vector<llvm::Type*> strcmpArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                                   llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
            llvm::FunctionType* strcmpType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), strcmpArgs, false);
            strcmpFunc = llvm::Function::Create(strcmpType, llvm::Function::ExternalLinkage, "strcmp", module_.get());
        }
        
        std::vector<llvm::Value*> strcmpArgs = {nativeVal, boxedStringVal};
        llvm::Value* strcmpResult = builder_->CreateCall(strcmpFunc, strcmpArgs);
        llvm::Value* stringEqResult = builder_->CreateICmpEQ(strcmpResult, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(notStringBlock);
        llvm::Value* falseResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        builder_->CreateBr(mergeBlock);
        
        builder_->SetInsertPoint(mergeBlock);
        llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "eq_result");
        resultPhi->addIncoming(stringEqResult, isStringBlock);
        resultPhi->addIncoming(falseResult, notStringBlock);
        return resultPhi;
        
    } else {
        // Fallback: treat as int
        return boxed_equals_native_left(builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_)), boxed);
    }
}

llvm::Value* LLVMCodeGenerator::boxed_neq_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_neq_native_left(native, BoxedValue*)" << std::endl;
    
    // Not equal is simply the negation of equals
    llvm::Value* equalsResult = boxed_equals_native_left(nativeVal, boxed);
    return builder_->CreateNot(equalsResult, "neq_result");
}

// Native right equality functions
llvm::Value* LLVMCodeGenerator::boxed_equals_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_equals_native_right(BoxedValue*, native)" << std::endl;
    
    // Equality is commutative, so we can just swap the arguments
    return boxed_equals_native_left(nativeVal, boxed);
}

llvm::Value* LLVMCodeGenerator::boxed_neq_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_neq_native_right(BoxedValue*, native)" << std::endl;
    
    // Not equal is commutative, so we can just swap the arguments
    return boxed_neq_native_left(nativeVal, boxed);
}

// Helper function to convert BoxedValue to string
llvm::Value* LLVMCodeGenerator::boxedValueToString(llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxedValueToString(BoxedValue*)" << std::endl;
    
    // Get type tag
    llvm::Value* typePtr = builder_->CreateStructGEP(getBoxedValueType(), boxed, 0, "type_ptr");
    llvm::Value* typeTag = builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), typePtr, "type_tag");
    
    // Create basic blocks for different types
    llvm::BasicBlock* boolBlock = llvm::BasicBlock::Create(*context_, "bool_to_str", current_function_);
    llvm::BasicBlock* intBlock = llvm::BasicBlock::Create(*context_, "int_to_str", current_function_);
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "double_to_str", current_function_);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "string_to_str", current_function_);
    llvm::BasicBlock* unknownBlock = llvm::BasicBlock::Create(*context_, "unknown_to_str", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_to_str", current_function_);
    
    // Create switch on type (0=bool, 1=int, 2=double, 3=string)
    llvm::SwitchInst* typeSwitch = builder_->CreateSwitch(typeTag, unknownBlock, 4);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), intBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), doubleBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), stringBlock);
    
    // Boolean to string
    builder_->SetInsertPoint(boolBlock);
    llvm::Value* boolVal = unboxBool(boxed);
    llvm::Value* trueStr = registerStringConstant("true");
    llvm::Value* falseStr = registerStringConstant("false");
    llvm::Value* boolResult = builder_->CreateSelect(boolVal, trueStr, falseStr);
    builder_->CreateBr(mergeBlock);
    
    // Integer to string  
    builder_->SetInsertPoint(intBlock);
    llvm::Value* intVal = unboxInt(boxed);
    
    // Create sprintf function declaration
    llvm::Function* sprintfFunc = module_->getFunction("sprintf");
    if (!sprintfFunc) {
        std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
                                                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
        sprintfFunc = llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    }
    
    // Allocate buffer for integer string (32 chars should be enough)
    llvm::Value* intBuffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), 
                                                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 32), "int_buffer");
    llvm::Value* intFormat = registerStringConstant("%d");
    std::vector<llvm::Value*> intArgs = {intBuffer, intFormat, intVal};
    builder_->CreateCall(sprintfFunc, intArgs);
    builder_->CreateBr(mergeBlock);
    
    // Double to string
    builder_->SetInsertPoint(doubleBlock);
    llvm::Value* doubleVal = unboxDouble(boxed);
    
    // Allocate buffer for double string (64 chars should be enough)
    llvm::Value* doubleBuffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), 
                                                       llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 64), "double_buffer");
    llvm::Value* doubleFormat = registerStringConstant("%.6f");
    std::vector<llvm::Value*> doubleArgs = {doubleBuffer, doubleFormat, doubleVal};
    builder_->CreateCall(sprintfFunc, doubleArgs);
    builder_->CreateBr(mergeBlock);
    
    // String (already a string)
    builder_->SetInsertPoint(stringBlock);
    llvm::Value* stringResult = unboxString(boxed);
    builder_->CreateBr(mergeBlock);
    
    // Unknown type (fallback to empty string)
    builder_->SetInsertPoint(unknownBlock);
    llvm::Value* unknownResult = registerStringConstant("");
    builder_->CreateBr(mergeBlock);
    
    // Merge results
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 5, "str_result");
    resultPhi->addIncoming(boolResult, boolBlock);
    resultPhi->addIncoming(intBuffer, intBlock);
    resultPhi->addIncoming(doubleBuffer, doubleBlock);
    resultPhi->addIncoming(stringResult, stringBlock);
    resultPhi->addIncoming(unknownResult, unknownBlock);
    
    return resultPhi;
}

// Helper functions for converting specific types to strings
llvm::Value* LLVMCodeGenerator::intToString(llvm::Value* intVal) {
    std::cout << "[LLVM CodeGen] intToString()" << std::endl;
    
    // Create sprintf function declaration
    llvm::Function* sprintfFunc = module_->getFunction("sprintf");
    if (!sprintfFunc) {
        std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
                                                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
        sprintfFunc = llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    }
    
    // Allocate buffer for integer string (32 chars should be enough)
    llvm::Value* buffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), 
                                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 32), "int_str_buffer");
    llvm::Value* format = registerStringConstant("%d");
    std::vector<llvm::Value*> args = {buffer, format, intVal};
    builder_->CreateCall(sprintfFunc, args);
    
    return buffer;
}

llvm::Value* LLVMCodeGenerator::doubleToString(llvm::Value* doubleVal) {
    std::cout << "[LLVM CodeGen] doubleToString()" << std::endl;
    
    // Create sprintf function declaration
    llvm::Function* sprintfFunc = module_->getFunction("sprintf");
    if (!sprintfFunc) {
        std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
                                                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
        sprintfFunc = llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    }
    
    // Allocate buffer for double string (64 chars should be enough)
    llvm::Value* buffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), 
                                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 64), "double_str_buffer");
    llvm::Value* format = registerStringConstant("%.6f");
    std::vector<llvm::Value*> args = {buffer, format, doubleVal};
    builder_->CreateCall(sprintfFunc, args);
    
    return buffer;
}

llvm::Value* LLVMCodeGenerator::boolToString(llvm::Value* boolVal) {
    std::cout << "[LLVM CodeGen] boolToString()" << std::endl;
    
    // Create conditional string selection
    llvm::Value* trueStr = registerStringConstant("true");
    llvm::Value* falseStr = registerStringConstant("false");
    
    return builder_->CreateSelect(boolVal, trueStr, falseStr);
}

// Helper function to convert any native value to string
llvm::Value* LLVMCodeGenerator::nativeValueToString(llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] nativeValueToString()" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    
    if (nativeType->isIntegerTy(32)) {
        // Convert i32 to string
        std::cout << "[LLVM CodeGen] Converting i32 to string" << std::endl;
        return intToString(nativeVal);
    } else if (nativeType->isDoubleTy()) {
        // Convert double to string
        std::cout << "[LLVM CodeGen] Converting double to string" << std::endl;
        return doubleToString(nativeVal);
    } else if (nativeType->isIntegerTy(1)) {
        // Convert bool to string
        std::cout << "[LLVM CodeGen] Converting bool to string" << std::endl;
        return boolToString(nativeVal);
    } else if (nativeType->isPointerTy()) {
        // Check if it's a string pointer (i8*) - assume it's already a string
        std::cout << "[LLVM CodeGen] Value is already a string pointer" << std::endl;
        return nativeVal;
    } else if (nativeType->isFloatTy()) {
        // Convert float to double first, then to string
        std::cout << "[LLVM CodeGen] Converting float to string" << std::endl;
        llvm::Value* doubleVal = builder_->CreateFPExt(nativeVal, llvm::Type::getDoubleTy(*context_));
        return doubleToString(doubleVal);
    } else if (nativeType->isIntegerTy(8)) {
        // Convert i8 to i32 first, then to string
        std::cout << "[LLVM CodeGen] Converting i8 to string" << std::endl;
        llvm::Value* intVal = builder_->CreateZExt(nativeVal, llvm::Type::getInt32Ty(*context_));
        return intToString(intVal);
    } else if (nativeType->isIntegerTy(16)) {
        // Convert i16 to i32 first, then to string
        std::cout << "[LLVM CodeGen] Converting i16 to string" << std::endl;
        llvm::Value* intVal = builder_->CreateZExt(nativeVal, llvm::Type::getInt32Ty(*context_));
        return intToString(intVal);
    } else if (nativeType->isIntegerTy(64)) {
        // Convert i64 to i32 first, then to string (truncating)
        std::cout << "[LLVM CodeGen] Converting i64 to string" << std::endl;
        llvm::Value* intVal = builder_->CreateTrunc(nativeVal, llvm::Type::getInt32Ty(*context_));
        return intToString(intVal);
    } else {
        // Unknown type, return empty string
        std::cout << "[LLVM CodeGen] Unknown type, returning empty string" << std::endl;
        return registerStringConstant("");
    }
}

// Concatenation operation functions
llvm::Value* LLVMCodeGenerator::boxed_concat(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_concat(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Convert both BoxedValues to strings
    llvm::Value* leftStr = boxedValueToString(left);
    llvm::Value* rightStr = boxedValueToString(right);
    
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
    llvm::Value* bufferSize = builder_->CreateAdd(totalLen, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1)); // +1 for null terminator
    
    // Allocate result buffer
    llvm::Value* resultBuffer = builder_->CreateAlloca(llvm::Type::getInt8Ty(*context_), bufferSize, "concat_result");
    
    // Create strcpy and strcat function declarations
    llvm::Function* strcpyFunc = module_->getFunction("strcpy");
    if (!strcpyFunc) {
        std::vector<llvm::Type*> strcpyArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                               llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
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
    
    // Perform concatenation
    builder_->CreateCall(strcpyFunc, {resultBuffer, leftStr});
    builder_->CreateCall(strcatFunc, {resultBuffer, rightStr});
    
    // Create BoxedValue from the concatenated string
    llvm::Value* boxedResult = createBoxedFromString(resultBuffer);
    return boxedResult;
}

llvm::Value* LLVMCodeGenerator::boxed_concat_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_concat_native_left(native, BoxedValue*)" << std::endl;
    
    // Convert BoxedValue to string
    llvm::Value* rightStr = boxedValueToString(boxed);
    
    // Convert native value to string
    llvm::Value* leftStr = nativeValueToString(nativeVal);
    
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
    
    // Create strcpy and strcat function declarations
    llvm::Function* strcpyFunc = module_->getFunction("strcpy");
    if (!strcpyFunc) {
        std::vector<llvm::Type*> strcpyArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                               llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
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
    
    // Perform concatenation
    builder_->CreateCall(strcpyFunc, {resultBuffer, leftStr});
    builder_->CreateCall(strcatFunc, {resultBuffer, rightStr});
    
    // Create BoxedValue from the concatenated string
    llvm::Value* boxedResult = createBoxedFromString(resultBuffer);
    return boxedResult;
}

llvm::Value* LLVMCodeGenerator::boxed_concat_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_concat_native_right(BoxedValue*, native)" << std::endl;
    
    // Convert BoxedValue to string
    llvm::Value* leftStr = boxedValueToString(boxed);
    
    // Convert native value to string
    llvm::Value* rightStr = nativeValueToString(nativeVal);
    
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
    
    // Create strcpy and strcat function declarations
    llvm::Function* strcpyFunc = module_->getFunction("strcpy");
    if (!strcpyFunc) {
        std::vector<llvm::Type*> strcpyArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                               llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
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
    
    // Perform concatenation
    builder_->CreateCall(strcpyFunc, {resultBuffer, leftStr});
    builder_->CreateCall(strcatFunc, {resultBuffer, rightStr});
    
    // Create BoxedValue from the concatenated string
    llvm::Value* boxedResult = createBoxedFromString(resultBuffer);
    return boxedResult;
}

// Helper function to convert BoxedValue to boolean
llvm::Value* LLVMCodeGenerator::boxedValueToBool(llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxedValueToBool(BoxedValue*)" << std::endl;
    
    // Get type tag
    llvm::Value* typePtr = builder_->CreateStructGEP(getBoxedValueType(), boxed, 0, "type_ptr");
    llvm::Value* typeTag = builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), typePtr, "type_tag");
    
    // Create basic blocks for different types
    llvm::BasicBlock* boolBlock = llvm::BasicBlock::Create(*context_, "bool_to_bool", current_function_);
    llvm::BasicBlock* intBlock = llvm::BasicBlock::Create(*context_, "int_to_bool", current_function_);
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "double_to_bool", current_function_);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "string_to_bool", current_function_);
    llvm::BasicBlock* unknownBlock = llvm::BasicBlock::Create(*context_, "unknown_to_bool", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_to_bool", current_function_);
    
    // Create switch on type (0=bool, 1=int, 2=double, 3=string)
    llvm::SwitchInst* typeSwitch = builder_->CreateSwitch(typeTag, unknownBlock, 4);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), intBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), doubleBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 3), stringBlock);
    
    // Boolean (direct value)
    builder_->SetInsertPoint(boolBlock);
    llvm::Value* boolResult = unboxBool(boxed);
    builder_->CreateBr(mergeBlock);
    
    // Integer (false if 0, true if != 0)
    builder_->SetInsertPoint(intBlock);
    llvm::Value* intVal = unboxInt(boxed);
    llvm::Value* intResult = builder_->CreateICmpNE(intVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
    builder_->CreateBr(mergeBlock);
    
    // Double (false if 0.0, true if != 0.0)
    builder_->SetInsertPoint(doubleBlock);
    llvm::Value* doubleVal = unboxDouble(boxed);
    llvm::Value* doubleResult = builder_->CreateFCmpONE(doubleVal, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0));
    builder_->CreateBr(mergeBlock);
    
    // String (false if empty, true if not empty)
    builder_->SetInsertPoint(stringBlock);
    llvm::Value* stringVal = unboxString(boxed);
    
    // Get string length
    llvm::Function* strlenFunc = module_->getFunction("strlen");
    if (!strlenFunc) {
        std::vector<llvm::Type*> strlenArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* strlenType = llvm::FunctionType::get(llvm::Type::getInt64Ty(*context_), strlenArgs, false);
        strlenFunc = llvm::Function::Create(strlenType, llvm::Function::ExternalLinkage, "strlen", module_.get());
    }
    
    llvm::Value* stringLen = builder_->CreateCall(strlenFunc, {stringVal});
    llvm::Value* stringResult = builder_->CreateICmpNE(stringLen, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0));
    builder_->CreateBr(mergeBlock);
    
    // Unknown type (default to false)
    builder_->SetInsertPoint(unknownBlock);
    llvm::Value* unknownResult = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
    builder_->CreateBr(mergeBlock);
    
    // Merge results
    builder_->SetInsertPoint(mergeBlock);
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 5, "bool_result");
    resultPhi->addIncoming(boolResult, boolBlock);
    resultPhi->addIncoming(intResult, intBlock);
    resultPhi->addIncoming(doubleResult, doubleBlock);
    resultPhi->addIncoming(stringResult, stringBlock);
    resultPhi->addIncoming(unknownResult, unknownBlock);
    
    return resultPhi;
}

// Logical operation functions with short-circuit evaluation
llvm::Value* LLVMCodeGenerator::boxed_and(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_and(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Convert left operand to boolean
    llvm::Value* leftBool = boxedValueToBool(left);
    
    // Create basic blocks for short-circuit evaluation
    llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_and", current_function_);
    llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_and", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_and", current_function_);
    
    // If left is false, short-circuit to false; otherwise evaluate right
    builder_->CreateCondBr(leftBool, evalRightBlock, shortCircuitBlock);
    
    // Evaluate right operand
    builder_->SetInsertPoint(evalRightBlock);
    llvm::Value* rightBool = boxedValueToBool(right);
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
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_or(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_or(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Convert left operand to boolean
    llvm::Value* leftBool = boxedValueToBool(left);
    
    // Create basic blocks for short-circuit evaluation
    llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_or", current_function_);
    llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_or", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_or", current_function_);
    
    // If left is true, short-circuit to true; otherwise evaluate right
    builder_->CreateCondBr(leftBool, shortCircuitBlock, evalRightBlock);
    
    // Evaluate right operand
    builder_->SetInsertPoint(evalRightBlock);
    llvm::Value* rightBool = boxedValueToBool(right);
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
    
    return resultPhi;
}

// Native left logical functions
llvm::Value* LLVMCodeGenerator::boxed_and_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_and_native_left(native, BoxedValue*)" << std::endl;
    
    // Convert native value to boolean
    llvm::Value* leftBool = convertToBool(nativeVal);
    
    // Create basic blocks for short-circuit evaluation
    llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_and_nat", current_function_);
    llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_and_nat", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_and_nat", current_function_);
    
    // If left is false, short-circuit to false; otherwise evaluate right
    builder_->CreateCondBr(leftBool, evalRightBlock, shortCircuitBlock);
    
    // Evaluate right operand
    builder_->SetInsertPoint(evalRightBlock);
    llvm::Value* rightBool = boxedValueToBool(boxed);
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
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_or_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_or_native_left(native, BoxedValue*)" << std::endl;
    
    // Convert native value to boolean
    llvm::Value* leftBool = convertToBool(nativeVal);
    
    // Create basic blocks for short-circuit evaluation
    llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_or_nat", current_function_);
    llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_or_nat", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_or_nat", current_function_);
    
    // If left is true, short-circuit to true; otherwise evaluate right
    builder_->CreateCondBr(leftBool, shortCircuitBlock, evalRightBlock);
    
    // Evaluate right operand
    builder_->SetInsertPoint(evalRightBlock);
    llvm::Value* rightBool = boxedValueToBool(boxed);
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
    
    return resultPhi;
}

// Native right logical functions
llvm::Value* LLVMCodeGenerator::boxed_and_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_and_native_right(BoxedValue*, native)" << std::endl;
    
    // Convert left operand (BoxedValue) to boolean
    llvm::Value* leftBool = boxedValueToBool(boxed);
    
    // Create basic blocks for short-circuit evaluation
    llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_and_nat_r", current_function_);
    llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_and_nat_r", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_and_nat_r", current_function_);
    
    // If left is false, short-circuit to false; otherwise evaluate right
    builder_->CreateCondBr(leftBool, evalRightBlock, shortCircuitBlock);
    
    // Evaluate right operand
    builder_->SetInsertPoint(evalRightBlock);
    llvm::Value* rightBool = convertToBool(nativeVal);
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
    
    return resultPhi;
}

llvm::Value* LLVMCodeGenerator::boxed_or_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_or_native_right(BoxedValue*, native)" << std::endl;
    
    // Convert left operand (BoxedValue) to boolean
    llvm::Value* leftBool = boxedValueToBool(boxed);
    
    // Create basic blocks for short-circuit evaluation
    llvm::BasicBlock* evalRightBlock = llvm::BasicBlock::Create(*context_, "eval_right_or_nat_r", current_function_);
    llvm::BasicBlock* shortCircuitBlock = llvm::BasicBlock::Create(*context_, "short_circuit_or_nat_r", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_or_nat_r", current_function_);
    
    // If left is true, short-circuit to true; otherwise evaluate right
    builder_->CreateCondBr(leftBool, shortCircuitBlock, evalRightBlock);
    
    // Evaluate right operand
    builder_->SetInsertPoint(evalRightBlock);
    llvm::Value* rightBool = convertToBool(nativeVal);
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
    
    return resultPhi;
}