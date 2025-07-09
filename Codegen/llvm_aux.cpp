#include "llvm_codegen.hpp"
#include <algorithm>
#include <set>
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
        // Object type is unknown at compile time, use BoxedValue*
        return llvm::PointerType::get(getBoxedValueType(), 0);
    case TypeInfo::Kind::Unknown:
        // Unknown type should be handled as BoxedValue* for dynamic typing
        return llvm::PointerType::get(getBoxedValueType(), 0);
    default:
        // Any other unknown type should also be BoxedValue*
        return llvm::PointerType::get(getBoxedValueType(), 0);
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

int LLVMCodeGenerator::getAttributeIndex(llvm::StructType* structType, const std::string& attrName)
{
    // Find the type name from the struct
    std::string typeName = "";
    for (const auto& typeEntry : types_) {
        if (typeEntry.second == structType) {
            typeName = typeEntry.first;
            break;
        }
    }
    
    if (typeName.empty()) {
        std::cerr << "[LLVM CodeGen] Error: Could not find type name for struct" << std::endl;
        return -1;
    }
    
    // Look up attribute index
    auto typeIt = type_attributes_.find(typeName);
    if (typeIt == type_attributes_.end()) {
        std::cerr << "[LLVM CodeGen] Error: No attribute metadata for type " << typeName << std::endl;
        return -1;
    }
    
    auto attrIt = typeIt->second.find(attrName);
    if (attrIt == typeIt->second.end()) {
        std::cerr << "[LLVM CodeGen] Error: Attribute " << attrName << " not found in type " << typeName << std::endl;
        return -1;
    }
    
    return attrIt->second;
}

void LLVMCodeGenerator::storeTypeMetadata(const std::string& typeName, const std::vector<std::string>& attrNames)
{
    std::unordered_map<std::string, int> attributes;
    
    // Check if this type has inheritance (if it has a base pointer field)
    llvm::StructType* structType = types_[typeName];
    int startIndex = 0;
    
    // If the struct has fields and the first field looks like a base pointer, skip it
    if (structType && structType->getNumElements() > 0) {
        llvm::Type* firstField = structType->getElementType(0);
        if (firstField->isPointerTy() && attrNames.size() < structType->getNumElements()) {
            startIndex = 1; // Skip base pointer
        }
    }
    
    int index = startIndex;
    for (const auto& attrName : attrNames) {
        attributes[attrName] = index;
        index++;
        std::cout << "[LLVM CodeGen] Stored attribute " << attrName << " at index " << (index-1) << " for type " << typeName << std::endl;
    }
    
    type_attributes_[typeName] = attributes;
}

std::string LLVMCodeGenerator::getObjectTypeName(llvm::Value* objectPtr)
{
    if (!objectPtr || !objectPtr->getType()->isPointerTy()) {
        return "";
    }
    
    // Method 0: Check our runtime type tracking first (LLVM 19 compatible)
    std::string trackedType = getTrackedValueType(objectPtr);
    if (!trackedType.empty()) {
        std::cout << "[LLVM CodeGen] Found tracked type: " << trackedType << std::endl;
        return trackedType;
    }
    
    // Method 1: Check if this is a constructor call
    if (auto callInst = llvm::dyn_cast<llvm::CallInst>(objectPtr)) {
        llvm::Function* calledFunc = callInst->getCalledFunction();
        if (calledFunc) {
            std::string funcName = calledFunc->getName().str();
            // If it's a constructor call like "Point_constructor", extract "Point"
            if (funcName.length() > 12 && funcName.substr(funcName.length() - 12) == "_constructor") {
                std::string typeName = funcName.substr(0, funcName.length() - 12);
                std::cout << "[LLVM CodeGen] Determined type from constructor call: " << typeName << std::endl;
                // Track this type for future reference
                trackValueType(objectPtr, typeName);
                return typeName;
            }
        }
    }
    
    // Method 2: Check if this is a variable load
    if (auto loadInst = llvm::dyn_cast<llvm::LoadInst>(objectPtr)) {
        llvm::Value* allocaPtr = loadInst->getPointerOperand();
        if (auto allocaInst = llvm::dyn_cast<llvm::AllocaInst>(allocaPtr)) {
            std::string varName = allocaInst->getName().str();
            
            // First, check our global variable type tracking
            std::string trackedVarType = getTrackedVariableType(varName);
            if (!trackedVarType.empty()) {
                std::cout << "[LLVM CodeGen] Found tracked variable type: " << trackedVarType << std::endl;
                trackValueType(objectPtr, trackedVarType);
                return trackedVarType;
            }
            
            // Check if we have type information for this variable in current scope
            if (current_scope_) {
                // First, check if we have explicit object type information
                auto objTypeIt = current_scope_->variable_object_types.find(varName);
                if (objTypeIt != current_scope_->variable_object_types.end()) {
                    std::cout << "[LLVM CodeGen] Found object type in variable_object_types: " << objTypeIt->second << std::endl;
                    trackValueType(objectPtr, objTypeIt->second);
                    trackVariableType(varName, objTypeIt->second);
                    return objTypeIt->second;
                }
                
                // Then check the general type information
                auto typeIt = current_scope_->variable_types.find(varName);
                if (typeIt != current_scope_->variable_types.end()) {
                    // Extract type name from the LLVM type
                    llvm::Type* llvmType = typeIt->second;
                    if (llvmType->isPointerTy()) {
                        // For struct types, try to extract the type name
                        std::string typeName = extractTypeNameFromLLVMType(llvmType);
                        if (!typeName.empty()) {
                            std::cout << "[LLVM CodeGen] Determined type from variable: " << typeName << std::endl;
                            trackValueType(objectPtr, typeName);
                            trackVariableType(varName, typeName);
                            return typeName;
                        }
                    }
                }
            }
            
            // Fallback: try to infer from variable name patterns
            if (varName.find("_obj") != std::string::npos) {
                // Look for patterns like "Point_obj", "Person_obj", etc.
                size_t pos = varName.find("_obj");
                if (pos > 0) {
                    std::string typeName = varName.substr(0, pos);
                    std::cout << "[LLVM CodeGen] Inferred type from variable name: " << typeName << std::endl;
                    return typeName;
                }
            }
        }
    }
    
    // Method 3: Check if this is a method call result
    if (auto callInst = llvm::dyn_cast<llvm::CallInst>(objectPtr)) {
        llvm::Function* calledFunc = callInst->getCalledFunction();
        if (calledFunc) {
            std::string funcName = calledFunc->getName().str();
            
            // Check if this is a method call that returns an object
            size_t underscorePos = funcName.find('_');
            if (underscorePos != std::string::npos) {
                std::string potentialType = funcName.substr(0, underscorePos);
                
                // Verify this is a known type
                if (types_.find(potentialType) != types_.end()) {
                    std::cout << "[LLVM CodeGen] Determined type from method call: " << potentialType << std::endl;
                    return potentialType;
                }
            }
        }
    }
    
    // Method 4: Check if this is a struct GEP operation
    if (auto gepInst = llvm::dyn_cast<llvm::GetElementPtrInst>(objectPtr)) {
        // This might be accessing a field, try to get the base object type
        llvm::Value* basePtr = gepInst->getPointerOperand();
        return getObjectTypeName(basePtr); // Recursive call
    }
    
    // Method 5: Check if this is a bitcast operation
    if (auto bitcastInst = llvm::dyn_cast<llvm::BitCastInst>(objectPtr)) {
        llvm::Value* sourcePtr = bitcastInst->getOperand(0);
        return getObjectTypeName(sourcePtr); // Recursive call
    }
    
    // Method 6: Try to extract type from instruction name/metadata
    if (auto inst = llvm::dyn_cast<llvm::Instruction>(objectPtr)) {
        std::string instName = inst->getName().str();
        if (!instName.empty()) {
            // Look for type patterns in instruction names
            for (const auto& typePair : types_) {
                if (instName.find(typePair.first) != std::string::npos) {
                    std::cout << "[LLVM CodeGen] Inferred type from instruction name: " << typePair.first << std::endl;
                    return typePair.first;
                }
            }
        }
    }
    
    // If we can't determine the type, return empty string
    std::cout << "[LLVM CodeGen] Could not determine object type name for value" << std::endl;
    return "";
}

