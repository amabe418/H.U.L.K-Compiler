#include "codegen.hpp"

void CodeGenerator::registerBuiltinFunctions()
{
    if (builtins_registered_)
        return;

    // Add BoxedValue struct definition FIRST - before any other code
    global_constants_ << "%struct.BoxedValue = type { i8, [7 x i8], i64 }\n";

    // Add Object struct definition - base of inheritance chain
    global_constants_ << "%struct.Object = type { i8 }\n";

    global_constants_ << "\n";

    // Add built-in function declarations
    global_constants_ << "; Built-in function declarations\n";
    global_constants_ << "declare double @sqrt(double)\n";
    global_constants_ << "declare double @sin(double)\n";
    global_constants_ << "declare double @cos(double)\n";
    // global_constants_ << "declare double @tan(double)\n";
    global_constants_ << "declare double @exp(double)\n";
    global_constants_ << "declare double @log(double)\n";
    global_constants_ << "declare double @pow(double, double)\n";
    global_constants_ << "declare i32 @rand()\n";
    global_constants_ << "declare i32 @printf(i8*, ...)\n";
    global_constants_ << "declare i32 @sprintf(i8*, i8*, ...)\n";
    global_constants_ << "declare i8* @malloc(i32)\n";
    global_constants_ << "declare void @free(i8*)\n";
    global_constants_ << "declare double @strtod(i8*, i8**)\n";
    global_constants_ << "\n";

    // Add boxed value runtime functions
    global_constants_ << "; Boxed value runtime functions\n";

    // Boxing functions
    global_constants_ << "define %struct.BoxedValue* @boxNumber(double %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %box = call i8* @malloc(i32 16)\n";
    global_constants_ << "  %box_ptr = bitcast i8* %box to %struct.BoxedValue*\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0\n";
    global_constants_ << "  store i8 0, i8* %type_ptr\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2\n";
    global_constants_ << "  %value_int = bitcast double %value to i64\n";
    global_constants_ << "  store i64 %value_int, i64* %value_ptr\n";
    global_constants_ << "  ret %struct.BoxedValue* %box_ptr\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @boxString(i8* %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %box = call i8* @malloc(i32 16)\n";
    global_constants_ << "  %box_ptr = bitcast i8* %box to %struct.BoxedValue*\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0\n";
    global_constants_ << "  store i8 1, i8* %type_ptr\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2\n";
    global_constants_ << "  %value_int = ptrtoint i8* %value to i64\n";
    global_constants_ << "  store i64 %value_int, i64* %value_ptr\n";
    global_constants_ << "  ret %struct.BoxedValue* %box_ptr\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @boxBoolean(i1 %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %box = call i8* @malloc(i32 16)\n";
    global_constants_ << "  %box_ptr = bitcast i8* %box to %struct.BoxedValue*\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0\n";
    global_constants_ << "  store i8 2, i8* %type_ptr\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2\n";
    global_constants_ << "  %value_int = zext i1 %value to i64\n";
    global_constants_ << "  store i64 %value_int, i64* %value_ptr\n";
    global_constants_ << "  ret %struct.BoxedValue* %box_ptr\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @boxObject(i8* %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %box = call i8* @malloc(i32 16)\n";
    global_constants_ << "  %box_ptr = bitcast i8* %box to %struct.BoxedValue*\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0\n";
    global_constants_ << "  store i8 3, i8* %type_ptr\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2\n";
    global_constants_ << "  %value_int = ptrtoint i8* %value to i64\n";
    global_constants_ << "  store i64 %value_int, i64* %value_ptr\n";
    global_constants_ << "  ret %struct.BoxedValue* %box_ptr\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @boxNull() {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %box = call i8* @malloc(i32 16)\n";
    global_constants_ << "  %box_ptr = bitcast i8* %box to %struct.BoxedValue*\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0\n";
    global_constants_ << "  store i8 4, i8* %type_ptr\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2\n";
    global_constants_ << "  store i64 0, i64* %value_ptr\n";
    global_constants_ << "  ret %struct.BoxedValue* %box_ptr\n";
    global_constants_ << "}\n\n";

    // Unboxing functions
    global_constants_ << "define double @unboxNumber(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2\n";
    global_constants_ << "  %value_int = load i64, i64* %value_ptr\n";
    global_constants_ << "  %value = bitcast i64 %value_int to double\n";
    global_constants_ << "  ret double %value\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i8* @unboxString(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2\n";
    global_constants_ << "  %value_int = load i64, i64* %value_ptr\n";
    global_constants_ << "  %value = inttoptr i64 %value_int to i8*\n";
    global_constants_ << "  ret i8* %value\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @unboxBoolean(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2\n";
    global_constants_ << "  %value_int = load i64, i64* %value_ptr\n";
    global_constants_ << "  %value = trunc i64 %value_int to i1\n";
    global_constants_ << "  ret i1 %value\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i8* @unboxObject(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2\n";
    global_constants_ << "  %value_int = load i64, i64* %value_ptr\n";
    global_constants_ << "  %value = inttoptr i64 %value_int to i8*\n";
    global_constants_ << "  ret i8* %value\n";
    global_constants_ << "}\n\n";

    // Type checking functions
    global_constants_ << "define i1 @isNumber(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    // ir_code_ << "  ; Debug: Print function call\n";
    // // ir_code_ << "  %debug_msg = getelementptr [25 x i8], [25 x i8]* @.str_debug_is_number, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_msg)\n";
    global_constants_ << "  ; Check for null pointer\n";
    global_constants_ << "  %is_null = icmp eq %struct.BoxedValue* %box, null\n";
    global_constants_ << "  br i1 %is_null, label %return_false, label %check_type\n";
    global_constants_ << "return_false:\n";
    // ir_code_ << "  ; Debug: Print result for null case\n";
    // // ir_code_ << "  %debug_result_null = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result_null, i32 0)\n";
    global_constants_ << "  ret i1 false\n";
    global_constants_ << "check_type:\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0\n";
    global_constants_ << "  %type = load i8, i8* %type_ptr\n";
    // ir_code_ << "  ; Debug: Print type value\n";
    // // ir_code_ << "  %debug_type = getelementptr [20 x i8], [20 x i8]* @.str_debug_type_value, i32 0, i32 0\n";
    global_constants_ << "  %type_int = zext i8 %type to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_type, i32 %type_int)\n";
    global_constants_ << "  %result = icmp eq i8 %type, 0\n";
    // ir_code_ << "  ; Debug: Print result\n";
    // // ir_code_ << "  %debug_result = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    global_constants_ << "  %result_int = zext i1 %result to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result, i32 %result_int)\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @isString(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    // ir_code_ << "  ; Debug: Print function call\n";
    // // ir_code_ << "  %debug_msg = getelementptr [25 x i8], [25 x i8]* @.str_debug_is_string, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_msg)\n";
    global_constants_ << "  ; Check for null pointer\n";
    global_constants_ << "  %is_null = icmp eq %struct.BoxedValue* %box, null\n";
    global_constants_ << "  br i1 %is_null, label %return_false, label %check_type\n";
    global_constants_ << "return_false:\n";
    // ir_code_ << "  ; Debug: Print result for null case\n";
    // // ir_code_ << "  %debug_result_null = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result_null, i32 0)\n";
    global_constants_ << "  ret i1 false\n";
    global_constants_ << "check_type:\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0\n";
    global_constants_ << "  %type = load i8, i8* %type_ptr\n";
    // ir_code_ << "  ; Debug: Print type value\n";
    // // ir_code_ << "  %debug_type = getelementptr [20 x i8], [20 x i8]* @.str_debug_type_value, i32 0, i32 0\n";
    global_constants_ << "  %type_int = zext i8 %type to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_type, i32 %type_int)\n";
    global_constants_ << "  %result = icmp eq i8 %type, 1\n";
    // ir_code_ << "  ; Debug: Print result\n";
    // // ir_code_ << "  %debug_result = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    global_constants_ << "  %result_int = zext i1 %result to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result, i32 %result_int)\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @isBoolean(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    // ir_code_ << "  ; Debug: Print function call\n";
    // // ir_code_ << "  %debug_msg = getelementptr [26 x i8], [26 x i8]* @.str_debug_is_boolean, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_msg)\n";
    global_constants_ << "  ; Check for null pointer\n";
    global_constants_ << "  %is_null = icmp eq %struct.BoxedValue* %box, null\n";
    global_constants_ << "  br i1 %is_null, label %return_false, label %check_type\n";
    global_constants_ << "return_false:\n";
    // ir_code_ << "  ; Debug: Print result for null case\n";
    // // ir_code_ << "  %debug_result_null = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result_null, i32 0)\n";
    global_constants_ << "  ret i1 false\n";
    global_constants_ << "check_type:\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0\n";
    global_constants_ << "  %type = load i8, i8* %type_ptr\n";
    // ir_code_ << "  ; Debug: Print type value\n";
    // // ir_code_ << "  %debug_type = getelementptr [20 x i8], [20 x i8]* @.str_debug_type_value, i32 0, i32 0\n";
    global_constants_ << "  %type_int = zext i8 %type to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_type, i32 %type_int)\n";
    global_constants_ << "  %result = icmp eq i8 %type, 2\n";
    // ir_code_ << "  ; Debug: Print result\n";
    // // ir_code_ << "  %debug_result = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    global_constants_ << "  %result_int = zext i1 %result to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result, i32 %result_int)\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @isObject(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    // ir_code_ << "  ; Debug: Print function call\n";
    // // ir_code_ << "  %debug_msg = getelementptr [25 x i8], [25 x i8]* @.str_debug_is_object, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_msg)\n";
    global_constants_ << "  ; Check for null pointer\n";
    global_constants_ << "  %is_null = icmp eq %struct.BoxedValue* %box, null\n";
    global_constants_ << "  br i1 %is_null, label %return_false, label %check_type\n";
    global_constants_ << "return_false:\n";
    // ir_code_ << "  ; Debug: Print result for null case\n";
    // // ir_code_ << "  %debug_result_null = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result_null, i32 0)\n";
    global_constants_ << "  ret i1 false\n";
    global_constants_ << "check_type:\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0\n";
    global_constants_ << "  %type = load i8, i8* %type_ptr\n";
    // ir_code_ << "  ; Debug: Print type value\n";
    // // ir_code_ << "  %debug_type = getelementptr [20 x i8], [20 x i8]* @.str_debug_type_value, i32 0, i32 0\n";
    global_constants_ << "  %type_int = zext i8 %type to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_type, i32 %type_int)\n";
    global_constants_ << "  %result = icmp eq i8 %type, 3\n";
    // ir_code_ << "  ; Debug: Print result\n";
    // // ir_code_ << "  %debug_result = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    global_constants_ << "  %result_int = zext i1 %result to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result, i32 %result_int)\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @isNull(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    // ir_code_ << "  ; Debug: Print function call\n";
    // // ir_code_ << "  %debug_msg = getelementptr [23 x i8], [23 x i8]* @.str_debug_is_null, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_msg)\n";
    global_constants_ << "  ; Check for null pointer\n";
    global_constants_ << "  %is_null = icmp eq %struct.BoxedValue* %box, null\n";
    global_constants_ << "  br i1 %is_null, label %return_true, label %check_type\n";
    global_constants_ << "return_true:\n";
    // ir_code_ << "  ; Debug: Print result for null pointer case\n";
    // // ir_code_ << "  %debug_result_null_ptr = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result_null_ptr, i32 1)\n";
    global_constants_ << "  ret i1 true\n";
    global_constants_ << "check_type:\n";
    global_constants_ << "  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0\n";
    global_constants_ << "  %type = load i8, i8* %type_ptr\n";
    // ir_code_ << "  ; Debug: Print type value\n";
    // // ir_code_ << "  %debug_type = getelementptr [20 x i8], [20 x i8]* @.str_debug_type_value, i32 0, i32 0\n";
    global_constants_ << "  %type_int = zext i8 %type to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_type, i32 %type_int)\n";
    global_constants_ << "  %result = icmp eq i8 %type, 4\n";
    // ir_code_ << "  ; Debug: Print result\n";
    // // ir_code_ << "  %debug_result = getelementptr [18 x i8], [18 x i8]* @.str_debug_result, i32 0, i32 0\n";
    global_constants_ << "  %result_int = zext i1 %result to i32\n";
    // ir_code_ << "  call i32 (i8*, ...) @printf(i8* %debug_result, i32 %result_int)\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    // Simple arithmetic operations (simplified for now)
    global_constants_ << "define double @boxedAdd(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fadd double %left_num, %right_num\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define double @boxedSubtract(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fsub double %left_num, %right_num\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define double @boxedMultiply(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fmul double %left_num, %right_num\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define double @boxedDivide(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fdiv double %left_num, %right_num\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define double @boxedModulo(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = frem double %left_num, %right_num\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define double @boxedPower(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = call double @pow(double %left_num, double %right_num)\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    // Comparison operations
    global_constants_ << "define i1 @boxedEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fcmp ueq double %left_num, %right_num\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedNotEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fcmp une double %left_num, %right_num\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedLessThan(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fcmp ult double %left_num, %right_num\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedGreaterThan(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fcmp ugt double %left_num, %right_num\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedLessEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fcmp ule double %left_num, %right_num\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedGreaterEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = fcmp uge double %left_num, %right_num\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    // Logical operations
    global_constants_ << "define i1 @boxedLogicalAnd(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_bool = call i1 @unboxBoolean(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_bool = call i1 @unboxBoolean(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = and i1 %left_bool, %right_bool\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedLogicalOr(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %left_bool = call i1 @unboxBoolean(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %right_bool = call i1 @unboxBoolean(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %result = or i1 %left_bool, %right_bool\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i1 @boxedLogicalNot(%struct.BoxedValue* %operand) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %operand)\n";
    global_constants_ << "  %result = xor i1 %bool_val, true\n";
    global_constants_ << "  ret i1 %result\n";
    global_constants_ << "}\n\n";

    // String operations
    global_constants_ << "define i8* @boxedConcatenate(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  ; Check for null operands\n";
    global_constants_ << "  %left_null = icmp eq %struct.BoxedValue* %left, null\n";
    global_constants_ << "  %right_null = icmp eq %struct.BoxedValue* %right, null\n";
    global_constants_ << "  %any_null = or i1 %left_null, %right_null\n";
    global_constants_ << "  br i1 %any_null, label %handle_null, label %process_operands\n";
    global_constants_ << "handle_null:\n";
    global_constants_ << "  ; Return null if any operand is null\n";
    global_constants_ << "  ret i8* null\n";
    global_constants_ << "process_operands:\n";
    global_constants_ << "  ; Check left operand type and convert to string if needed\n";
    global_constants_ << "  %left_is_string = call i1 @isString(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br i1 %left_is_string, label %left_is_str, label %left_check_number\n";
    global_constants_ << "left_is_str:\n";
    global_constants_ << "  %left_str_1 = call i8* @unboxString(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_check_number:\n";
    global_constants_ << "  %left_is_number = call i1 @isNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br i1 %left_is_number, label %left_convert_number, label %left_check_bool\n";
    global_constants_ << "left_convert_number:\n";
    global_constants_ << "  %left_num_val = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %left_str_2 = call i8* @double_to_string(double %left_num_val)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_check_bool:\n";
    global_constants_ << "  %left_is_bool = call i1 @isBoolean(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br i1 %left_is_bool, label %left_convert_bool, label %left_convert_object\n";
    global_constants_ << "left_convert_bool:\n";
    global_constants_ << "  %left_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %left_str_3 = call i8* @bool_to_string(i1 %left_bool_val)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_convert_object:\n";
    global_constants_ << "  %left_obj_val = call i8* @unboxObject(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %left_str_4 = call i8* @object_to_string(i8* %left_obj_val)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_done:\n";
    global_constants_ << "  %left_str = phi i8* [ %left_str_1, %left_is_str ], [ %left_str_2, %left_convert_number ], [ %left_str_3, %left_convert_bool ], [ %left_str_4, %left_convert_object ]\n";
    // ir_code_ << "  call double @print_string(i8* %left_str)\n"; // [DEBUGGING]
    global_constants_ << "  ; Check right operand type and convert to string if needed\n";
    global_constants_ << "  %right_is_string = call i1 @isString(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br i1 %right_is_string, label %right_is_str, label %right_check_number\n";
    global_constants_ << "right_is_str:\n";
    global_constants_ << "  %right_str_1 = call i8* @unboxString(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_check_number:\n";
    global_constants_ << "  %right_is_number = call i1 @isNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br i1 %right_is_number, label %right_convert_number, label %right_check_bool\n";
    global_constants_ << "right_convert_number:\n";
    global_constants_ << "  %right_num_val = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %right_str_2 = call i8* @double_to_string(double %right_num_val)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_check_bool:\n";
    global_constants_ << "  %right_is_bool = call i1 @isBoolean(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br i1 %right_is_bool, label %right_convert_bool, label %right_convert_object\n";
    global_constants_ << "right_convert_bool:\n";
    global_constants_ << "  %right_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %right_str_3 = call i8* @bool_to_string(i1 %right_bool_val)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_convert_object:\n";
    global_constants_ << "  %right_obj_val = call i8* @unboxObject(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %right_str_4 = call i8* @object_to_string(i8* %right_obj_val)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_done:\n";
    global_constants_ << "  %right_str = phi i8* [ %right_str_1, %right_is_str ], [ %right_str_2, %right_convert_number ], [ %right_str_3, %right_convert_bool ], [ %right_str_4, %right_convert_object ]\n";
    // ir_code_ << "  call double @print_string(i8* %right_str)\n"; // [DEBUGGING]
    global_constants_ << "  ; Check for null strings after conversion\n";
    global_constants_ << "  %left_str_null = icmp eq i8* %left_str, null\n";
    global_constants_ << "  %right_str_null = icmp eq i8* %right_str, null\n";
    global_constants_ << "  %any_str_null = or i1 %left_str_null, %right_str_null\n";
    global_constants_ << "  br i1 %any_str_null, label %handle_str_null, label %concatenate\n";
    global_constants_ << "handle_str_null:\n";
    global_constants_ << "  ; If any string is null after conversion, return null\n";
    global_constants_ << "  ret i8* null\n";
    global_constants_ << "concatenate:\n";
    global_constants_ << "  ; Concatenate both strings\n";
    global_constants_ << "  %result = call i8* @concat_strings(i8* %left_str, i8* %right_str)\n";
    // ir_code_ << "  call double @print_string(i8* %result)\n"; // [DEBUGGING]
    global_constants_ << "  ; Check if concatenation failed\n";
    global_constants_ << "  %concat_failed = icmp eq i8* %result, null\n";
    global_constants_ << "  br i1 %concat_failed, label %handle_concat_failure, label %return_result\n";
    global_constants_ << "handle_concat_failure:\n";
    global_constants_ << "  ; Return null if concatenation failed\n";
    global_constants_ << "  ret i8* null\n";
    global_constants_ << "return_result:\n";
    global_constants_ << "  ; Return the concatenated string directly\n";
    global_constants_ << "  ret i8* %result\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define i8* @boxedConcatenateWithSpace(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  ; Check for null operands\n";
    global_constants_ << "  %left_null = icmp eq %struct.BoxedValue* %left, null\n";
    global_constants_ << "  %right_null = icmp eq %struct.BoxedValue* %right, null\n";
    global_constants_ << "  %any_null = or i1 %left_null, %right_null\n";
    global_constants_ << "  br i1 %any_null, label %handle_null, label %process_operands\n";
    global_constants_ << "handle_null:\n";
    global_constants_ << "  ; Return null if any operand is null\n";
    global_constants_ << "  ret i8* null\n";
    global_constants_ << "process_operands:\n";
    global_constants_ << "  ; Check left operand type and convert to string if needed\n";
    global_constants_ << "  %left_is_string = call i1 @isString(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br i1 %left_is_string, label %left_is_str, label %left_check_number\n";
    global_constants_ << "left_is_str:\n";
    global_constants_ << "  %left_str_1 = call i8* @unboxString(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_check_number:\n";
    global_constants_ << "  %left_is_number = call i1 @isNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br i1 %left_is_number, label %left_convert_number, label %left_check_bool\n";
    global_constants_ << "left_convert_number:\n";
    global_constants_ << "  %left_num_val = call double @unboxNumber(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %left_str_2 = call i8* @double_to_string(double %left_num_val)\n";
    global_constants_ << "  call double @print_string(i8* %left_str_2)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_check_bool:\n";
    global_constants_ << "  %left_is_bool = call i1 @isBoolean(%struct.BoxedValue* %left)\n";
    global_constants_ << "  br i1 %left_is_bool, label %left_convert_bool, label %left_convert_object\n";
    global_constants_ << "left_convert_bool:\n";
    global_constants_ << "  %left_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %left_str_3 = call i8* @bool_to_string(i1 %left_bool_val)\n";
    global_constants_ << "  call double @print_string(i8* %left_str_3)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_convert_object:\n";
    global_constants_ << "  %left_obj_val = call i8* @unboxObject(%struct.BoxedValue* %left)\n";
    global_constants_ << "  %left_str_4 = call i8* @object_to_string(i8* %left_obj_val)\n";
    global_constants_ << "  br label %left_done\n";
    global_constants_ << "left_done:\n";
    global_constants_ << "  %left_str = phi i8* [ %left_str_1, %left_is_str ], [ %left_str_2, %left_convert_number ], [ %left_str_3, %left_convert_bool ], [ %left_str_4, %left_convert_object ]\n";
    global_constants_ << "  ; Check right operand type and convert to string if needed\n";
    global_constants_ << "  %right_is_string = call i1 @isString(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br i1 %right_is_string, label %right_is_str, label %right_check_number\n";
    global_constants_ << "right_is_str:\n";
    global_constants_ << "  %right_str_1 = call i8* @unboxString(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_check_number:\n";
    global_constants_ << "  %right_is_number = call i1 @isNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br i1 %right_is_number, label %right_convert_number, label %right_check_bool\n";
    global_constants_ << "right_convert_number:\n";
    global_constants_ << "  %right_num_val = call double @unboxNumber(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %right_str_2 = call i8* @double_to_string(double %right_num_val)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_check_bool:\n";
    global_constants_ << "  %right_is_bool = call i1 @isBoolean(%struct.BoxedValue* %right)\n";
    global_constants_ << "  br i1 %right_is_bool, label %right_convert_bool, label %right_convert_object\n";
    global_constants_ << "right_convert_bool:\n";
    global_constants_ << "  %right_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %right_str_3 = call i8* @bool_to_string(i1 %right_bool_val)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_convert_object:\n";
    global_constants_ << "  %right_obj_val = call i8* @unboxObject(%struct.BoxedValue* %right)\n";
    global_constants_ << "  %right_str_4 = call i8* @object_to_string(i8* %right_obj_val)\n";
    global_constants_ << "  br label %right_done\n";
    global_constants_ << "right_done:\n";
    global_constants_ << "  %right_str = phi i8* [ %right_str_1, %right_is_str ], [ %right_str_2, %right_convert_number ], [ %right_str_3, %right_convert_bool ], [ %right_str_4, %right_convert_object ]\n";
    global_constants_ << "  ; Check for null strings after conversion\n";
    global_constants_ << "  %left_str_null = icmp eq i8* %left_str, null\n";
    global_constants_ << "  %right_str_null = icmp eq i8* %right_str, null\n";
    global_constants_ << "  %any_str_null = or i1 %left_str_null, %right_str_null\n";
    global_constants_ << "  br i1 %any_str_null, label %handle_str_null, label %concatenate\n";
    global_constants_ << "handle_str_null:\n";
    global_constants_ << "  ; If any string is null after conversion, return null\n";
    global_constants_ << "  ret i8* null\n";
    global_constants_ << "concatenate:\n";
    global_constants_ << "  ; Concatenate both strings with space\n";
    global_constants_ << "  %result = call i8* @concat_strings_ws(i8* %left_str, i8* %right_str)\n";
    global_constants_ << "  ; Check if concatenation failed\n";
    global_constants_ << "  %concat_failed = icmp eq i8* %result, null\n";
    global_constants_ << "  br i1 %concat_failed, label %handle_concat_failure, label %return_result\n";
    global_constants_ << "handle_concat_failure:\n";
    global_constants_ << "  ; Return null if concatenation failed\n";
    global_constants_ << "  ret i8* null\n";
    global_constants_ << "return_result:\n";
    global_constants_ << "  ; Return the concatenated string directly\n";
    global_constants_ << "  ret i8* %result\n";
    global_constants_ << "}\n\n";

    // Memory management
    global_constants_ << "define void @freeBoxedValue(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %box_ptr = bitcast %struct.BoxedValue* %box to i8*\n";
    global_constants_ << "  call void @free(i8* %box_ptr)\n";
    global_constants_ << "  ret void\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @copyBoxedValue(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  ret %struct.BoxedValue* %box\n";
    global_constants_ << "}\n\n";

    // Type conversion
    global_constants_ << "define %struct.BoxedValue* @convertToString(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  ; Check if it's already a string\n";
    global_constants_ << "  %is_string = call i1 @isString(%struct.BoxedValue* %box)\n";
    global_constants_ << "  br i1 %is_string, label %return_original, label %check_number\n";
    global_constants_ << "return_original:\n";
    global_constants_ << "  ret %struct.BoxedValue* %box\n";
    global_constants_ << "check_number:\n";
    global_constants_ << "  %is_number = call i1 @isNumber(%struct.BoxedValue* %box)\n";
    global_constants_ << "  br i1 %is_number, label %convert_number, label %check_boolean\n";
    global_constants_ << "convert_number:\n";
    global_constants_ << "  %num = call double @unboxNumber(%struct.BoxedValue* %box)\n";
    global_constants_ << "  %str = call i8* @double_to_string(double %num)\n";
    global_constants_ << "  %boxed = call %struct.BoxedValue* @boxString(i8* %str)\n";
    global_constants_ << "  ret %struct.BoxedValue* %boxed\n";
    global_constants_ << "check_boolean:\n";
    global_constants_ << "  %is_boolean = call i1 @isBoolean(%struct.BoxedValue* %box)\n";
    global_constants_ << "  br i1 %is_boolean, label %convert_boolean, label %return_error\n";
    global_constants_ << "convert_boolean:\n";
    global_constants_ << "  %bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %box)\n";
    global_constants_ << "  %bool_str = call i8* @bool_to_string(i1 %bool_val)\n";
    global_constants_ << "  %bool_boxed = call %struct.BoxedValue* @boxString(i8* %bool_str)\n";
    global_constants_ << "  ret %struct.BoxedValue* %bool_boxed\n";
    global_constants_ << "return_error:\n";
    global_constants_ << "  ; Return error string for unsupported types\n";
    global_constants_ << "  %error_str = call i8* @malloc(i32 8)\n";
    global_constants_ << "  %error_src = getelementptr [8 x i8], [8 x i8]* @.str_error, i32 0, i32 0\n";
    global_constants_ << "  %format = getelementptr [3 x i8], [3 x i8]* @.str_copy_format, i32 0, i32 0\n";
    global_constants_ << "  %error_result = call i32 (i8*, i8*, ...) @sprintf(i8* %error_str, i8* %format, i8* %error_src)\n";
    global_constants_ << "  %error_boxed = call %struct.BoxedValue* @boxString(i8* %error_str)\n";
    global_constants_ << "  ret %struct.BoxedValue* %error_boxed\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @convertToNumber(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %is_string = call i1 @isString(%struct.BoxedValue* %box)\n";
    global_constants_ << "  br i1 %is_string, label %convert_string, label %return_original\n";
    global_constants_ << "convert_string:\n";
    global_constants_ << "  %str = call i8* @unboxString(%struct.BoxedValue* %box)\n";
    global_constants_ << "  %num = call double @strtod(i8* %str, i8** null)\n";
    global_constants_ << "  %boxed = call %struct.BoxedValue* @boxNumber(double %num)\n";
    global_constants_ << "  ret %struct.BoxedValue* %boxed\n";
    global_constants_ << "return_original:\n";
    global_constants_ << "  ret %struct.BoxedValue* %box\n";
    global_constants_ << "}\n\n";

    global_constants_ << "define %struct.BoxedValue* @convertToBoolean(%struct.BoxedValue* %box) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %is_number = call i1 @isNumber(%struct.BoxedValue* %box)\n";
    global_constants_ << "  br i1 %is_number, label %convert_number, label %return_original\n";
    global_constants_ << "convert_number:\n";
    global_constants_ << "  %num = call double @unboxNumber(%struct.BoxedValue* %box)\n";
    global_constants_ << "  %bool = fcmp one double %num, 0.0\n";
    global_constants_ << "  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %bool)\n";
    global_constants_ << "  ret %struct.BoxedValue* %boxed\n";
    global_constants_ << "return_original:\n";
    global_constants_ << "  ret %struct.BoxedValue* %box\n";
    global_constants_ << "}\n\n";

    // Object constructor - base of inheritance chain
    global_constants_ << "define %struct.Object* @construct_Object() {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %obj = call i8* @malloc(i32 8)\n";
    global_constants_ << "  %obj_ptr = bitcast i8* %obj to %struct.Object*\n";
    global_constants_ << "  %dummy_field = getelementptr %struct.Object, %struct.Object* %obj_ptr, i32 0, i32 0\n";
    global_constants_ << "  store i8 0, i8* %dummy_field\n";
    global_constants_ << "  ret %struct.Object* %obj_ptr\n";
    global_constants_ << "}\n\n";

    // Add print functions for different types
    global_constants_ << "; Print functions for different types\n";

    // Print double
    global_constants_ << "define double @print_double(double %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %format = getelementptr [4 x i8], [4 x i8]* @.str_double, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, ...) @printf(i8* %format, double %value)\n";
    global_constants_ << "  ret double %value\n";
    global_constants_ << "}\n\n";

    // Print boolean
    global_constants_ << "define double @print_bool(i1 %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %format_true = getelementptr [6 x i8], [6 x i8]* @.str_bool_true, i32 0, i32 0\n";
    global_constants_ << "  %format_false = getelementptr [7 x i8], [7 x i8]* @.str_bool_false, i32 0, i32 0\n";
    global_constants_ << "  %format = select i1 %value, i8* %format_true, i8* %format_false\n";
    global_constants_ << "  %result = call i32 (i8*, ...) @printf(i8* %format)\n";
    global_constants_ << "  %value_double = uitofp i1 %value to double\n";
    global_constants_ << "  ret double %value_double\n";
    global_constants_ << "}\n\n";

    // Print string
    global_constants_ << "define double @print_string(i8* %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %format = getelementptr [4 x i8], [4 x i8]* @.str_string, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, ...) @printf(i8* %format, i8* %value)\n";
    global_constants_ << "  ret double 0.0\n";
    global_constants_ << "}\n\n";

    // Print boxed value
    global_constants_ << "define double @print_boxed(%struct.BoxedValue* %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %is_number = call i1 @isNumber(%struct.BoxedValue* %value)\n";
    global_constants_ << "  br i1 %is_number, label %print_number, label %check_string\n";
    global_constants_ << "print_number:\n";
    global_constants_ << "  %num_value = call double @unboxNumber(%struct.BoxedValue* %value)\n";
    global_constants_ << "  %result1 = call double @print_double(double %num_value)\n";
    global_constants_ << "  ret double %result1\n";
    global_constants_ << "check_string:\n";
    global_constants_ << "  %is_string = call i1 @isString(%struct.BoxedValue* %value)\n";
    global_constants_ << "  br i1 %is_string, label %print_string, label %check_boolean\n";
    global_constants_ << "print_string:\n";
    global_constants_ << "  %str_value = call i8* @unboxString(%struct.BoxedValue* %value)\n";
    global_constants_ << "  %result2 = call double @print_string(i8* %str_value)\n";
    global_constants_ << "  ret double %result2\n";
    global_constants_ << "check_boolean:\n";
    global_constants_ << "  %is_boolean = call i1 @isBoolean(%struct.BoxedValue* %value)\n";
    global_constants_ << "  br i1 %is_boolean, label %print_boolean, label %print_object\n";
    global_constants_ << "print_boolean:\n";
    global_constants_ << "  %bool_value = call i1 @unboxBoolean(%struct.BoxedValue* %value)\n";
    global_constants_ << "  %result3 = call double @print_bool(i1 %bool_value)\n";
    global_constants_ << "  ret double %result3\n";
    global_constants_ << "print_object:\n";
    global_constants_ << "  %obj_value = call i8* @unboxObject(%struct.BoxedValue* %value)\n";
    global_constants_ << "  %result4 = call double @print_string(i8* %obj_value)\n";
    global_constants_ << "  ret double %result4\n";
    global_constants_ << "}\n\n";

    // Generic print function that dispatches based on type
    global_constants_ << "define double @print(double %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %result = call double @print_double(double %value)\n";
    global_constants_ << "  ret double %result\n";
    global_constants_ << "}\n\n";

    // String conversion functions
    global_constants_ << "; String conversion functions\n";

    // Convert double to string
    global_constants_ << "define i8* @double_to_string(double %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %buffer = call i8* @malloc(i32 64)\n";
    global_constants_ << "  %format = getelementptr [4 x i8], [4 x i8]* @.str_double_format, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, double %value)\n";
    // ir_code_ << "  call double @print_string(i8* %buffer)\n"; // [DEBUGGING]
    global_constants_ << "  ret i8* %buffer\n";
    global_constants_ << "}\n\n";

    // Convert boolean to string
    global_constants_ << "define i8* @bool_to_string(i1 %value) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %buffer = call i8* @malloc(i32 8)\n";
    global_constants_ << "  %true_str = getelementptr [5 x i8], [5 x i8]* @.str_true, i32 0, i32 0\n";
    global_constants_ << "  %false_str = getelementptr [6 x i8], [6 x i8]* @.str_false, i32 0, i32 0\n";
    global_constants_ << "  %str_to_copy = select i1 %value, i8* %true_str, i8* %false_str\n";
    global_constants_ << "  %format = getelementptr [3 x i8], [3 x i8]* @.str_copy_format, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str_to_copy)\n";
    // ir_code_ << "  call double @print_string(i8* %buffer)\n"; // [DEBUGGING]
    global_constants_ << "  ret i8* %buffer\n";
    global_constants_ << "}\n\n";

    // Convert object to string (simplified - just return a generic object string)
    global_constants_ << "define i8* @object_to_string(i8* %obj) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %buffer = call i8* @malloc(i32 32)\n";
    global_constants_ << "  %format = getelementptr [12 x i8], [12 x i8]* @.str_object_format, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %obj)\n";
    global_constants_ << "  ret i8* %buffer\n";
    global_constants_ << "}\n\n";

    // String concatenation function
    global_constants_ << "define i8* @concat_strings(i8* %str1, i8* %str2) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %buffer = call i8* @malloc(i32 256)\n";
    global_constants_ << "  %format = getelementptr [5 x i8], [5 x i8]* @.str_concat_format, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)\n";
    global_constants_ << "  ret i8* %buffer\n";
    global_constants_ << "}\n\n";

    // String concatenation with space function
    global_constants_ << "define i8* @concat_strings_ws(i8* %str1, i8* %str2) {\n";
    global_constants_ << "entry:\n";
    global_constants_ << "  %buffer = call i8* @malloc(i32 256)\n";
    global_constants_ << "  %format = getelementptr [6 x i8], [6 x i8]* @.str_concat_ws_format, i32 0, i32 0\n";
    global_constants_ << "  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)\n";
    global_constants_ << "  ret i8* %buffer\n";
    global_constants_ << "}\n\n";

    // Add string constants for formatting
    global_constants_ << "@.str_double = private unnamed_addr constant [4 x i8] c\"%f\\0A\\00\"\n";
    global_constants_ << "@.str_bool_true = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n";
    global_constants_ << "@.str_bool_false = private unnamed_addr constant [7 x i8] c\"false\\0A\\00\"\n";
    global_constants_ << "@.str_string = private unnamed_addr constant [4 x i8] c\"%s\\0A\\00\"\n";
    global_constants_ << "@.str_double_format = private unnamed_addr constant [3 x i8] c\"%f\\00\"\n";
    global_constants_ << "@.str_true = private unnamed_addr constant [5 x i8] c\"true\\00\"\n";
    global_constants_ << "@.str_false = private unnamed_addr constant [6 x i8] c\"false\\00\"\n";
    global_constants_ << "@.str_copy_format = private unnamed_addr constant [3 x i8] c\"%s\\00\"\n";
    global_constants_ << "@.str_concat_format = private unnamed_addr constant [5 x i8] c\"%s%s\\00\"\n";
    global_constants_ << "@.str_concat_ws_format = private unnamed_addr constant [6 x i8] c\"%s %s\\00\"\n";
    global_constants_ << "@.str_null = private unnamed_addr constant [5 x i8] c\"null\\00\"\n";
    global_constants_ << "@.str_object_format = private unnamed_addr constant [12 x i8] c\"Object: %s\\0A\\00\"\n";
    global_constants_ << "@.str_error = private unnamed_addr constant [8 x i8] c\"[ERROR]\\00\"\n";

    // // Debug message constants for type checking functions
    // // ir_code_ << "@.str_debug_is_number = private unnamed_addr constant [25 x i8] c\"[DEBUG] isNumber called\\0A\\00\"\n";
    // // ir_code_ << "@.str_debug_is_string = private unnamed_addr constant [25 x i8] c\"[DEBUG] isString called\\0A\\00\"\n";
    // // ir_code_ << "@.str_debug_is_boolean = private unnamed_addr constant [26 x i8] c\"[DEBUG] isBoolean called\\0A\\00\"\n";
    // // ir_code_ << "@.str_debug_is_object = private unnamed_addr constant [25 x i8] c\"[DEBUG] isObject called\\0A\\00\"\n";
    // // ir_code_ << "@.str_debug_is_null = private unnamed_addr constant [23 x i8] c\"[DEBUG] isNull called\\0A\\00\"\n";
    // // ir_code_ << "@.str_debug_type_value = private unnamed_addr constant [24 x i8] c\"[DEBUG] Type value: %d\\0A\\00\"\n";
    // // ir_code_ << "@.str_debug_result = private unnamed_addr constant [20 x i8] c\"[DEBUG] Result: %d\\0A\\00\"\n";
    global_constants_ << "\n";

    // Register Object type in the inheritance system
    // Note: This will be done by the codegen when it processes type declarations
    // Object is the implicit base type for all types that don't specify inheritance

    builtins_registered_ = true;
}