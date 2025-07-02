#include "boxed_value.hpp"
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <iostream>

namespace BoxedValueOps {

// Boxing functions
BoxedValue* boxNumber(double value) {
    BoxedValue* box = new BoxedValue(value);
    return box;
}

BoxedValue* boxString(char* value) {
    BoxedValue* box = new BoxedValue(value);
    return box;
}

BoxedValue* boxBoolean(bool value) {
    BoxedValue* box = new BoxedValue(value);
    return box;
}

BoxedValue* boxObject(void* value) {
    BoxedValue* box = new BoxedValue(value);
    return box;
}

BoxedValue* boxNull() {
    BoxedValue* box = new BoxedValue();
    return box;
}

// Unboxing functions
double unboxNumber(const BoxedValue* box) {
    if (!box || !box->isNumber()) {
        throw std::runtime_error("Cannot unbox non-number value as number");
    }
    return box->getNumber();
}

char* unboxString(const BoxedValue* box) {
    if (!box || !box->isString()) {
        throw std::runtime_error("Cannot unbox non-string value as string");
    }
    return box->getString();
}

bool unboxBoolean(const BoxedValue* box) {
    if (!box || !box->isBoolean()) {
        throw std::runtime_error("Cannot unbox non-boolean value as boolean");
    }
    return box->getBoolean();
}

void* unboxObject(const BoxedValue* box) {
    if (!box || !box->isObject()) {
        throw std::runtime_error("Cannot unbox non-object value as object");
    }
    return box->getObject();
}

// Type checking functions
bool isNumber(const BoxedValue* box) {
    return box && box->isNumber();
}

bool isString(const BoxedValue* box) {
    return box && box->isString();
}

bool isBoolean(const BoxedValue* box) {
    return box && box->isBoolean();
}

bool isObject(const BoxedValue* box) {
    return box && box->isObject();
}

bool isNull(const BoxedValue* box) {
    return box && box->isNull();
}

// Arithmetic operations
BoxedValue* add(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot add null values");
    }
    
    if (left->isNumber() && right->isNumber()) {
        return boxNumber(left->getNumber() + right->getNumber());
    }
    
    if (left->isString() || right->isString()) {
        // Convert both to strings and concatenate
        char* leftStr = convertToString(left)->getString();
        char* rightStr = convertToString(right)->getString();
        
        size_t leftLen = strlen(leftStr);
        size_t rightLen = strlen(rightStr);
        char* result = new char[leftLen + rightLen + 1];
        
        strcpy(result, leftStr);
        strcat(result, rightStr);
        
        return boxString(result);
    }
    
    throw std::runtime_error("Invalid operands for addition");
}

BoxedValue* subtract(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot subtract null values");
    }
    
    if (left->isNumber() && right->isNumber()) {
        return boxNumber(left->getNumber() - right->getNumber());
    }
    
    throw std::runtime_error("Invalid operands for subtraction");
}

BoxedValue* multiply(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot multiply null values");
    }
    
    if (left->isNumber() && right->isNumber()) {
        return boxNumber(left->getNumber() * right->getNumber());
    }
    
    throw std::runtime_error("Invalid operands for multiplication");
}

BoxedValue* divide(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot divide null values");
    }
    
    if (left->isNumber() && right->isNumber()) {
        if (right->getNumber() == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return boxNumber(left->getNumber() / right->getNumber());
    }
    
    throw std::runtime_error("Invalid operands for division");
}

// Comparison operations
BoxedValue* equal(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        return boxBoolean(left == right);
    }
    
    if (left->isNumber() && right->isNumber()) {
        return boxBoolean(left->getNumber() == right->getNumber());
    }
    
    if (left->isString() && right->isString()) {
        return boxBoolean(strcmp(left->getString(), right->getString()) == 0);
    }
    
    if (left->isBoolean() && right->isBoolean()) {
        return boxBoolean(left->getBoolean() == right->getBoolean());
    }
    
    if (left->isObject() && right->isObject()) {
        return boxBoolean(left->getObject() == right->getObject());
    }
    
    return boxBoolean(false);
}

BoxedValue* notEqual(const BoxedValue* left, const BoxedValue* right) {
    BoxedValue* eq = equal(left, right);
    bool result = !eq->getBoolean();
    delete eq;
    return boxBoolean(result);
}

BoxedValue* lessThan(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot compare null values");
    }
    
    if (left->isNumber() && right->isNumber()) {
        return boxBoolean(left->getNumber() < right->getNumber());
    }
    
    if (left->isString() && right->isString()) {
        return boxBoolean(strcmp(left->getString(), right->getString()) < 0);
    }
    
    throw std::runtime_error("Invalid operands for less than comparison");
}

BoxedValue* greaterThan(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot compare null values");
    }
    
    if (left->isNumber() && right->isNumber()) {
        return boxBoolean(left->getNumber() > right->getNumber());
    }
    
    if (left->isString() && right->isString()) {
        return boxBoolean(strcmp(left->getString(), right->getString()) > 0);
    }
    
    throw std::runtime_error("Invalid operands for greater than comparison");
}