std::string LLVMCodeGenerator::extractTypeNameFromLLVMType(llvm::Type* llvmType)
{
    if (!llvmType->isPointerTy()) {
        return "";
    }
    
    // For opaque pointers in LLVM 19, we need to use other methods
    // Try to match against known struct types
    for (const auto& typePair : types_) {
        llvm::Type* knownType = llvm::PointerType::get(typePair.second, 0);
        if (llvmType == knownType) {
            return typePair.first;
        }
    }
    
    return "";
}

void LLVMCodeGenerator::addBaseTypeFields(const std::string& baseTypeName, std::vector<llvm::Type*>& fieldTypes, std::vector<std::string>& attributeNames)
{
    if (baseTypeName == "Object") {
        return; // Object has no fields beyond vtable
    }
    
    // Get base type declaration
    auto baseIt = type_declarations_.find(baseTypeName);
    if (baseIt == type_declarations_.end()) {
        std::cerr << "[LLVM CodeGen] Error: Base type " << baseTypeName << " not found in declarations" << std::endl;
        return;
    }
    
    TypeDecl* baseDecl = baseIt->second;
    
    // Recursively add fields from base's base type
    if (baseDecl->baseType != "Object") {
        addBaseTypeFields(baseDecl->baseType, fieldTypes, attributeNames);
    }
    
    // Add base type's own attributes
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    
    for (const auto& attr : baseDecl->attributes) {
        fieldTypes.push_back(boxedPtrTy);
        attributeNames.push_back(attr->name);
        std::cout << "[LLVM CodeGen] Added inherited field: " << attr->name << " from " << baseTypeName << std::endl;
    }
}

std::vector<std::string> LLVMCodeGenerator::getInheritanceChain(const std::string& typeName)
{
    std::vector<std::string> chain;
    std::string current = typeName;
    
    while (current != "Object") {
        chain.push_back(current);
        
        auto it = type_declarations_.find(current);
        if (it == type_declarations_.end()) {
            break;
        }
        
        current = it->second->baseType;
    }
    
    return chain;
}

std::vector<std::string> LLVMCodeGenerator::collectAllMethods(TypeDecl* typeDecl)
{
    std::vector<std::string> allMethods;
    std::set<std::string> methodNames; // Para evitar duplicados
    
    // Collect methods from inheritance chain (bottom-up)
    std::vector<std::string> chain = getInheritanceChain(typeDecl->name);
    
    // Reverse to go from base to derived
    std::reverse(chain.begin(), chain.end());
    
    std::cout << "[LLVM CodeGen] Collecting methods for " << typeDecl->name << " from inheritance chain: ";
    for (const auto& t : chain) {
        std::cout << t << " ";
    }
    std::cout << std::endl;
    
    for (const std::string& typeName : chain) {
        auto it = type_declarations_.find(typeName);
        if (it != type_declarations_.end()) {
            std::cout << "[LLVM CodeGen] Processing methods from type: " << typeName << std::endl;
            for (const auto& method : it->second->methods) {
                if (methodNames.find(method->name) == methodNames.end()) {
                    // New method - add to end
                    allMethods.push_back(method->name);
                    methodNames.insert(method->name);
                    std::cout << "[LLVM CodeGen] Added new method: " << method->name << " at index " << (allMethods.size() - 1) << std::endl;
                } else {
                    // Method override - keep same position for vtable compatibility
                    auto pos = std::find(allMethods.begin(), allMethods.end(), method->name);
                    if (pos != allMethods.end()) {
                        std::cout << "[LLVM CodeGen] Method " << method->name << " overridden in " << typeName << " (keeping same vtable position)" << std::endl;
                    }
                }
            }
        }
    }
    
    std::cout << "[LLVM CodeGen] Final method list for " << typeDecl->name << ": ";
    for (size_t i = 0; i < allMethods.size(); i++) {
        std::cout << i << ":" << allMethods[i] << " ";
    }
    std::cout << std::endl;
    
    return allMethods;
}

