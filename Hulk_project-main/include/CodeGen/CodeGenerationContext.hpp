#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include "../../common/AstNodes.hpp"

class CodeGenerationContext {
public:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> irBuilder;
    llvm::Module llvmModule;

    std::unordered_map<std::string, llvm::StructType*> namedStruct;
    std::unordered_map<std::string, TypeNode*> namedTypeNode;
    std::string currentType;

    CodeGenerationContext():
        irBuilder(llvmContext), llvmModule("main_module", llvmContext) {};

    void generateIR(AstNode* root);
    void dumpIR(const std::string& filename = "hulk/output.ll");

    // Variables locales
    std::vector<std::unordered_map<std::string, llvm::AllocaInst*>> variableScopes;
    void pushLocalScope();
    void popLocalScope();
    void addLocalVariable(const std::string& name, llvm::AllocaInst* value);
    llvm::AllocaInst* getLocalVariable(const std::string& name) const;
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName);

    // Funciones declaradas
    std::vector<std::unordered_map<std::string, MethodNode*>> functionScopes;
    void pushFunctionScope();
    void popFunctionScope();
    void declareFunction(const std::string& name, MethodNode* decl);
    MethodNode* getFunctionDeclaration(const std::string& name) const;

    // Tipos primitivos
    llvm::Type* getInteralType(std::string type);

    // Utilidades
    int GetIndexOfMember(TypeNode* node, std::string member);
    llvm::Value* getFormatString();
};
