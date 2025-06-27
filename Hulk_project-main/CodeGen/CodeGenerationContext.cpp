#include "../include/CodeGen/CodeGenerationContext.hpp"
#include "../include/CodeGen/LlvmVisitor.hpp"




void CodeGenerationContext::pushLocalScope() { variableScopes.emplace_back(); }
void CodeGenerationContext::popLocalScope()  { if (!variableScopes.empty()) variableScopes.pop_back(); }
void CodeGenerationContext::addLocalVariable(const std::string& name, llvm::AllocaInst* value) {
    if (!variableScopes.empty())
        variableScopes.back()[name] = value;
}
llvm::AllocaInst* CodeGenerationContext::getLocalVariable(const std::string& name) const {
    for (auto it = variableScopes.rbegin(); it != variableScopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end())
            return found->second;
    }
    return nullptr;
}

llvm::AllocaInst* CodeGenerationContext::createEntryBlockAlloca(llvm::Function *function, llvm::Type* type,const std::string &varName) 
{
    llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(),function->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}


llvm::Type* CodeGenerationContext::getInteralType(std::string type)
{
    if (type == "Number") 
        return llvm::Type::getDoubleTy(llvmContext);
    if (type == "String") 
        return llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0);
    if (type == "Boolean") 
        return llvm::Type::getInt1Ty(llvmContext);
    if (type == "Object") 
        return llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0);
    if (type == "Void" ) 
        return llvm::Type::getVoidTy(llvmContext);

    return nullptr;
}

int CodeGenerationContext::GetIndexOfMember(TypeNode*node,std::string member)
{
    for (size_t i = 0; i < node->atributes.size(); i++)
    {
        auto attr= static_cast<AttributeNode*>(node->atributes[i]);
        if(attr->id.lexeme==member) return i;
    }
    
    return -1;
}


void CodeGenerationContext::pushFunctionScope() { functionScopes.emplace_back(); }
void CodeGenerationContext::popFunctionScope()  { if (!functionScopes.empty()) functionScopes.pop_back(); }
void CodeGenerationContext::declareFunction(const std::string& name, MethodNode* decl) {
    if (!functionScopes.empty())
        functionScopes.back()[name] = decl;
}
MethodNode* CodeGenerationContext::getFunctionDeclaration(const std::string& name) const {
    for (auto it = functionScopes.rbegin(); it != functionScopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end())
            return found->second;
    }
    return nullptr;
}


llvm::Value* CodeGenerationContext::getFormatString()
{
    llvm::GlobalVariable* formatVar = llvmModule.getNamedGlobal("formatStr");
    if (!formatVar) 
    {
        llvm::Constant* formatConst = llvm::ConstantDataArray::getString(llvmModule.getContext(), "%g\n", true);
        formatVar = new llvm::GlobalVariable(
            llvmModule,
            formatConst->getType(),
            true, // Es una constante
            llvm::GlobalValue::PrivateLinkage,
            formatConst,
            "formatStr"
        );
    }
    // Asegurarse de obtener un tipo pointer a i8
    return irBuilder.CreateBitCast(formatVar, llvm::PointerType::get(llvm::Type::getInt8Ty(llvmModule.getContext()), 0));
}

void CodeGenerationContext::generateIR(AstNode* root)
{
    pushFunctionScope();

    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(llvmContext),
        {llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0)},
        true // Variadic function
    );
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", &llvmModule);

 

    llvm::FunctionType* putsType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(llvmContext),
        {llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0)},
        false
    );
    llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", &llvmModule);


    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(llvmContext), false
    );
    llvm::Function* mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", llvmModule
    );
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", mainFunc);
    irBuilder.SetInsertPoint(entry);

    
    LlvmVisitor visitor(*this);
    visitor.currentFunction=mainFunc;
    root->accept(visitor);


        // auto val=visitor.lastValue;
        // if(!val)    return;
        // if (val->getType()->isDoubleTy()) { // Number
        //     llvm::Value* format = getFormatString();
        //     irBuilder.CreateCall(llvmModule.getFunction("printf"), {format, val});
        // } else if (val->getType()->isIntegerTy(1)) { // Boolean
        //     llvm::Value* str = irBuilder.CreateSelect(
        //         val,
        //         irBuilder.CreateGlobalStringPtr("true\n"),
        //         irBuilder.CreateGlobalStringPtr("false\n")
        //     );
        //     irBuilder.CreateCall(llvmModule.getFunction("puts"), {str});
        // } else if (val->getType()->isPointerTy()) { // String
        //     irBuilder.CreateCall(llvmModule.getFunction("puts"), {val});
        // }
    
   

    irBuilder.CreateRet(llvm::ConstantInt::get(llvmContext, llvm::APInt(32, 0)));
    llvm::verifyFunction(*mainFunc);

}

void CodeGenerationContext::dumpIR(const std::string &filename) {
        std::error_code EC;
        llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
        if (EC) {
            llvm::errs() << "Error al abrir el archivo: " << EC.message();
            return;
        }
        llvmModule.print(dest, nullptr);
}