void LLVMCodeGenerator::generateVTable(TypeDecl* typeDecl)
{
    std::cout << "[LLVM CodeGen] Generating vtable for type: " << typeDecl->name << std::endl;
    
    // Collect all methods for this type (including inherited)
    std::vector<std::string> allMethods = collectAllMethods(typeDecl);
    
    // Store method list for this type
    type_methods_[typeDecl->name] = allMethods;
    
    // Create vtable type
    std::vector<llvm::Type*> methodPtrTypes;
    for (const std::string& methodName : allMethods) {
        // All methods return BoxedValue* and take (self, ...params) as BoxedValue*
        std::vector<llvm::Type*> params = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)}; // self
        // Add actual method parameters (simplified for now)
        llvm::FunctionType* methodType = llvm::FunctionType::get(
            llvm::PointerType::get(getBoxedValueType(), 0), // return BoxedValue*
            params, 
            true // variadic for simplicity
        );
        methodPtrTypes.push_back(llvm::PointerType::get(methodType, 0));
    }
    
    if (methodPtrTypes.empty()) {
        // No methods, create empty vtable
        methodPtrTypes.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    }
    
    llvm::ArrayType* vtableType = llvm::ArrayType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
        methodPtrTypes.size()
    );
    
    // Create vtable initializer
    std::vector<llvm::Constant*> methodPointers;
    for (const std::string& methodName : allMethods) {
        llvm::Function* methodFunc = nullptr;
        std::string functionName;
        
        // Try to find the method in current type first
        functionName = typeDecl->name + "_" + methodName;
        methodFunc = module_->getFunction(functionName);
        
        if (!methodFunc) {
            // Method not found in current type, search in inheritance chain
            std::vector<std::string> chain = getInheritanceChain(typeDecl->name);
            // Reverse to go from base to derived (excluding current type which we already checked)
            std::reverse(chain.begin(), chain.end());
            
            for (const std::string& ancestorType : chain) {
                if (ancestorType == typeDecl->name) continue; // Skip current type
                
                functionName = ancestorType + "_" + methodName;
                methodFunc = module_->getFunction(functionName);
                if (methodFunc) {
                    std::cout << "[LLVM CodeGen] Found inherited method " << functionName << " for " << typeDecl->name << std::endl;
                    break;
                }
            }
        }
        
        if (methodFunc) {
            llvm::Constant* funcPtr = llvm::ConstantExpr::getBitCast(
                methodFunc, 
                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)
            );
            methodPointers.push_back(funcPtr);
            std::cout << "[LLVM CodeGen] Added method " << functionName << " to vtable for " << typeDecl->name << std::endl;
        } else {
            // Method not found in entire hierarchy, use null pointer
            methodPointers.push_back(llvm::ConstantPointerNull::get(
                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)
            ));
            std::cerr << "[LLVM CodeGen] Error: Method " << methodName << " not found in entire hierarchy for " << typeDecl->name << std::endl;
        }
    }
    
    if (methodPointers.empty()) {
        methodPointers.push_back(llvm::ConstantPointerNull::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)
        ));
    }
    
    llvm::Constant* vtableInit = llvm::ConstantArray::get(vtableType, methodPointers);
    
    // Create global vtable
    std::string vtableName = typeDecl->name + "_vtable";
    llvm::GlobalVariable* vtable = new llvm::GlobalVariable(
        *module_,
        vtableType,
        true,  // isConstant
        llvm::GlobalValue::ExternalLinkage,
        vtableInit,
        vtableName
    );
    
    vtables_[typeDecl->name] = vtable;
    
    std::cout << "[LLVM CodeGen] Created vtable " << vtableName << " with " << allMethods.size() << " methods" << std::endl;
}

llvm::GlobalVariable* LLVMCodeGenerator::getOrCreateVTable(const std::string& typeName)
{
    auto it = vtables_.find(typeName);
    if (it != vtables_.end()) {
        return it->second;
    }
    
    std::cerr << "[LLVM CodeGen] Error: VTable for type " << typeName << " not found" << std::endl;
    return nullptr;
}

void LLVMCodeGenerator::generateConstructorFunction(TypeDecl *typeDecl)
{
    std::cout << "[LLVM CodeGen] Generating constructor function for type: " << typeDecl->name << std::endl;
    
    // Get the struct type
    llvm::StructType* structType = getOrCreateStructType(typeDecl->name);
    
    // Create constructor function signature - all parameters are BoxedValue*
    std::vector<llvm::Type*> paramTypes;
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    
    for (size_t i = 0; i < typeDecl->paramTypes.size(); i++) {
        // All constructor parameters for user-defined types are BoxedValue*
        paramTypes.push_back(boxedPtrTy);
    }
    
    llvm::FunctionType* constructorType = llvm::FunctionType::get(
        llvm::PointerType::get(structType, 0), 
        paramTypes, 
        false
    );
    
    std::string constructorName = typeDecl->name + "_constructor";
    llvm::Function* constructor = llvm::Function::Create(
        constructorType,
        llvm::Function::ExternalLinkage,
        constructorName,
        module_.get()
    );
    
    // Save current context
    llvm::Function* savedFunction = current_function_;
    llvm::BasicBlock* savedBlock = current_block_;
    
    // Set up constructor function
    current_function_ = constructor;
    current_block_ = llvm::BasicBlock::Create(*context_, "entry", constructor);
    builder_->SetInsertPoint(current_block_);
    
    // Enter new scope for constructor
    enterScope();
    
    // Add constructor parameters to scope
    size_t paramIndex = 0;
    for (auto &arg : constructor->args()) {
        if (paramIndex < typeDecl->params.size()) {
            std::string paramName = typeDecl->params[paramIndex];
            llvm::AllocaInst* alloca = builder_->CreateAlloca(arg.getType(), nullptr, paramName);
            builder_->CreateStore(&arg, alloca);
            current_scope_->variables[paramName] = alloca;
            current_scope_->variable_types[paramName] = arg.getType();
            paramIndex++;
        }
    }
    
    // Allocate memory for the new instance
    llvm::Value* instanceSize = llvm::ConstantExpr::getSizeOf(structType);
    llvm::Function* mallocFunc = module_->getFunction("malloc");
    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
            {llvm::Type::getInt64Ty(*context_)}, 
            false
        );
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    }
    
    llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {instanceSize});
    llvm::Value* instance = builder_->CreateBitCast(rawPtr, llvm::PointerType::get(structType, 0));
    
    // Initialize vtable pointer (first field)
    llvm::GlobalVariable* vtable = getOrCreateVTable(typeDecl->name);
    if (vtable) {
        llvm::Value* vtablePtr = builder_->CreateStructGEP(structType, instance, 0);
        llvm::Value* vtableCast = builder_->CreateBitCast(vtable, llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
        builder_->CreateStore(vtableCast, vtablePtr);
        std::cout << "[LLVM CodeGen] Initialized vtable for " << typeDecl->name << std::endl;
    } else {
        std::cout << "[LLVM CodeGen] Error: VTable for type " << typeDecl->name << " not found" << std::endl;
    }
    
    // Initialize attributes (start from index 1, vtable is at index 0)
    int fieldIndex = 1;
    
    // --- INICIALIZACIÓN DE ATRIBUTOS HEREDADOS ---
    // Recorre la cadena de herencia desde la base más lejana hasta la más cercana
    if (typeDecl->baseType != "Object") {
        std::vector<std::string> inheritanceChain = getInheritanceChain(typeDecl->name);
        std::reverse(inheritanceChain.begin(), inheritanceChain.end()); // base primero
        inheritanceChain.pop_back(); // quitamos el tipo actual

        size_t baseArgIdx = 0;
        for (const std::string& ancestorType : inheritanceChain) {
            auto ancestorIt = type_declarations_.find(ancestorType);
            if (ancestorIt != type_declarations_.end()) {
                const auto& baseAttributes = ancestorIt->second->attributes;
                for (size_t i = 0; i < baseAttributes.size(); ++i) {
                    Expr* expr = nullptr;
                    // Si hay baseArgs, usa el mapping; si no, usa el parámetro correspondiente
                    if (baseArgIdx < typeDecl->baseArgs.size()) {
                        expr = typeDecl->baseArgs[baseArgIdx].get();
                    } else if (baseArgIdx < typeDecl->params.size()) {
                        expr = new VariableExpr(typeDecl->params[baseArgIdx]);
                    }
                    if (expr) {
                        expr->accept(this);
                        llvm::Value* initValue = current_value_;
                        llvm::Value* fieldPtr = builder_->CreateStructGEP(structType, instance, fieldIndex);
                        llvm::Value* boxedValue = initValue;
                        llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
                        if (initValue->getType() != boxedPtrTy) {
                            llvm::Type* valueType = initValue->getType();
                            if (valueType->isDoubleTy()) {
                                boxedValue = createBoxedFromDouble(initValue);
                            } else if (valueType->isIntegerTy(1)) {
                                boxedValue = createBoxedFromBool(initValue);
                            } else if (valueType->isPointerTy() && !valueType->getPointerAddressSpace()) {
                                boxedValue = createBoxedFromString(initValue);
                            } else {
                                boxedValue = builder_->CreateBitCast(initValue, boxedPtrTy);
                            }
                        }
                        builder_->CreateStore(boxedValue, fieldPtr);
                        fieldIndex++;
                    }
                    baseArgIdx++;
                }
            }
        }
    }

    // --- INICIALIZACIÓN DE ATRIBUTOS PROPIOS ---
    for (const auto& attr : typeDecl->attributes) {
        std::cout << "[LLVM CodeGen] Initializing attribute: " << attr->name << std::endl;
        attr->initializer->accept(this);
        llvm::Value* initValue = current_value_;
        llvm::Value* fieldPtr = builder_->CreateStructGEP(structType, instance, fieldIndex);
        llvm::Value* boxedValue = initValue;
        llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
        if (initValue->getType() != boxedPtrTy) {
            llvm::Type* valueType = initValue->getType();
            if (valueType->isDoubleTy()) {
                boxedValue = createBoxedFromDouble(initValue);
            } else if (valueType->isIntegerTy(1)) {
                boxedValue = createBoxedFromBool(initValue);
            } else if (valueType->isPointerTy() && !valueType->getPointerAddressSpace()) {
                boxedValue = createBoxedFromString(initValue);
            } else if (initValue->getType() == boxedPtrTy) {
                boxedValue = initValue;
            } else {
                std::cerr << "[LLVM CodeGen] Warning: Unknown type for attribute initializer, using as-is" << std::endl;
                boxedValue = builder_->CreateBitCast(initValue, boxedPtrTy);
            }
        }
        builder_->CreateStore(boxedValue, fieldPtr);
        fieldIndex++;
    }
    
    // Return the initialized instance
    builder_->CreateRet(instance);
    
    // Exit scope and restore context
    exitScope();
    current_function_ = savedFunction;
    current_block_ = savedBlock;
    if (current_function_ && current_block_) {
        builder_->SetInsertPoint(current_block_);
    }
    
    std::cout << "[LLVM CodeGen] Constructor function " << constructorName << " generated successfully" << std::endl;
}

