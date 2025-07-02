#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>

/**
 * @brief Runtime type information for boxed values
 */
enum class RuntimeType : uint8_t {
    NUMBER = 0,
    STRING = 1,
    BOOLEAN = 2,
    OBJECT = 3,
    NULL_VALUE = 4,
    UNKNOWN = 5
};

/**
 * @brief Boxed value structure for dynamic typing
 * This allows us to handle values of unknown types at compile time
 * by boxing them with runtime type information
 */
struct BoxedValue {
    RuntimeType type;
    union {
        double number;
        char* string;
        bool boolean;
        void* object;
    } value;
    
    // Constructor for numbers
    explicit BoxedValue(double num) : type(RuntimeType::NUMBER) {
        value.number = num;
    }
    
    // Constructor for strings
    explicit BoxedValue(char* str) : type(RuntimeType::STRING) {
        value.string = str;
    }
    
    // Constructor for booleans
    explicit BoxedValue(bool b) : type(RuntimeType::BOOLEAN) {
        value.boolean = b;
    }
    
    // Constructor for objects
    explicit BoxedValue(void* obj) : type(RuntimeType::OBJECT) {
        value.object = obj;
    }
    
    // Default constructor for null/unknown
    BoxedValue() : type(RuntimeType::NULL_VALUE) {
        value.object = nullptr;
    }
    
    // Copy constructor
    BoxedValue(const BoxedValue& other) : type(other.type) {
        switch (type) {
            case RuntimeType::NUMBER:
                value.number = other.value.number;
                break;
            case RuntimeType::STRING:
                value.string = other.value.string;
                break;
            case RuntimeType::BOOLEAN:
                value.boolean = other.value.boolean;
                break;
            case RuntimeType::OBJECT:
            case RuntimeType::NULL_VALUE:
            case RuntimeType::UNKNOWN:
                value.object = other.value.object;
                break;
        }
    }
    
    // Assignment operator
    BoxedValue& operator=(const BoxedValue& other) {
        if (this != &other) {
            type = other.type;
            switch (type) {
                case RuntimeType::NUMBER:
                    value.number = other.value.number;
                    break;
                case RuntimeType::STRING:
                    value.string = other.value.string;
                    break;
                case RuntimeType::BOOLEAN:
                    value.boolean = other.value.boolean;
                    break;
                case RuntimeType::OBJECT:
                case RuntimeType::NULL_VALUE:
                case RuntimeType::UNKNOWN:
                    value.object = other.value.object;
                    break;
            }
        }
        return *this;
    }
    
    // Destructor
    ~BoxedValue() = default;
    
    // Type checking methods
    bool isNumber() const { return type == RuntimeType::NUMBER; }
    bool isString() const { return type == RuntimeType::STRING; }
    bool isBoolean() const { return type == RuntimeType::BOOLEAN; }
    bool isObject() const { return type == RuntimeType::OBJECT; }
    bool isNull() const { return type == RuntimeType::NULL_VALUE; }
    
    // Value extraction methods (with type checking)
    double getNumber() const {
        if (type != RuntimeType::NUMBER) {
            throw std::runtime_error("Attempted to get number from non-number boxed value");
        }
        return value.number;
    }
    
    char* getString() const {
        if (type != RuntimeType::STRING) {
            throw std::runtime_error("Attempted to get string from non-string boxed value");
        }
        return value.string;
    }
    
    bool getBoolean() const {
        if (type != RuntimeType::BOOLEAN) {
            throw std::runtime_error("Attempted to get boolean from non-boolean boxed value");
        }
        return value.boolean;
    }
    
    void* getObject() const {
        if (type != RuntimeType::OBJECT) {
            throw std::runtime_error("Attempted to get object from non-object boxed value");
        }
        return value.object;
    }
    
    // String representation for debugging
    std::string toString() const {
        switch (type) {
            case RuntimeType::NUMBER:
                return "Number(" + std::to_string(value.number) + ")";
            case RuntimeType::STRING:
                return "String(" + std::string(value.string) + ")";
            case RuntimeType::BOOLEAN:
                return "Boolean(" + std::string(value.boolean ? "true" : "false") + ")";
            case RuntimeType::OBJECT:
                return "Object(" + std::to_string(reinterpret_cast<uintptr_t>(value.object)) + ")";
            case RuntimeType::NULL_VALUE:
                return "Null";
            case RuntimeType::UNKNOWN:
                return "Unknown";
        }
        return "Invalid";
    }
};

/**
 * @brief Boxed value operations
 * These functions provide safe operations on boxed values
 */
namespace BoxedValueOps {
    
    // Boxing functions
    BoxedValue* boxNumber(double value);
    BoxedValue* boxString(char* value);
    BoxedValue* boxBoolean(bool value);
    BoxedValue* boxObject(void* value);
    BoxedValue* boxNull();
    
    // Unboxing functions
    double unboxNumber(const BoxedValue* box);
    char* unboxString(const BoxedValue* box);
    bool unboxBoolean(const BoxedValue* box);
    void* unboxObject(const BoxedValue* box);
    
    // Type checking functions
    bool isNumber(const BoxedValue* box);
    bool isString(const BoxedValue* box);
    bool isBoolean(const BoxedValue* box);
    bool isObject(const BoxedValue* box);
    bool isNull(const BoxedValue* box);
    
    // Arithmetic operations
    BoxedValue* add(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* subtract(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* multiply(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* divide(const BoxedValue* left, const BoxedValue* right);
    
    // Comparison operations
    BoxedValue* equal(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* notEqual(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* lessThan(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* greaterThan(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* lessEqual(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* greaterEqual(const BoxedValue* left, const BoxedValue* right);
    
    // Logical operations
    BoxedValue* logicalAnd(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* logicalOr(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* logicalNot(const BoxedValue* operand);
    
    // String operations
    BoxedValue* concatenate(const BoxedValue* left, const BoxedValue* right);
    BoxedValue* concatenateWithSpace(const BoxedValue* left, const BoxedValue* right);
    
    // Memory management
    void freeBoxedValue(BoxedValue* box);
    BoxedValue* copyBoxedValue(const BoxedValue* box);
    
    // Type conversion
    BoxedValue* convertToString(const BoxedValue* box);
    BoxedValue* convertToNumber(const BoxedValue* box);
    BoxedValue* convertToBoolean(const BoxedValue* box);
} 