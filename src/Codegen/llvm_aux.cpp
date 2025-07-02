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
    
    // Create print_boxed function
    createPrintBoxedFunction();
    
    // Create type check functions
    createTypeCheckFunctions();
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
    
    // More robust check: is it a pointer to BoxedValue?
    bool isBoxedValue = false;
    if (auto ptrType = llvm::dyn_cast<llvm::PointerType>(argType)) {
        if (ptrType->getNumContainedTypes() > 0) {
            llvm::Type* elementType = ptrType->getContainedType(0);
            std::cout << "[LLVM CodeGen] Pointer element type: ";
            elementType->print(llvm::outs());
            std::cout << std::endl;
            
            // Check if it's a pointer to BoxedValue struct by name
            if (elementType->isStructTy()) {
                std::string structName = elementType->getStructName().str();
                std::cout << "[LLVM CodeGen] Struct name: " << structName << std::endl;
                isBoxedValue = (structName == "BoxedValue");
                std::cout << "[LLVM CodeGen] Is BoxedValue struct: " << isBoxedValue << std::endl;
            }
        }
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
    llvm::BasicBlock* numberBlock = llvm::BasicBlock::Create(*context_, "print_number", printBoxedFunc);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "print_string", printBoxedFunc);
    llvm::BasicBlock* boolBlock = llvm::BasicBlock::Create(*context_, "print_bool", printBoxedFunc);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge", printBoxedFunc);
    
    // Switch on type tag (similar to script.hulk.ll)
    llvm::SwitchInst* switchInst = printBuilder.CreateSwitch(typeTag, numberBlock, 4);
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);   // bool
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), numberBlock); // int
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), numberBlock); // double
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
    
    // Print number (int or double)
    printBuilder.SetInsertPoint(numberBlock);
    llvm::Value* numDataPtr = printBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    numDataPtr = printBuilder.CreatePointerCast(numDataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    llvm::Value* numValue = printBuilder.CreateLoad(llvm::Type::getDoubleTy(*context_), numDataPtr);
    
    llvm::Value* numFormatStr = registerStringConstant("%f\n");
    auto numFormatPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(
        llvm::cast<llvm::GlobalVariable>(numFormatStr)->getValueType(), 
        llvm::cast<llvm::GlobalVariable>(numFormatStr), 
        llvm::ArrayRef<llvm::Constant*>{zero, zero}
    );
    std::vector<llvm::Value*> numArgs = {numFormatPtr, numValue};
    printBuilder.CreateCall(printfFunc, numArgs);
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