void LLVMCodeGenerator::generateMethodFunction(MethodDecl *methodDecl)
{
    std::cout << "[LLVM CodeGen] Generating method function: " << methodDecl->name << std::endl;
    
    // Create method function signature
    std::vector<llvm::Type*> paramTypes;
    
    // First parameter is always 'self' (pointer to the instance)
    llvm::StructType* instanceType = getOrCreateStructType(current_type_);
    paramTypes.push_back(llvm::PointerType::get(instanceType, 0));
    
    // Add method parameters - BoxedValue* for unknown types
    llvm::StructType* boxedTy = getBoxedValueType();
    llvm::Type* boxedPtrTy = llvm::PointerType::get(boxedTy, 0);
    
    for (const auto& paramType : methodDecl->paramTypes) {
        llvm::Type* llvmType = getLLVMType(*paramType);
        // If the parameter type is unknown, use BoxedValue*
        if (paramType->getKind() == TypeInfo::Kind::Unknown || 
            paramType->getKind() == TypeInfo::Kind::Object) {
            paramTypes.push_back(boxedPtrTy);
        } else {
            paramTypes.push_back(llvmType);
        }
    }
    
    // CRITICAL FIX: All method functions must return BoxedValue* for dynamic dispatch compatibility
    // regardless of the actual return type in HULK source
    llvm::Type* returnType = llvm::PointerType::get(boxedTy, 0);  // Always BoxedValue*
    
    llvm::FunctionType* methodType = llvm::FunctionType::get(returnType, paramTypes, false);
    
    std::string methodName = current_type_ + "_" + methodDecl->name;
    llvm::Function* method = llvm::Function::Create(
        methodType,
        llvm::Function::ExternalLinkage,
        methodName,
        module_.get()
    );
    
    // Save current context
    llvm::Function* savedFunction = current_function_;
    llvm::BasicBlock* savedBlock = current_block_;
    
    // Set up method function
    current_function_ = method;
    current_block_ = llvm::BasicBlock::Create(*context_, "entry", method);
    builder_->SetInsertPoint(current_block_);
    
    // Enter new scope for method
    enterScope();
    
    // Add method parameters to scope
    size_t argIndex = 0;
    for (auto &arg : method->args()) {
        if (argIndex == 0) {
            // First argument is 'self'
            llvm::AllocaInst* selfAlloca = builder_->CreateAlloca(arg.getType(), nullptr, "self");
            builder_->CreateStore(&arg, selfAlloca);
            current_scope_->variables["self"] = selfAlloca;
            current_scope_->variable_types["self"] = arg.getType();
        } else {
            // Regular method parameters
            size_t paramIndex = argIndex - 1;
            if (paramIndex < methodDecl->params.size()) {
                std::string paramName = methodDecl->params[paramIndex];
                llvm::AllocaInst* alloca = builder_->CreateAlloca(arg.getType(), nullptr, paramName);
                builder_->CreateStore(&arg, alloca);
                current_scope_->variables[paramName] = alloca;
                current_scope_->variable_types[paramName] = arg.getType();
            }
        }
        argIndex++;
    }
    
    // Generate method body
    if (methodDecl->body) {
        methodDecl->body->accept(this);
        
        // Add return statement - CRITICAL FIX: Always convert to BoxedValue*
        if (current_value_) {
            llvm::Value* returnValue = current_value_;
            
            // ALWAYS convert to BoxedValue* since all method functions return BoxedValue*
            if (returnValue->getType() != returnType) {
                llvm::Type* valueType = returnValue->getType();
                if (valueType->isDoubleTy()) {
                    returnValue = createBoxedFromDouble(returnValue);
                    std::cout << "[LLVM CodeGen] Boxing double return value for method" << std::endl;
                } else if (valueType->isIntegerTy(1)) {
                    returnValue = createBoxedFromBool(returnValue);
                    std::cout << "[LLVM CodeGen] Boxing bool return value for method" << std::endl;
                } else if (valueType->isPointerTy() && valueType != returnType) {
                    // Check if it's a string
                    if (valueType == llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)) {
                        returnValue = createBoxedFromString(returnValue);
                        std::cout << "[LLVM CodeGen] Boxing string return value for method" << std::endl;
                    } else {
                        // If already BoxedValue*, use as-is
                        if (valueType == returnType) {
                            // Already correct type
                            std::cout << "[LLVM CodeGen] Return value already BoxedValue*" << std::endl;
                        } else {
                            // Cast to BoxedValue*
                            returnValue = builder_->CreateBitCast(returnValue, returnType);
                            std::cout << "[LLVM CodeGen] Casting return value to BoxedValue*" << std::endl;
                        }
                    }
                } else if (valueType->isIntegerTy(32)) {
                    // Convert int to double then box
                    llvm::Value* doubleValue = builder_->CreateSIToFP(returnValue, llvm::Type::getDoubleTy(*context_));
                    returnValue = createBoxedFromDouble(doubleValue);
                    std::cout << "[LLVM CodeGen] Boxing int->double return value for method" << std::endl;
                } else {
                    std::cout << "[LLVM CodeGen] Warning: Unknown return value type, creating default boxed value" << std::endl;
                    llvm::Value* defaultValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
                    returnValue = createBoxedFromDouble(defaultValue);
                }
            }
            builder_->CreateRet(returnValue);
        } else {
            // No return value - create default BoxedValue* (0.0)
            std::cout << "[LLVM CodeGen] Creating default BoxedValue* return value (0.0) for method" << std::endl;
            llvm::Value* defaultValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
            llvm::Value* boxedDefault = createBoxedFromDouble(defaultValue);
            builder_->CreateRet(boxedDefault);
        }
    } else {
        // No body, create default return
        if (returnType->isVoidTy()) {
            builder_->CreateRetVoid();
        } else {
            builder_->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0));
        }
    }
    
    // Exit scope and restore context
    exitScope();
    current_function_ = savedFunction;
    current_block_ = savedBlock;
    if (current_function_ && current_block_) {
        builder_->SetInsertPoint(current_block_);
    }
    
    std::cout << "[LLVM CodeGen] Method function " << methodName << " generated successfully" << std::endl;
}