BoxedValue* lessEqual(const BoxedValue* left, const BoxedValue* right) {
    BoxedValue* gt = greaterThan(left, right);
    bool result = !gt->getBoolean();
    delete gt;
    return boxBoolean(result);
}

BoxedValue* greaterEqual(const BoxedValue* left, const BoxedValue* right) {
    BoxedValue* lt = lessThan(left, right);
    bool result = !lt->getBoolean();
    delete lt;
    return boxBoolean(result);
}

// Logical operations
BoxedValue* logicalAnd(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot perform logical AND on null values");
    }
    
    bool leftBool = convertToBoolean(left)->getBoolean();
    bool rightBool = convertToBoolean(right)->getBoolean();
    
    return boxBoolean(leftBool && rightBool);
}

BoxedValue* logicalOr(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot perform logical OR on null values");
    }
    
    bool leftBool = convertToBoolean(left)->getBoolean();
    bool rightBool = convertToBoolean(right)->getBoolean();
    
    return boxBoolean(leftBool || rightBool);
}

BoxedValue* logicalNot(const BoxedValue* operand) {
    if (!operand) {
        throw std::runtime_error("Cannot perform logical NOT on null value");
    }
    
    bool boolVal = convertToBoolean(operand)->getBoolean();
    return boxBoolean(!boolVal);
}

// String operations
BoxedValue* concatenate(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot concatenate null values");
    }
    
    char* leftStr = convertToString(left)->getString();
    char* rightStr = convertToString(right)->getString();
    
    size_t leftLen = strlen(leftStr);
    size_t rightLen = strlen(rightStr);
    char* result = new char[leftLen + rightLen + 1];
    
    strcpy(result, leftStr);
    strcat(result, rightStr);
    
    return boxString(result);
}

BoxedValue* concatenateWithSpace(const BoxedValue* left, const BoxedValue* right) {
    if (!left || !right) {
        throw std::runtime_error("Cannot concatenate null values");
    }
    
    char* leftStr = convertToString(left)->getString();
    char* rightStr = convertToString(right)->getString();
    
    size_t leftLen = strlen(leftStr);
    size_t rightLen = strlen(rightStr);
    char* result = new char[leftLen + rightLen + 2]; // +2 for space and null terminator
    
    strcpy(result, leftStr);
    strcat(result, " ");
    strcat(result, rightStr);
    
    return boxString(result);
}

// Memory management
void freeBoxedValue(BoxedValue* box) {
    if (box) {
        delete box;
    }
}

BoxedValue* copyBoxedValue(const BoxedValue* box) {
    if (!box) {
        return nullptr;
    }
    
    return new BoxedValue(*box);
}

// Type conversion
BoxedValue* convertToString(const BoxedValue* box) {
    if (!box) {
        return boxString(strdup("null"));
    }
    
    switch (box->type) {
        case RuntimeType::NUMBER: {
            char* str = new char[64];
            sprintf(str, "%g", box->getNumber());
            return boxString(str);
        }
        case RuntimeType::STRING:
            return boxString(strdup(box->getString()));
        case RuntimeType::BOOLEAN:
            return boxString(strdup(box->getBoolean() ? "true" : "false"));
        case RuntimeType::OBJECT: {
            char* str = new char[64];
            sprintf(str, "object_%p", box->getObject());
            return boxString(str);
        }
        case RuntimeType::NULL_VALUE:
            return boxString(strdup("null"));
        default:
            return boxString(strdup("unknown"));
    }
}

BoxedValue* convertToNumber(const BoxedValue* box) {
    if (!box) {
        return boxNumber(0.0);
    }
    
    switch (box->type) {
        case RuntimeType::NUMBER:
            return boxNumber(box->getNumber());
        case RuntimeType::STRING: {
            char* endptr;
            double num = strtod(box->getString(), &endptr);
            if (*endptr == '\0') {
                return boxNumber(num);
            }
            return boxNumber(0.0);
        }
        case RuntimeType::BOOLEAN:
            return boxNumber(box->getBoolean() ? 1.0 : 0.0);
        case RuntimeType::OBJECT:
            return boxNumber(static_cast<double>(reinterpret_cast<uintptr_t>(box->getObject())));
        case RuntimeType::NULL_VALUE:
            return boxNumber(0.0);
        default:
            return boxNumber(0.0);
    }
}

BoxedValue* convertToBoolean(const BoxedValue* box) {
    if (!box) {
        return boxBoolean(false);
    }
    
    switch (box->type) {
        case RuntimeType::NUMBER:
            return boxBoolean(box->getNumber() != 0.0);
        case RuntimeType::STRING:
            return boxBoolean(strlen(box->getString()) > 0);
        case RuntimeType::BOOLEAN:
            return boxBoolean(box->getBoolean());
        case RuntimeType::OBJECT:
            return boxBoolean(box->getObject() != nullptr);
        case RuntimeType::NULL_VALUE:
            return boxBoolean(false);
        default:
            return boxBoolean(false);
    }
}

} // namespace BoxedValueOps 