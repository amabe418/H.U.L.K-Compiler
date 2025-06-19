#pragma once
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Type information for HULK language types
 */
class TypeInfo
{
public:
    enum class Kind
    {
        Number,
        String,
        Boolean,
        Function,
        Object,
        Null,
        Unknown,
        Void
    };

private:
    Kind kind_;
    std::string typeName_; // For object types
    std::vector<TypeInfo> parameter_types_;
    std::unique_ptr<TypeInfo> return_type_;
    std::string typeAssigned_;

public:
    explicit TypeInfo(Kind k = Kind::Unknown, const std::string &name = "", const std::string &assigned = "")
        : kind_(k), typeName_(name), typeAssigned_(assigned) {}
    // Copy constructor
    TypeInfo(const TypeInfo &other) : kind_(other.kind_), typeName_(other.typeName_), parameter_types_(other.parameter_types_)
    {
        if (other.return_type_)
        {
            return_type_ = std::make_unique<TypeInfo>(*other.return_type_);
        }
    }
    // Copy assignment operator
    TypeInfo &operator=(const TypeInfo &other)
    {
        if (this != &other)
        {
            kind_ = other.kind_;
            typeName_ = other.typeName_;
            parameter_types_ = other.parameter_types_;
            if (other.return_type_)
            {
                return_type_ = std::make_unique<TypeInfo>(*other.return_type_);
            }
            else
            {
                return_type_.reset();
            }
        }
        return *this;
    }
    // Move constructor
    TypeInfo(TypeInfo &&other) noexcept
        : kind_(other.kind_), typeName_(std::move(other.typeName_)), parameter_types_(std::move(other.parameter_types_)),
          return_type_(std::move(other.return_type_)) {}
    // Move assignment operator
    TypeInfo &operator=(TypeInfo &&other) noexcept
    {
        if (this != &other)
        {
            kind_ = other.kind_;
            typeName_ = std::move(other.typeName_);
            parameter_types_ = std::move(other.parameter_types_);
            return_type_ = std::move(other.return_type_);
        }
        return *this;
    }

    // For function types
    TypeInfo(const std::vector<TypeInfo> &params, const TypeInfo &ret)
        : kind_(Kind::Function), parameter_types_(params), return_type_(std::make_unique<TypeInfo>(ret)) {}
    Kind getKind() const { return kind_; }
    const std::string &getTypeName() const { return typeName_; }

    bool isNumeric() const { return kind_ == Kind::Number; }
    bool isString() const { return kind_ == Kind::String; }
    bool isBoolean() const { return kind_ == Kind::Boolean; }
    bool isFunction() const { return kind_ == Kind::Function; }
    bool isObject() const { return kind_ == Kind::Object; }
    bool isNull() const { return kind_ == Kind::Null; }
    bool isUnknown() const { return kind_ == Kind::Unknown; }

    /**
     * @brief Check if this type is compatible with another type
     */
    bool isCompatibleWith(const TypeInfo &other) const
    {
        if (kind_ == Kind::Unknown || other.kind_ == Kind::Unknown)
        {
            return true; // Unknown types are compatible with everything
        }
        if (kind_ == other.kind_)
        {
            if (kind_ == Kind::Object)
            {
                return typeName_ == other.typeName_;
            }
            return true;
        }

        // Special compatibility rules
        if (kind_ == Kind::Null || other.kind_ == Kind::Null)
        {
            return true; // Null is compatible with any type
        }

        return false;
    }

    /**
     * @brief Convert type to string representation
     */
    std::string toString() const
    {
        switch (kind_)
        {
        case Kind::Number:
            return "Number";
        case Kind::String:
            return "String";
        case Kind::Boolean:
            return "Boolean";
        case Kind::Function:
            return "Function";
        case Kind::Object:
            return typeName_.empty() ? "Object" : typeName_;
        case Kind::Null:
            return "Null";
        case Kind::Unknown:
            return "Unknown";
        case Kind::Void:
            return "Void";
        }
        return "Unknown";
    }

    // Static factory methods
    static TypeInfo Number() { return TypeInfo(Kind::Number); }
    static TypeInfo String() { return TypeInfo(Kind::String); }
    static TypeInfo Boolean() { return TypeInfo(Kind::Boolean); }
    static TypeInfo Void() { return TypeInfo(Kind::Void); }
    static TypeInfo Object(const std::string &assigned = "")
    {
        TypeInfo obj(Kind::Object);
        obj.setTypeAssigned(assigned);
        return obj;
    }

    /**
     * @brief Create TypeInfo from string
     */
    static TypeInfo fromString(const std::string &str)
    {
        if (str == "Number" || str == "number")
            return TypeInfo(Kind::Number);
        if (str == "String" || str == "string")
            return TypeInfo(Kind::String);
        if (str == "Boolean" || str == "boolean")
            return TypeInfo(Kind::Boolean);
        if (str == "Function" || str == "function")
            return TypeInfo(Kind::Function);
        if (str == "Null" || str == "null")
            return TypeInfo(Kind::Null);
        return TypeInfo(Kind::Unknown);
    }
    /**
     * @brief Infer type for binary operations
     */
    static TypeInfo inferBinaryOp(const std::string &op, const TypeInfo &left, const TypeInfo &right)
    {
        // Arithmetic operations
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "^" ||
            op == "//" || op == "%%")
        {
            if (!(left.isNumeric() || left.isUnknown()) && (right.isNumeric() || right.isUnknown()))
            {
            }
            return TypeInfo(Kind::Number);
        }

        // String concatenation
        if (op == "@" || op == "@@")
        {
            if (left.isString() || right.isString())
            {
                return TypeInfo(Kind::String);
            }
        }

        // Comparison operations
        if (op == "==" || op == "!=")
        {
            return TypeInfo(Kind::Boolean);
        }

        if (op == "<" || op == ">" || op == "<=" || op == ">=")
        {
            return TypeInfo(Kind::Boolean);
        }

        // Logical operations
        if (op == "&&" || op == "||")
        {
            return TypeInfo(Kind::Boolean);
        }

        return TypeInfo(Kind::Unknown);
    }

    /**
     * @brief Infer type for unary operations
     */
    static TypeInfo inferUnaryOp(const std::string &op, const TypeInfo &operand)
    {
        if (op == "-")
        {
            return operand.isNumeric() ? TypeInfo(Kind::Number) : TypeInfo(Kind::Unknown);
        }

        if (op == "!" || op == "not")
        {
            return TypeInfo(Kind::Boolean);
        }

        return TypeInfo(Kind::Unknown);
    }

    void setTypeAssigned(const std::string &assigned) { typeAssigned_ = assigned; }
    const std::string &getTypeAssigned() const { return typeAssigned_; }
};