void LLVMCodeGenerator::registerBuiltinFunctions()
{
    // Register printf
    std::vector<llvm::Type*> printfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), printfArgs, true);
    (void)llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module_.get());

    // Register malloc
    llvm::FunctionType* mallocType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
        {llvm::Type::getInt64Ty(*context_)}, false);
    (void)llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    
    // Register pow (math library)
    std::vector<llvm::Type*> powArgs = {llvm::Type::getDoubleTy(*context_), llvm::Type::getDoubleTy(*context_)};
    llvm::FunctionType* powType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), powArgs, false);
    (void)llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "pow", module_.get());
    
    // Register mathematical functions (sin, cos, sqrt, etc.)
    std::vector<llvm::Type*> mathSingleArgs = {llvm::Type::getDoubleTy(*context_)};
    llvm::FunctionType* mathSingleType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), mathSingleArgs, false);
    
    (void)llvm::Function::Create(mathSingleType, llvm::Function::ExternalLinkage, "sin", module_.get());
    (void)llvm::Function::Create(mathSingleType, llvm::Function::ExternalLinkage, "cos", module_.get());
    (void)llvm::Function::Create(mathSingleType, llvm::Function::ExternalLinkage, "sqrt", module_.get());
    (void)llvm::Function::Create(mathSingleType, llvm::Function::ExternalLinkage, "exp", module_.get());
    
    // Register log function - needs natural logarithm for internal calculations
    (void)llvm::Function::Create(mathSingleType, llvm::Function::ExternalLinkage, "log", module_.get());
    
    // Note: log(base, argument) will be handled specially in handleLogFunction
    // using the formula: log_base(argument) = ln(argument) / ln(base)
    
    // Register standard rand function - no parameters, returns int
    std::vector<llvm::Type*> stdRandArgs = {}; // No parameters  
    llvm::FunctionType* stdRandType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), stdRandArgs, false);
    (void)llvm::Function::Create(stdRandType, llvm::Function::ExternalLinkage, "rand", module_.get());
    
    // Register srand function - takes int seed, returns void
    std::vector<llvm::Type*> srandArgs = {llvm::Type::getInt32Ty(*context_)};
    llvm::FunctionType* srandType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), srandArgs, false);
    (void)llvm::Function::Create(srandType, llvm::Function::ExternalLinkage, "srand", module_.get());
    
    // Register time function - takes pointer to time_t (can be null), returns time_t (long/int64)
    std::vector<llvm::Type*> timeArgs = {llvm::PointerType::get(llvm::Type::getInt64Ty(*context_), 0)};
    llvm::FunctionType* timeType = llvm::FunctionType::get(llvm::Type::getInt64Ty(*context_), timeArgs, false);
    (void)llvm::Function::Create(timeType, llvm::Function::ExternalLinkage, "time", module_.get());
    
    // Register sprintf (for string formatting)
    std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                           llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
    (void)llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    
    // Register strcpy (for string copying)
    std::vector<llvm::Type*> strcpyArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                          llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* strcpyType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strcpyArgs, false);
    (void)llvm::Function::Create(strcpyType, llvm::Function::ExternalLinkage, "strcpy", module_.get());
    
    // Register strcat (for string concatenation)
    std::vector<llvm::Type*> strcatArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 
                                          llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
    llvm::FunctionType* strcatType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), strcatArgs, false);
    (void)llvm::Function::Create(strcatType, llvm::Function::ExternalLinkage, "strcat", module_.get());
    
    // Create BoxedValue helper functions
    std::cout << "[LLVM CodeGen] Creating BoxedValue helper functions" << std::endl;
    createPrintBoxedFunction();
    createUnboxFunction();
    createTypeCheckFunctions();
    createTypeSpecificUnboxFunctions();
    
    // Register mathematical constants
    registerMathematicalConstants();
    
    std::cout << "[LLVM CodeGen] All built-in functions and constants registered" << std::endl;
}

