#include "../Types/boxed_value.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>

// External C functions for LLVM to call
extern "C" {

// Boxing functions
BoxedValue* boxNumber(double value) {
    return BoxedValueOps::boxNumber(value);
}

BoxedValue* boxString(char* value) {
    return BoxedValueOps::boxString(value);
}

BoxedValue* boxBoolean(bool value) {
    return BoxedValueOps::boxBoolean(value);
}

BoxedValue* boxObject(void* value) {
    return BoxedValueOps::boxObject(value);
}

BoxedValue* boxNull() {
    return BoxedValueOps::boxNull();
}

// Unboxing functions
double unboxNumber(const BoxedValue* box) {
    return BoxedValueOps::unboxNumber(box);
}

char* unboxString(const BoxedValue* box) {
    return BoxedValueOps::unboxString(box);
}

bool unboxBoolean(const BoxedValue* box) {
    return BoxedValueOps::unboxBoolean(box);
}

void* unboxObject(const BoxedValue* box) {
    return BoxedValueOps::unboxObject(box);
}

// Type checking functions
bool isNumber(const BoxedValue* box) {
    return BoxedValueOps::isNumber(box);
}

bool isString(const BoxedValue* box) {
    return BoxedValueOps::isString(box);
}

bool isBoolean(const BoxedValue* box) {
    return BoxedValueOps::isBoolean(box);
}

bool isObject(const BoxedValue* box) {
    return BoxedValueOps::isObject(box);
}

bool isNull(const BoxedValue* box) {
    return BoxedValueOps::isNull(box);
}

// Arithmetic operations
BoxedValue* boxedAdd(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::add(left, right);
}

BoxedValue* boxedSubtract(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::subtract(left, right);
}

BoxedValue* boxedMultiply(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::multiply(left, right);
}

BoxedValue* boxedDivide(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::divide(left, right);
}

// Comparison operations
BoxedValue* boxedEqual(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::equal(left, right);
}

BoxedValue* boxedNotEqual(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::notEqual(left, right);
}

BoxedValue* boxedLessThan(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::lessThan(left, right);
}

BoxedValue* boxedGreaterThan(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::greaterThan(left, right);
}

BoxedValue* boxedLessEqual(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::lessEqual(left, right);
}

BoxedValue* boxedGreaterEqual(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::greaterEqual(left, right);
}

// Logical operations
BoxedValue* boxedLogicalAnd(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::logicalAnd(left, right);
}

BoxedValue* boxedLogicalOr(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::logicalOr(left, right);
}

BoxedValue* boxedLogicalNot(const BoxedValue* operand) {
    return BoxedValueOps::logicalNot(operand);
}

// String operations
BoxedValue* boxedConcatenate(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::concatenate(left, right);
}

BoxedValue* boxedConcatenateWithSpace(const BoxedValue* left, const BoxedValue* right) {
    return BoxedValueOps::concatenateWithSpace(left, right);
}

// Memory management
void freeBoxedValue(BoxedValue* box) {
    BoxedValueOps::freeBoxedValue(box);
}

BoxedValue* copyBoxedValue(const BoxedValue* box) {
    return BoxedValueOps::copyBoxedValue(box);
}

// Type conversion
BoxedValue* convertToString(const BoxedValue* box) {
    return BoxedValueOps::convertToString(box);
}

BoxedValue* convertToNumber(const BoxedValue* box) {
    return BoxedValueOps::convertToNumber(box);
}

BoxedValue* convertToBoolean(const BoxedValue* box) {
    return BoxedValueOps::convertToBoolean(box);
}

} // extern "C" 