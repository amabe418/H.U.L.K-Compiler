#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include "../AST/ast.hpp"
#include "../Types/type_info.hpp"
#include "../Types/boxed_value.hpp"

// Forward declaration
class Scope;

class CodeGenerator : public StmtVisitor, public ExprVisitor
{
private:
    std::stringstream ir_code_;
    std::stringstream global_constants_;     // Separate stream for global constants
    std::stringstream function_definitions_; // Separate stream for function definitions
    std::string module_name_;

    // Variable counter for unique names
    int var_counter_ = 0;

    // Function declarations
    std::unordered_map<std::string, std::string> functions_;

    // Function signatures - store parameter types for each function
    std::unordered_map<std::string, std::vector<TypeInfo::Kind>> function_signatures_;

    // Type declarations
    std::unordered_map<std::string, std::string> types_;

    // Attribute type mappings for each type
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> attribute_types_;

    // Parameter to attribute mappings for each type
    std::unordered_map<std::string, std::vector<std::string>> parameter_to_attribute_mappings_;

    // Attribute name to index mappings for each type
    std::unordered_map<std::string, std::unordered_map<std::string, int>> attribute_indices_;

    // Type declarations for accessing attribute initializers
    std::unordered_map<std::string, TypeDecl *> type_declarations_;

    // Inheritance relationships (child -> parent)
    std::unordered_map<std::string, std::string> type_inheritance_;

    // Current function being generated
    std::string current_function_;

    // Current type being generated
    std::string current_type_;

    // Context variables for attribute initialization in constructors
    std::string current_object_ptr_;
    std::string current_struct_type_;
    int current_attr_index_;
    bool in_constructor_context_ = false;

    // Current return value for expressions
    std::string current_value_;

    // Scope management for variables
    Scope *current_scope_;
    std::vector<std::unique_ptr<Scope>> scopes_;

    // Track generated string constants
    std::set<std::string> generated_strings_;

    // Track if built-in functions have been registered
    bool builtins_registered_ = false;

    // Boxed value management
    std::unordered_map<std::string, std::string> boxed_value_types_; // variable name -> boxed type info

    // ===== NEW: VTable and Inheritance Support =====
    
    // VTable management
    std::unordered_map<std::string, std::string> type_vtables_; // type name -> vtable name
    std::unordered_map<std::string, std::vector<std::string>> type_methods_; // type name -> method names
    std::unordered_map<std::string, std::string> method_implementations_; // method name -> LLVM function name
    std::unordered_map<std::string, int> method_indices_; // method name -> vtable index
    
    // Type layout management (flattened inheritance)
    std::unordered_map<std::string, std::vector<std::string>> type_layouts_; // type name -> attribute names (including inherited)
    std::unordered_map<std::string, std::unordered_map<std::string, int>> attribute_offsets_; // type name -> attribute name -> offset
    
    // Type IDs for runtime type checking
    std::unordered_map<std::string, int> type_ids_;
    int next_type_id_ = 1;

    // Built-in functions
    void registerBuiltinFunctions();

    // Helper methods
    std::string generateUniqueName(const std::string &base);
    std::string getLLVMType(const TypeInfo &type);
    std::string getAttributeLLVMType(const std::string &typeName, const std::string &attrName);
    std::string createConstant(const TypeInfo &type, const std::string &value);
    void enterScope();
    void exitScope();
    std::string registerStringConstant(const std::string &value);
    std::stringstream &getCurrentStream();                                        // Get current output stream
    void generateConstructorFunction(TypeDecl *typeDecl);                         // Generate constructor function for types
    std::vector<std::string> getInheritedAttributes(const std::string &typeName); // Get all attributes including inherited ones

    // ===== NEW: VTable and Inheritance Methods =====
    
    // VTable generation
    void generateVTable(const std::string& typeName, TypeDecl* typeDecl);
    void generateVTableStructure(const std::string& typeName, const std::vector<std::string>& methods);
    void generateVTableInstance(const std::string& typeName, TypeDecl* typeDecl, const std::vector<std::string>& methods);
    
    // Method management
    std::vector<std::string> getAllMethods(const std::string& typeName);
    std::string getMethodSignature(const std::string& methodName, const std::string& typeName);
    std::string getMethodImplementation(const std::string& typeName, const std::string& methodName);
    int getMethodIndex(const std::string& methodName);
    std::string getMethodReturnType(const std::string& methodName, const std::string& typeName);
    
    // Type layout management
    void computeTypeLayout(const std::string& typeName, TypeDecl* typeDecl);
    std::vector<std::string> getFlattenedAttributes(const std::string& typeName);
    int getAttributeOffset(const std::string& typeName, const std::string& attrName);
    int getTypeSize(const std::string& typeName);
    
    // Constructor generation with VTable support
    void generateConstructorWithVTable(TypeDecl* typeDecl);
    void initializeVTablePointer(const std::string& typeName, const std::string& objectPtr);
    void initializeAttributes(const std::string& typeName, const std::string& objectPtr, TypeDecl* typeDecl);
    
    // Method dispatch
    std::string generateMethodDispatch(const std::string& objectPtr, const std::string& methodName, const std::vector<std::string>& args);
    std::string generateDirectMethodCall(const std::string& objectPtr, const std::string& methodName, const std::vector<std::string>& args, const std::string& objectType);
    bool isTypeKnownAtCompileTime(const std::string& typeName);
    
    // Type checking support
    int getTypeId(const std::string& typeName);
    std::string generateTypeCheck(const std::string& objectPtr, const std::string& targetType);
    std::string generateTypeCast(const std::string& objectPtr, const std::string& targetType);

public:
    CodeGenerator();
    ~CodeGenerator();

    // Initialize the code generator
    void initialize(const std::string &module_name);

    // Generate code for a program
    void generateCode(Program *program);

    // Print the generated IR
    void printIR();

    // Write IR to file
    void writeIRToFile(const std::string &filename);

    // Get the generated IR as string
    std::string getIR() const { return ir_code_.str(); }

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

    // Boxed value support
    std::string generateBoxedValue(const std::string &value, const TypeInfo &type);
    std::string generateUnboxedValue(const std::string &boxed_value, const TypeInfo &target_type);
    std::string generateBoxedValueType(const TypeInfo &type);
    std::string generateBoxedValueAlloca(const std::string &var_name);
    std::string generateBoxedValueStore(const std::string &var_name, const std::string &value, const TypeInfo &type);
    std::string generateBoxedValueLoad(const std::string &var_name);
    std::string generateBoxedValueOperation(const std::string &left, const std::string &right, const std::string &operation, const TypeInfo &left_type, const TypeInfo &right_type);
};

// Scope structure for variable management
class Scope
{
public:
    std::unordered_map<std::string, std::string> variables;       // variable name -> alloca name
    std::unordered_map<std::string, std::string> boxed_variables; // variable name -> boxed alloca name
    std::unordered_map<std::string, TypeInfo> variable_types;     // variable name -> type info
    Scope *parent;

    Scope(Scope *p = nullptr) : parent(p) {}
};