void LLVMCodeGenerator::registerMathematicalConstants()
{
    std::cout << "[LLVM CodeGen] Registering mathematical constants PI and E" << std::endl;
    
    // Create global constant for PI (3.14159265359)
    llvm::Constant* piValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 3.14159265359);
    llvm::GlobalVariable* piGlobal = new llvm::GlobalVariable(
        *module_,
        llvm::Type::getDoubleTy(*context_),
        true, // isConstant
        llvm::GlobalValue::ExternalLinkage,
        piValue,
        "PI"
    );
    piGlobal->setAlignment(llvm::MaybeAlign(8));
    global_constants_["PI"] = piGlobal;
    
    // Create global constant for E (2.71828182846)
    llvm::Constant* eValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 2.71828182846);
    llvm::GlobalVariable* eGlobal = new llvm::GlobalVariable(
        *module_,
        llvm::Type::getDoubleTy(*context_),
        true, // isConstant
        llvm::GlobalValue::ExternalLinkage,
        eValue,
        "E"
    );
    eGlobal->setAlignment(llvm::MaybeAlign(8));
    global_constants_["E"] = eGlobal;
    
    std::cout << "[LLVM CodeGen] Constants PI and E registered successfully" << std::endl;
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

// Removed createBoxedFromInt - all numbers are now double

llvm::Value* LLVMCodeGenerator::createBoxedFromDouble(llvm::Value* doubleVal) {
    auto boxedTy = getBoxedValueType();
    
    // Allocate memory on heap using malloc
    llvm::Value* size = llvm::ConstantExpr::getSizeOf(boxedTy);
    llvm::Function* mallocFunc = module_->getFunction("malloc");
    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
            {llvm::Type::getInt64Ty(*context_)}, false);
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    }
    
    llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {size});
    llvm::Value* boxed = builder_->CreateBitCast(rawPtr, llvm::PointerType::get(boxedTy, 0), "boxed_double");
    
    // type_tag = 1 (numbers are now always double)
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), builder_->CreateStructGEP(boxedTy, boxed, 0));
    // Guardar el double en los primeros 8 bytes de data
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    builder_->CreateStore(doubleVal, dataPtr);
    return boxed;
}

llvm::Value* LLVMCodeGenerator::createBoxedFromBool(llvm::Value* boolVal) {
    auto boxedTy = getBoxedValueType();
    
    // Allocate memory on heap using malloc
    llvm::Value* size = llvm::ConstantExpr::getSizeOf(boxedTy);
    llvm::Function* mallocFunc = module_->getFunction("malloc");
    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
            {llvm::Type::getInt64Ty(*context_)}, false);
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    }
    
    llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {size});
    llvm::Value* boxed = builder_->CreateBitCast(rawPtr, llvm::PointerType::get(boxedTy, 0), "boxed_bool");
    
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
    
    // Allocate memory on heap using malloc
    llvm::Value* size = llvm::ConstantExpr::getSizeOf(boxedTy);
    llvm::Function* mallocFunc = module_->getFunction("malloc");
    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
            {llvm::Type::getInt64Ty(*context_)}, false);
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module_.get());
    }
    
    llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {size});
    llvm::Value* boxed = builder_->CreateBitCast(rawPtr, llvm::PointerType::get(boxedTy, 0), "boxed_str");
    
    // type_tag = 2 (simplified: 0=bool, 1=double, 2=string)
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), builder_->CreateStructGEP(boxedTy, boxed, 0));
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
    (void)builder_->CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    // Comparar con el esperado (eliminado cmp porque no se usa)
    // Extraer el valor según el tipo
    llvm::Value* dataPtr = builder_->CreateStructGEP(boxedTy, boxed, 1);
    switch (type_tag) {
        case 0: // bool
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
            return builder_->CreateLoad(llvm::Type::getInt1Ty(*context_), dataPtr);
        case 1: // double
            dataPtr = builder_->CreatePointerCast(dataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
            return builder_->CreateLoad(llvm::Type::getDoubleTy(*context_), dataPtr);
        case 2: // string
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
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "print_double", printBoxedFunc);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "print_string", printBoxedFunc);
    llvm::BasicBlock* boolBlock = llvm::BasicBlock::Create(*context_, "print_bool", printBoxedFunc);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge", printBoxedFunc);
    
    // Switch on type tag (simplified: 0=bool, 1=double, 2=string)
    llvm::SwitchInst* switchInst = printBuilder.CreateSwitch(typeTag, doubleBlock, 3);
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);   // bool
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), doubleBlock); // double
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), stringBlock); // string
    
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
    
    // Print double (now handles all numeric values)
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
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "unbox_double", unboxFunc);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "unbox_string", unboxFunc);
    
    // Switch on type tag (simplified: 0=bool, 1=double, 2=string)
    llvm::SwitchInst* switchInst = unboxBuilder.CreateSwitch(typeTag, boolBlock, 3);
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);   // bool
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), doubleBlock); // double
    switchInst->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), stringBlock); // string
    
    // Unbox boolean
    unboxBuilder.SetInsertPoint(boolBlock);
    llvm::Value* boolDataPtr = unboxBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    boolDataPtr = unboxBuilder.CreatePointerCast(boolDataPtr, llvm::PointerType::get(llvm::Type::getInt1Ty(*context_), 0));
    llvm::Value* boolValue = unboxBuilder.CreateLoad(llvm::Type::getInt1Ty(*context_), boolDataPtr);
    llvm::Value* boolResult = unboxBuilder.CreateIntToPtr(boolValue, llvm::Type::getInt8Ty(*context_)->getPointerTo());
    unboxBuilder.CreateRet(boolResult);
    
    // Unbox double (now handles all numeric values)
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
    
    // isInt - now checks for double (tag 1)
    llvm::Function* isIntFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "isInt", module_.get());
    llvm::BasicBlock* isIntEntry = llvm::BasicBlock::Create(*context_, "entry", isIntFunc);
    llvm::IRBuilder<> isIntBuilder(isIntEntry);
    llvm::Value* boxedValue = isIntFunc->getArg(0);
    llvm::Value* tagPtr = isIntBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    llvm::Value* tag = isIntBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    llvm::Value* cmp = isIntBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1));
    isIntBuilder.CreateRet(cmp);
    
    // isDouble - now also checks for tag 1 (same as isInt since all numbers are double)
    llvm::Function* isDoubleFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "isDouble", module_.get());
    llvm::BasicBlock* isDoubleEntry = llvm::BasicBlock::Create(*context_, "entry", isDoubleFunc);
    llvm::IRBuilder<> isDoubleBuilder(isDoubleEntry);
    boxedValue = isDoubleFunc->getArg(0);
    tagPtr = isDoubleBuilder.CreateStructGEP(boxedTy, boxedValue, 0);
    tag = isDoubleBuilder.CreateLoad(llvm::Type::getInt32Ty(*context_), tagPtr);
    cmp = isDoubleBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1));
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
    cmp = isStringBuilder.CreateICmpEQ(tag, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2));
    isStringBuilder.CreateRet(cmp);
}

