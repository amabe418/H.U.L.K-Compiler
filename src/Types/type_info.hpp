// type_info.hpp
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declaration
struct FunctionDecl;

struct TypeInfo
{
    enum class Kind
    {
        Int,
        Float,
        Bool,
        String,
        Void,
        Function,
        Class,
        Error,
        Any
    };

    Kind kind;
    std::string name;
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    std::shared_ptr<TypeInfo> returnType;
    std::unordered_map<std::string, std::shared_ptr<TypeInfo>> attributes;
    std::unordered_map<std::string, std::shared_ptr<TypeInfo>> methods;
    std::shared_ptr<TypeInfo> baseType;

    TypeInfo() : kind(Kind::Error), baseType(nullptr) {}
    TypeInfo(Kind k) : kind(k), baseType(nullptr) {}
    TypeInfo(Kind k, const std::string& n) : kind(k), name(n), baseType(nullptr) {}
    TypeInfo(Kind k, const std::vector<std::shared_ptr<TypeInfo>>& params,
             const std::shared_ptr<TypeInfo>& ret)
        : kind(k), paramTypes(params), returnType(ret), baseType(nullptr)
    {
    }
    TypeInfo(Kind k, const std::string& n, const std::shared_ptr<TypeInfo>& base)
        : kind(k), name(n), baseType(base)
    {
    }

    static std::shared_ptr<TypeInfo>
    Int()
    {
        return std::make_shared<TypeInfo>(Kind::Int);
    }
    static std::shared_ptr<TypeInfo>
    Float()
    {
        return std::make_shared<TypeInfo>(Kind::Float);
    }
    static std::shared_ptr<TypeInfo>
    Bool()
    {
        return std::make_shared<TypeInfo>(Kind::Bool);
    }
    static std::shared_ptr<TypeInfo>
    String()
    {
        return std::make_shared<TypeInfo>(Kind::String);
    }
    static std::shared_ptr<TypeInfo>
    Void()
    {
        return std::make_shared<TypeInfo>(Kind::Void);
    }
    static std::shared_ptr<TypeInfo>
    Error()
    {
        return std::make_shared<TypeInfo>(Kind::Error);
    }
    static std::shared_ptr<TypeInfo>
    Any()
    {
        return std::make_shared<TypeInfo>(Kind::Any);
    }
    static std::shared_ptr<TypeInfo>
    Function(const std::vector<std::shared_ptr<TypeInfo>>& params,
             const std::shared_ptr<TypeInfo>& ret)
    {
        return std::make_shared<TypeInfo>(Kind::Function, params, ret);
    }
    static std::shared_ptr<TypeInfo>
    Class(const std::string& name, const std::shared_ptr<TypeInfo>& base)
    {
        return std::make_shared<TypeInfo>(Kind::Class, name, base);
    }
    static std::shared_ptr<TypeInfo>
    Object()
    {
        return std::make_shared<TypeInfo>(Kind::Class, "Object", nullptr);
    }
    static std::shared_ptr<TypeInfo>
    UserDefined(const std::string& name)
    {
        return std::make_shared<TypeInfo>(Kind::Class, name, nullptr);
    }

    bool
    isFunction() const
    {
        return kind == Kind::Function;
    }
    bool
    isClass() const
    {
        return kind == Kind::Class;
    }
    bool
    hasBase() const
    {
        return baseType != nullptr;
    }

    const std::vector<std::shared_ptr<TypeInfo>>&
    getParamTypes() const
    {
        return paramTypes;
    }
    const std::shared_ptr<TypeInfo>&
    getReturnType() const
    {
        return returnType;
    }
    const std::shared_ptr<TypeInfo>*
    getAttributeType(const std::string& name) const
    {
        auto it = attributes.find(name);
        return it != attributes.end() ? &it->second : nullptr;
    }
    const std::shared_ptr<TypeInfo>*
    getMethodType(const std::string& name) const
    {
        auto it = methods.find(name);
        return it != methods.end() ? &it->second : nullptr;
    }
    const std::shared_ptr<TypeInfo>*
    getBaseMethodType(const std::string& name) const
    {
        return baseType ? baseType->getMethodType(name) : nullptr;
    }

    void
    addAttribute(const std::string& name, const std::shared_ptr<TypeInfo>& type)
    {
        attributes[name] = type;
    }

    void
    addMethod(const std::string& name, const std::shared_ptr<TypeInfo>& type)
    {
        methods[name] = type;
    }

    void
    setConstructorParams(const std::vector<std::shared_ptr<TypeInfo>>& params)
    {
        paramTypes = params;
    }

    std::string
    toString() const
    {
        switch (kind)
        {
            case Kind::Int:
                return "Int";
            case Kind::Float:
                return "Float";
            case Kind::Bool:
                return "Bool";
            case Kind::String:
                return "String";
            case Kind::Void:
                return "Void";
            case Kind::Error:
                return "Error";
            case Kind::Any:
                return "Any";
            case Kind::Function:
            {
                std::string result = "Function(";
                for (size_t i = 0; i < paramTypes.size(); ++i)
                {
                    if (i > 0)
                        result += ", ";
                    result += paramTypes[i]->toString();
                }
                result += ") -> " + returnType->toString();
                return result;
            }
            case Kind::Class:
            {
                std::string result = name;
                if (baseType)
                {
                    result += " extends " + baseType->toString();
                }
                return result;
            }
            default:
                return "Unknown";
        }
    }

    bool
    operator==(const TypeInfo& other) const
    {
        if (kind != other.kind)
            return false;
        if (kind == Kind::Function)
        {
            if (paramTypes.size() != other.paramTypes.size())
                return false;
            for (size_t i = 0; i < paramTypes.size(); ++i)
            {
                if (*paramTypes[i] != *other.paramTypes[i])
                    return false;
            }
            return *returnType == *other.returnType;
        }
        if (kind == Kind::Class)
        {
            return name == other.name &&
                   ((!baseType && !other.baseType) ||
                    (baseType && other.baseType && *baseType == *other.baseType));
        }
        return true;
    }

    bool
    operator!=(const TypeInfo& other) const
    {
        return !(*this == other);
    }
};
