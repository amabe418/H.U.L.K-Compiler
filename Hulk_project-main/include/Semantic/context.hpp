#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
struct Attribute 
{
    std::string name;
    std::string type;

    Attribute(std::string name, std::string type)
        : name(std::move(name)), type(std::move(type)) {}
};

struct Method {
    std::string m_name;
    std::vector<Attribute> m_paramaters;
    std::string m_returnType;

    Method(std::string name, std::vector<Attribute> paramaters, std::string returnedtype)
        : m_name(std::move(name)), m_paramaters(std::move(paramaters)), m_returnType(std::move(returnedtype)) {}
};

struct Type {
    std::string name;
    std::map<std::string, Attribute*> atributes;
    std::vector<std::pair<std::string, Attribute*>> arguments;
    std::map<std::string, Method*> methods;
    Type* parent = nullptr;
    std::string parentInstantiation;

    Type(std::string name) : name(std::move(name)) {}
    Type() = default;

    void DefineAttribute(std::string name, std::string type);
    void DefineArgument(std::string name, std::string type);
    bool exist_Attribute(const std::string& name);
    bool exist_Argument(const std::string& name);
    bool exist_Method(const std::string& name);
    void DefineMethod(std::string name, std::string type, std::vector<Attribute> paramaters);
    Method* GetMethod(const std::string& name);
    bool isDescendantOf(const Type* other) const;
    bool operator==(const Type& other) const;
    bool operator<(const Type& other) const;
};

class Context {
public:
    std::map<std::string, Type*> types;
    std::map<std::string, Method*> methods;
    std::map<std::string, Type*> atributes;
    Context* parent = nullptr;
    Context* child = nullptr;

    const std::vector<std::string> INTERNAL_TYPES = { "Object", "Number", "String", "Boolean", "Null" };

    explicit Context(Context* parent = nullptr)
    :parent(parent){}

    Type* GetType(const std::string& name);
    Type* GetTypeOfAtribute(const std::string& name);
    Type* CreateType(const std::string& name);
    bool exist_Type(const std::string& name);
    bool exist_Atribute(const std::string& name);
    bool exist_local_Attribute(const std::string& name);
    bool Define_local_Attribute(const std::string& name, const std::string& typeName);
    bool exist_Method(const std::string& name);
    Method* GetMethod(const std::string& name);
    bool DefineMethod(const std::string& name, const std::string& type, std::vector<Attribute> paramaters);
    Context* createChildContext();
    Context* RemoveContext();
    void loadInternalTypeAndMethod();
    std::string getLeastCommonAncestor(std::vector<std::string> exp_types);
    Type* LeastCommonAncestors(Type* first, Type* second);
};