void LLVMCodeGenerator::createTypeSpecificUnboxFunctions()
{
    std::cout << "[LLVM CodeGen] Creating type-specific unbox functions" << std::endl;
    
    llvm::StructType* boxedTy = getBoxedValueType();
    std::vector<llvm::Type*> funcArgs = {llvm::PointerType::get(boxedTy, 0)};
    
    // Create unboxInt function (now redirected to handle double since tag 1 is double)
    llvm::FunctionType* unboxIntType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), funcArgs, false);
    llvm::Function* unboxIntFunc = llvm::Function::Create(unboxIntType, llvm::Function::InternalLinkage, "unboxInt", module_.get());
    llvm::BasicBlock* unboxIntEntry = llvm::BasicBlock::Create(*context_, "entry", unboxIntFunc);
    llvm::IRBuilder<> unboxIntBuilder(unboxIntEntry);
    
    llvm::Value* boxedValue = unboxIntFunc->getArg(0);
    llvm::Value* intDataPtr = unboxIntBuilder.CreateStructGEP(boxedTy, boxedValue, 1);
    intDataPtr = unboxIntBuilder.CreatePointerCast(intDataPtr, llvm::PointerType::get(llvm::Type::getDoubleTy(*context_), 0));
    llvm::Value* intValue = unboxIntBuilder.CreateLoad(llvm::Type::getDoubleTy(*context_), intDataPtr);
    llvm::Value* doubleAsInt = unboxIntBuilder.CreateFPToSI(intValue, llvm::Type::getInt32Ty(*context_));
    unboxIntBuilder.CreateRet(doubleAsInt);
    
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
    
    // Since all numbers are now double, just unbox both as double and add
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* result = builder_->CreateFAdd(leftDoubleVal, rightDoubleVal);
    
    return result;
}

llvm::Value* LLVMCodeGenerator::boxed_add_native_left(llvm::Value* nativeVal, llvm::Value* boxed) {
    std::cout << "[LLVM CodeGen] boxed_add_native_left(native, BoxedValue*)" << std::endl;
    
    llvm::Type* nativeType = nativeVal->getType();
    
    // Convert native value to double if needed
    llvm::Value* nativeAsDouble;
    if (nativeType->isIntegerTy(32)) {
        nativeAsDouble = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
    } else if (nativeType->isDoubleTy()) {
        nativeAsDouble = nativeVal;
    } else {
        // Fallback: convert to int first, then to double
        llvm::Value* nativeAsInt = builder_->CreatePtrToInt(nativeVal, llvm::Type::getInt32Ty(*context_));
        nativeAsDouble = builder_->CreateSIToFP(nativeAsInt, llvm::Type::getDoubleTy(*context_));
    }
    
    // Since all numbers are now double, just unbox as double and add
        llvm::Value* boxedDoubleVal = unboxDouble(boxed);
    llvm::Value* result = builder_->CreateFAdd(nativeAsDouble, boxedDoubleVal);
    
    return result;
}

llvm::Value* LLVMCodeGenerator::boxed_add_native_right(llvm::Value* boxed, llvm::Value* nativeVal) {
    std::cout << "[LLVM CodeGen] boxed_add_native_right(BoxedValue*, native)" << std::endl;
    // Addition is commutative, so we can just swap the arguments
    return boxed_add_native_left(nativeVal, boxed);
}

llvm::Value* LLVMCodeGenerator::boxed_sub(llvm::Value* left, llvm::Value* right) {
    std::cout << "[LLVM CodeGen] boxed_sub(BoxedValue*, BoxedValue*)" << std::endl;
    
    // Since all numbers are now double, just unbox both as double and subtract
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* result = builder_->CreateFSub(leftDoubleVal, rightDoubleVal);
    
    return result;
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
    
    // Since all numbers are now double, just unbox both as double and multiply
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* result = builder_->CreateFMul(leftDoubleVal, rightDoubleVal);
    
    return result;
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
    
    // Since all numbers are now double, just unbox both as double and divide
    llvm::Value* leftDoubleVal = unboxDouble(left);
    llvm::Value* rightDoubleVal = unboxDouble(right);
    llvm::Value* result = builder_->CreateFDiv(leftDoubleVal, rightDoubleVal);
    
    return result;
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
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "double_to_str", current_function_);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "string_to_str", current_function_);
    llvm::BasicBlock* unknownBlock = llvm::BasicBlock::Create(*context_, "unknown_to_str", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_to_str", current_function_);
    
    // Create switch on type (simplified: 0=bool, 1=double, 2=string)
    llvm::SwitchInst* typeSwitch = builder_->CreateSwitch(typeTag, unknownBlock, 3);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), doubleBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), stringBlock);
    
    // Boolean to string
    builder_->SetInsertPoint(boolBlock);
    llvm::Value* boolVal = unboxBool(boxed);
    llvm::Value* trueStr = registerStringConstant("true");
    llvm::Value* falseStr = registerStringConstant("false");
    llvm::Value* boolResult = builder_->CreateSelect(boolVal, trueStr, falseStr);
    builder_->CreateBr(mergeBlock);
    
    // Double to string (now handles all numeric values)
    builder_->SetInsertPoint(doubleBlock);
    llvm::Value* doubleVal = unboxDouble(boxed);
    
    // Create sprintf function declaration
    llvm::Function* sprintfFunc = module_->getFunction("sprintf");
    if (!sprintfFunc) {
        std::vector<llvm::Type*> sprintfArgs = {llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
                                                llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0)};
        llvm::FunctionType* sprintfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), sprintfArgs, true);
        sprintfFunc = llvm::Function::Create(sprintfType, llvm::Function::ExternalLinkage, "sprintf", module_.get());
    }
    
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
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0), 4, "str_result");
    resultPhi->addIncoming(boolResult, boolBlock);
    resultPhi->addIncoming(doubleBuffer, doubleBlock);
    resultPhi->addIncoming(stringResult, stringBlock);
    resultPhi->addIncoming(unknownResult, unknownBlock);
    
    return resultPhi;
}

