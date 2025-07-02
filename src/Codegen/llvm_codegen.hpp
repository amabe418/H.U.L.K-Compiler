#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <set>

// LLVM includes
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include "../AST/ast.hpp"
#include "../Types/type_info.hpp"
#include "../Types/boxed_value.hpp"

// Forward declaration
class LLVMScope;

class LLVMCodeGenerator : public StmtVisitor, public ExprVisitor
{
private:
    // LLVM components
    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    
    // Current function being generated
    llvm::Function* current_function_;
    
    // Current basic block
    llvm::BasicBlock* current_block_;
    
    // Current return value for expressions
    llvm::Value* current_value_;
    
    // Variable counter for unique names
    int var_counter_ = 0;
    
    // Function declarations
    std::unordered_map<std::string, llvm::Function*> functions_;
    
    // Type declarations
    std::unordered_map<std::string, llvm::StructType*> types_;
    
    // Scope management for variables
    LLVMScope* current_scope_;
    std::vector<std::unique_ptr<LLVMScope>> scopes_;
    
    // Track generated string constants
    std::unordered_map<std::string, llvm::GlobalVariable*> string_constants_;
    
    // Current type being generated
    std::string current_type_;
    
    // BoxedValue helpers
    /**
     * Estructura BoxedValue en LLVM: { i32 type_tag, [8 x i8] data }
     * type_tag: 0=bool, 1=int, 2=double, 3=string
     */
    llvm::StructType* getBoxedValueType();
    llvm::Value* createBoxedFromInt(llvm::Value* intVal);
    llvm::Value* createBoxedFromDouble(llvm::Value* doubleVal);
    llvm::Value* createBoxedFromBool(llvm::Value* boolVal);
    llvm::Value* createBoxedFromString(llvm::Value* strVal);
    llvm::Value* extractFromBoxed(llvm::Value* boxed, int type_tag); // Extrae el valor nativo si el tag coincide
    
    // Type checking functions
    llvm::Value* isInt(llvm::Value* boxed);
    llvm::Value* isDouble(llvm::Value* boxed);
    llvm::Value* isBool(llvm::Value* boxed);
    llvm::Value* isString(llvm::Value* boxed);
    
    // Type-specific unbox functions
    llvm::Value* unboxInt(llvm::Value* boxed);
    llvm::Value* unboxDouble(llvm::Value* boxed);
    llvm::Value* unboxBool(llvm::Value* boxed);
    llvm::Value* unboxString(llvm::Value* boxed);
    
    llvm::Value* unbox(llvm::Value* boxed); // Función unbox genérica que usa las funciones específicas
    
    // Helper methods
    std::string generateUniqueName(const std::string &base);
    llvm::Type* getLLVMType(const TypeInfo &type);
    llvm::Value* createConstant(const TypeInfo &type, const std::string &value);
    void enterScope();
    void exitScope();
    llvm::GlobalVariable* registerStringConstant(const std::string &value);
    llvm::Function* getOrCreateFunction(const std::string &name, llvm::Type* returnType, const std::vector<llvm::Type*>& paramTypes);
    llvm::StructType* getOrCreateStructType(const std::string &name);
    void generateConstructorFunction(TypeDecl *typeDecl);
    void generateMethodFunction(MethodDecl *methodDecl);
    llvm::Value* toStringLLVM(llvm::Value* value);
    
    // Built-in functions
    void registerBuiltinFunctions();
    void createPrintBoxedFunction();
    void createUnboxFunction();
    void createTypeCheckFunctions();
    void createTypeSpecificUnboxFunctions();
    
    // Special function handlers
    void handlePrintFunction(CallExpr *expr);

public:
    LLVMCodeGenerator();
    ~LLVMCodeGenerator();

    // Initialize the code generator
    void initialize(const std::string &module_name);

    // Generate code for a program
    void generateCode(Program *program);

    // Print the generated IR
    void printIR();

    // Write IR to file
    void writeIRToFile(const std::string &filename);

    // Get the generated IR as string
    std::string getIR() const;

    // StmtVisitor implementations
    void visit(Program *stmt) override;
    void visit(ExprStmt *stmt) override;
    void visit(FunctionDecl *stmt) override;
    void visit(TypeDecl *stmt) override;
    void visit(MethodDecl *stmt) override;
    void visit(AttributeDecl *stmt) override;

    // ExprVisitor implementations
    void visit(NumberExpr *expr) override;
    void visit(StringExpr *expr) override;
    void visit(BooleanExpr *expr) override;
    void visit(UnaryExpr *expr) override;
    void visit(BinaryExpr *expr) override;
    void visit(CallExpr *expr) override;
    void visit(VariableExpr *expr) override;
    void visit(LetExpr *expr) override;
    void visit(AssignExpr *expr) override;
    void visit(IfExpr *expr) override;
    void visit(ExprBlock *expr) override;
    void visit(WhileExpr *expr) override;
    void visit(ForExpr *expr) override;
    void visit(NewExpr *expr) override;
    void visit(GetAttrExpr *expr) override;
    void visit(SetAttrExpr *expr) override;
    void visit(MethodCallExpr *expr) override;
    void visit(SelfExpr *expr) override;
    void visit(BaseCallExpr *expr) override;
    void visit(IsExpr *expr) override;
    void visit(AsExpr *expr) override;
};

// Scope structure for variable management in LLVM
class LLVMScope
{
public:
    std::unordered_map<std::string, llvm::AllocaInst*> variables; // variable name -> alloca instruction
    std::unordered_map<std::string, llvm::Type*> variable_types;  // variable name -> type
    LLVMScope* parent;

    LLVMScope(LLVMScope* p = nullptr) : parent(p) {}
}; 