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
    
    // Type metadata: maps type name to attribute name -> index
    std::unordered_map<std::string, std::unordered_map<std::string, int>> type_attributes_;
    
    // Inheritance and polymorphism metadata
    std::unordered_map<std::string, TypeDecl*> type_declarations_;
    std::unordered_map<std::string, llvm::GlobalVariable*> vtables_;
    std::unordered_map<std::string, std::vector<std::string>> type_methods_;
    
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
    // Removed createBoxedFromInt - all numbers are now double
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
    
    // Mixed-type arithmetic operations (return native types)
    // BoxedValue + BoxedValue → native type
    llvm::Value* boxed_add(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_sub(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_mul(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_div(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_pow(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_mod(llvm::Value* left, llvm::Value* right);
    
    // Native + BoxedValue → native type
    llvm::Value* boxed_add_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_sub_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_mul_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_div_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_pow_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_mod_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    
    // BoxedValue + Native → native type
    llvm::Value* boxed_add_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_sub_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_mul_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_div_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_pow_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_mod_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    
    // Mixed-type comparison operations (return i1 boolean)
    // BoxedValue comparison BoxedValue → i1
    llvm::Value* boxed_greater(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_lessthan(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_ge(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_le(llvm::Value* left, llvm::Value* right);
    
    // Native comparison BoxedValue → i1
    llvm::Value* boxed_greater_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_lessthan_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_ge_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_le_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    
    // BoxedValue comparison Native → i1
    llvm::Value* boxed_greater_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_lessthan_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_ge_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_le_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    
    // Mixed-type equality operations (return i1 boolean)
    // BoxedValue equality BoxedValue → i1
    llvm::Value* boxed_equals(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_neq(llvm::Value* left, llvm::Value* right);
    
    // Native equality BoxedValue → i1
    llvm::Value* boxed_equals_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_neq_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    
    // BoxedValue equality Native → i1
    llvm::Value* boxed_equals_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_neq_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    
    // Mixed-type concatenation operations (return BoxedValue* when BoxedValues involved)
    // Helper function to convert any BoxedValue to string
    llvm::Value* boxedValueToString(llvm::Value* boxed);
    
    // Helper function to convert any native value to string
    llvm::Value* nativeValueToString(llvm::Value* nativeVal);
    
    // Helper functions for converting specific types to strings
    llvm::Value* doubleToString(llvm::Value* doubleVal);
    llvm::Value* boolToString(llvm::Value* boolVal);
    
    // BoxedValue concat BoxedValue → BoxedValue*
    llvm::Value* boxed_concat(llvm::Value* left, llvm::Value* right);
    
    // Native concat BoxedValue → BoxedValue*
    llvm::Value* boxed_concat_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    
    // BoxedValue concat Native → BoxedValue*
    llvm::Value* boxed_concat_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    
    // Mixed-type logical operations (return i1 boolean)
    // Helper function to convert any BoxedValue to boolean
    llvm::Value* boxedValueToBool(llvm::Value* boxed);
    
    // BoxedValue logical BoxedValue → i1
    llvm::Value* boxed_and(llvm::Value* left, llvm::Value* right);
    llvm::Value* boxed_or(llvm::Value* left, llvm::Value* right);
    
    // Native logical BoxedValue → i1
    llvm::Value* boxed_and_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    llvm::Value* boxed_or_native_left(llvm::Value* nativeVal, llvm::Value* boxed);
    
    // BoxedValue logical Native → i1
    llvm::Value* boxed_and_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    llvm::Value* boxed_or_native_right(llvm::Value* boxed, llvm::Value* nativeVal);
    
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
    int getAttributeIndex(llvm::StructType* structType, const std::string& attrName);
    void storeTypeMetadata(const std::string& typeName, const std::vector<std::string>& attrNames);
    std::string getObjectTypeName(llvm::Value* objectPtr);
    std::string extractTypeNameFromLLVMType(llvm::Type* llvmType);
    
    // Inheritance and polymorphism support
    void addBaseTypeFields(const std::string& baseTypeName, std::vector<llvm::Type*>& fieldTypes, std::vector<std::string>& attributeNames);
    void generateVTable(TypeDecl* typeDecl);
    llvm::GlobalVariable* getOrCreateVTable(const std::string& typeName);
    std::vector<std::string> collectAllMethods(TypeDecl* typeDecl);
    std::vector<std::string> getInheritanceChain(const std::string& typeName);
    llvm::Value* toStringLLVM(llvm::Value* value);
    
    // Type conversion functions for destructive assignments
    llvm::Value* convertToInt(llvm::Value* value);
    llvm::Value* convertToDouble(llvm::Value* value);
    llvm::Value* convertToString(llvm::Value* value);
    llvm::Value* convertToBool(llvm::Value* value);
    llvm::Value* convertValue(llvm::Value* value, llvm::Type* targetType);
    
    // Built-in functions
    void registerBuiltinFunctions();
    void createPrintBoxedFunction();
    void createUnboxFunction();
    void createTypeCheckFunctions();
    void createTypeSpecificUnboxFunctions();
    
    // Special function handlers
    void handlePrintFunction(CallExpr *expr);
    void handleRandFunction(CallExpr *expr);

    // Runtime type tracking for LLVM 19 opaque pointers
    std::unordered_map<llvm::Value*, std::string> value_type_map_; // Track value -> type name
    std::unordered_map<std::string, std::string> variable_type_map_; // Track variable name -> type name in current context
    
    // Type tracking helpers
    void trackValueType(llvm::Value* value, const std::string& typeName);
    std::string getTrackedValueType(llvm::Value* value);
    void trackVariableType(const std::string& varName, const std::string& typeName);
    std::string getTrackedVariableType(const std::string& varName);

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
    std::unordered_map<std::string, std::string> variable_object_types; // variable name -> object type name
    LLVMScope* parent;

    LLVMScope(LLVMScope* p = nullptr) : parent(p) {}
}; 