// Helper functions for converting specific types to strings

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
        // Convert i32 to double first, then to string
        std::cout << "[LLVM CodeGen] Converting i32 to double then string" << std::endl;
        llvm::Value* doubleVal = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        return doubleToString(doubleVal);
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
        // Convert i8 to double first, then to string
        std::cout << "[LLVM CodeGen] Converting i8 to double then string" << std::endl;
        llvm::Value* doubleVal = builder_->CreateUIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        return doubleToString(doubleVal);
    } else if (nativeType->isIntegerTy(16)) {
        // Convert i16 to double first, then to string
        std::cout << "[LLVM CodeGen] Converting i16 to double then string" << std::endl;
        llvm::Value* doubleVal = builder_->CreateUIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        return doubleToString(doubleVal);
    } else if (nativeType->isIntegerTy(64)) {
        // Convert i64 to double first, then to string
        std::cout << "[LLVM CodeGen] Converting i64 to double then string" << std::endl;
        llvm::Value* doubleVal = builder_->CreateSIToFP(nativeVal, llvm::Type::getDoubleTy(*context_));
        return doubleToString(doubleVal);
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
    llvm::BasicBlock* doubleBlock = llvm::BasicBlock::Create(*context_, "double_to_bool", current_function_);
    llvm::BasicBlock* stringBlock = llvm::BasicBlock::Create(*context_, "string_to_bool", current_function_);
    llvm::BasicBlock* unknownBlock = llvm::BasicBlock::Create(*context_, "unknown_to_bool", current_function_);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context_, "merge_to_bool", current_function_);
    
    // Create switch on type (simplified: 0=bool, 1=double, 2=string)
    llvm::SwitchInst* typeSwitch = builder_->CreateSwitch(typeTag, unknownBlock, 3);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), boolBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1), doubleBlock);
    typeSwitch->addCase(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 2), stringBlock);
    
    // Boolean (direct value)
    builder_->SetInsertPoint(boolBlock);
    llvm::Value* boolResult = unboxBool(boxed);
    builder_->CreateBr(mergeBlock);
    
    // Double (false if 0.0, true if != 0.0) - now handles all numeric values
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
    llvm::PHINode* resultPhi = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 4, "bool_result");
    resultPhi->addIncoming(boolResult, boolBlock);
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

// === Type tracking functions for LLVM 19 opaque pointers ===

void LLVMCodeGenerator::trackValueType(llvm::Value* value, const std::string& typeName)
{
    if (value && !typeName.empty()) {
        value_type_map_[value] = typeName;
        std::cout << "[LLVM CodeGen] Tracked value type: " << typeName << " for value " << value << std::endl;
    }
}

std::string LLVMCodeGenerator::getTrackedValueType(llvm::Value* value)
{
    if (!value) return "";
    
    auto it = value_type_map_.find(value);
    if (it != value_type_map_.end()) {
        std::cout << "[LLVM CodeGen] Found tracked type: " << it->second << " for value " << value << std::endl;
        return it->second;
    }
    return "";
}

void LLVMCodeGenerator::trackVariableType(const std::string& varName, const std::string& typeName)
{
    if (!varName.empty() && !typeName.empty()) {
        variable_type_map_[varName] = typeName;
        std::cout << "[LLVM CodeGen] Tracked variable type: " << varName << " -> " << typeName << std::endl;
    }
}

std::string LLVMCodeGenerator::getTrackedVariableType(const std::string& varName)
{
    auto it = variable_type_map_.find(varName);
    if (it != variable_type_map_.end()) {
        std::cout << "[LLVM CodeGen] Found tracked variable type: " << varName << " -> " << it->second << std::endl;
        return it->second;
    }
    return "";
}

void LLVMCodeGenerator::handleRandFunction(CallExpr *expr)
{
    std::cout << "[LLVM CodeGen] Handling rand function" << std::endl;
    
    // Call the standard rand() function which returns int
    llvm::Function* randFunc = module_->getFunction("rand");
    if (!randFunc) {
        std::cerr << "[LLVM CodeGen] Error: rand function not found" << std::endl;
        current_value_ = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 0.0);
        return;
    }
    
    // Call rand() - no arguments
    llvm::Value* randResult = builder_->CreateCall(randFunc, {});
    
    // Limit to range 0-100 using modulo 101
    llvm::Value* modValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 101);
    llvm::Value* randLimited = builder_->CreateSRem(randResult, modValue);
    
    // Ensure positive result (in case of negative numbers, though rand() shouldn't return them)
    llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    llvm::Value* isNegative = builder_->CreateICmpSLT(randLimited, zero);
    llvm::Value* positiveResult = builder_->CreateSelect(isNegative, 
                                                         builder_->CreateAdd(randLimited, modValue), 
                                                         randLimited);
    
    // Convert int result to double (HULK uses Number = double)
    current_value_ = builder_->CreateSIToFP(positiveResult, llvm::Type::getDoubleTy(*context_));
    
    std::cout << "[LLVM CodeGen] rand() function processed successfully - returns integer between 0 and 100" << std::endl;
}

void LLVMCodeGenerator::handleLogFunction(CallExpr *expr)
{
    std::cout << "[LLVM CodeGen] Handling log function with base and argument" << std::endl;
    
    // Verify we have exactly 2 arguments
    if (expr->args.size() != 2) {
        std::cerr << "Error: log function expects exactly 2 arguments (base, argument)" << std::endl;
        current_value_ = nullptr;
        return;
    }
    
    // Evaluate the base (first argument)
    expr->args[0]->accept(this);
    llvm::Value* base = current_value_;
    
    // Evaluate the argument (second argument)
    expr->args[1]->accept(this);
    llvm::Value* argument = current_value_;
    
    // Convert both to double if they aren't already
    if (!base->getType()->isDoubleTy()) {
        base = convertToDouble(base);
    }
    if (!argument->getType()->isDoubleTy()) {
        argument = convertToDouble(argument);
    }
    
    // Get the natural logarithm function
    llvm::Function* logFunc = module_->getFunction("log");
    if (!logFunc) {
        std::cerr << "Error: log function not found in module" << std::endl;
        current_value_ = nullptr;
        return;
    }
    
    // Calculate ln(argument) and ln(base)
    llvm::Value* lnArgument = builder_->CreateCall(logFunc, {argument}, "ln_argument");
    llvm::Value* lnBase = builder_->CreateCall(logFunc, {base}, "ln_base");
    
    // Calculate log_base(argument) = ln(argument) / ln(base)
    llvm::Value* result = builder_->CreateFDiv(lnArgument, lnBase, "log_base_result");
    
    current_value_ = result;
    std::cout << "[LLVM CodeGen] log(base, argument) function result: double" << std::endl;
}