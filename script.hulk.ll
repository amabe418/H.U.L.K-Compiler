@.str.3658472883277130625 = private unnamed_addr constant [6 x i8] c"false\00"
; Built-in function declarations
declare double @sqrt(double)
declare double @sin(double)
declare double @cos(double)
declare double @tan(double)
declare double @exp(double)
declare double @log(double)
declare i32 @rand()
declare i32 @printf(i8*, ...)
declare i32 @sprintf(i8*, i8*, ...)
declare i8* @malloc(i32)
declare void @free(i8*)
declare double @strtod(i8*, i8**)

%struct.BoxedValue = type { i8, [7 x i8], i64 }

; Boxed value runtime functions
define %struct.BoxedValue* @boxNumber(double %value) {
entry:
  %box = call i8* @malloc(i32 16)
  %box_ptr = bitcast i8* %box to %struct.BoxedValue*
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0
  store i8 0, i8* %type_ptr
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2
  %value_int = bitcast double %value to i64
  store i64 %value_int, i64* %value_ptr
  ret %struct.BoxedValue* %box_ptr
}

define %struct.BoxedValue* @boxString(i8* %value) {
entry:
  %box = call i8* @malloc(i32 16)
  %box_ptr = bitcast i8* %box to %struct.BoxedValue*
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0
  store i8 1, i8* %type_ptr
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2
  %value_int = ptrtoint i8* %value to i64
  store i64 %value_int, i64* %value_ptr
  ret %struct.BoxedValue* %box_ptr
}

define %struct.BoxedValue* @boxBoolean(i1 %value) {
entry:
  %box = call i8* @malloc(i32 16)
  %box_ptr = bitcast i8* %box to %struct.BoxedValue*
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0
  store i8 2, i8* %type_ptr
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2
  %value_int = zext i1 %value to i64
  store i64 %value_int, i64* %value_ptr
  ret %struct.BoxedValue* %box_ptr
}

define %struct.BoxedValue* @boxObject(i8* %value) {
entry:
  %box = call i8* @malloc(i32 16)
  %box_ptr = bitcast i8* %box to %struct.BoxedValue*
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0
  store i8 3, i8* %type_ptr
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2
  %value_int = ptrtoint i8* %value to i64
  store i64 %value_int, i64* %value_ptr
  ret %struct.BoxedValue* %box_ptr
}

define %struct.BoxedValue* @boxNull() {
entry:
  %box = call i8* @malloc(i32 16)
  %box_ptr = bitcast i8* %box to %struct.BoxedValue*
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 0
  store i8 4, i8* %type_ptr
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box_ptr, i32 0, i32 2
  store i64 0, i64* %value_ptr
  ret %struct.BoxedValue* %box_ptr
}

define double @unboxNumber(%struct.BoxedValue* %box) {
entry:
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2
  %value_int = load i64, i64* %value_ptr
  %value = bitcast i64 %value_int to double
  ret double %value
}

define i8* @unboxString(%struct.BoxedValue* %box) {
entry:
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2
  %value_int = load i64, i64* %value_ptr
  %value = inttoptr i64 %value_int to i8*
  ret i8* %value
}

define i1 @unboxBoolean(%struct.BoxedValue* %box) {
entry:
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2
  %value_int = load i64, i64* %value_ptr
  %value = trunc i64 %value_int to i1
  ret i1 %value
}

define i8* @unboxObject(%struct.BoxedValue* %box) {
entry:
  %value_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 2
  %value_int = load i64, i64* %value_ptr
  %value = inttoptr i64 %value_int to i8*
  ret i8* %value
}

define i1 @isNumber(%struct.BoxedValue* %box) {
entry:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %result = icmp eq i8 %type, 0
  ret i1 %result
}

define i1 @isString(%struct.BoxedValue* %box) {
entry:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %result = icmp eq i8 %type, 1
  ret i1 %result
}

define i1 @isBoolean(%struct.BoxedValue* %box) {
entry:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %result = icmp eq i8 %type, 2
  ret i1 %result
}

define i1 @isObject(%struct.BoxedValue* %box) {
entry:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %result = icmp eq i8 %type, 3
  ret i1 %result
}

define i1 @isNull(%struct.BoxedValue* %box) {
entry:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %result = icmp eq i8 %type, 4
  ret i1 %result
}

define %struct.BoxedValue* @boxedAdd(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fadd double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxNumber(double %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedSubtract(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fsub double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxNumber(double %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedMultiply(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fmul double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxNumber(double %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedDivide(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fdiv double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxNumber(double %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ueq double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedNotEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp une double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedLessThan(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ult double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedGreaterThan(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ugt double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedLessEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ule double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedGreaterEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp uge double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedLogicalAnd(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_bool = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %right_bool = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %result = and i1 %left_bool, %right_bool
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedLogicalOr(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_bool = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %right_bool = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %result = or i1 %left_bool, %right_bool
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedLogicalNot(%struct.BoxedValue* %operand) {
entry:
  %bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %operand)
  %result = xor i1 %bool_val, true
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  ; Check left operand type and convert to string if needed
  %left_is_string = call i1 @isString(%struct.BoxedValue* %left)
  br i1 %left_is_string, label %left_is_str, label %left_check_bool
left_is_str:
  %left_str_1 = call i8* @unboxString(%struct.BoxedValue* %left)
  br label %left_done
left_check_bool:
  %left_is_bool = call i1 @isBoolean(%struct.BoxedValue* %left)
  br i1 %left_is_bool, label %left_convert_bool, label %left_convert_number
left_convert_bool:
  %left_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %left_str_2 = call i8* @bool_to_string(i1 %left_bool_val)
  br label %left_done
left_convert_number:
  %left_num_val = call double @unboxNumber(%struct.BoxedValue* %left)
  %left_str_3 = call i8* @double_to_string(double %left_num_val)
  br label %left_done
left_done:
  %left_str = phi i8* [ %left_str_1, %left_is_str ], [ %left_str_2, %left_convert_bool ], [ %left_str_3, %left_convert_number ]
  ; Check right operand type and convert to string if needed
  %right_is_string = call i1 @isString(%struct.BoxedValue* %right)
  br i1 %right_is_string, label %right_is_str, label %right_check_bool
right_is_str:
  %right_str_1 = call i8* @unboxString(%struct.BoxedValue* %right)
  br label %right_done
right_check_bool:
  %right_is_bool = call i1 @isBoolean(%struct.BoxedValue* %right)
  br i1 %right_is_bool, label %right_convert_bool, label %right_convert_number
right_convert_bool:
  %right_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %right_str_2 = call i8* @bool_to_string(i1 %right_bool_val)
  br label %right_done
right_convert_number:
  %right_num_val = call double @unboxNumber(%struct.BoxedValue* %right)
  %right_str_3 = call i8* @double_to_string(double %right_num_val)
  br label %right_done
right_done:
  %right_str = phi i8* [ %right_str_1, %right_is_str ], [ %right_str_2, %right_convert_bool ], [ %right_str_3, %right_convert_number ]
  ; Concatenate both strings
  %result = call i8* @concat_strings(i8* %left_str, i8* %right_str)
  ; Box the result
  %boxed = call %struct.BoxedValue* @boxString(i8* %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedConcatenateWithSpace(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  ; Check left operand type and convert to string if needed
  %left_is_string = call i1 @isString(%struct.BoxedValue* %left)
  br i1 %left_is_string, label %left_is_str, label %left_check_bool
left_is_str:
  %left_str_1 = call i8* @unboxString(%struct.BoxedValue* %left)
  br label %left_done
left_check_bool:
  %left_is_bool = call i1 @isBoolean(%struct.BoxedValue* %left)
  br i1 %left_is_bool, label %left_convert_bool, label %left_convert_number
left_convert_bool:
  %left_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %left_str_2 = call i8* @bool_to_string(i1 %left_bool_val)
  br label %left_done
left_convert_number:
  %left_num_val = call double @unboxNumber(%struct.BoxedValue* %left)
  %left_str_3 = call i8* @double_to_string(double %left_num_val)
  br label %left_done
left_done:
  %left_str = phi i8* [ %left_str_1, %left_is_str ], [ %left_str_2, %left_convert_bool ], [ %left_str_3, %left_convert_number ]
  ; Check right operand type and convert to string if needed
  %right_is_string = call i1 @isString(%struct.BoxedValue* %right)
  br i1 %right_is_string, label %right_is_str, label %right_check_bool
right_is_str:
  %right_str_1 = call i8* @unboxString(%struct.BoxedValue* %right)
  br label %right_done
right_check_bool:
  %right_is_bool = call i1 @isBoolean(%struct.BoxedValue* %right)
  br i1 %right_is_bool, label %right_convert_bool, label %right_convert_number
right_convert_bool:
  %right_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %right_str_2 = call i8* @bool_to_string(i1 %right_bool_val)
  br label %right_done
right_convert_number:
  %right_num_val = call double @unboxNumber(%struct.BoxedValue* %right)
  %right_str_3 = call i8* @double_to_string(double %right_num_val)
  br label %right_done
right_done:
  %right_str = phi i8* [ %right_str_1, %right_is_str ], [ %right_str_2, %right_convert_bool ], [ %right_str_3, %right_convert_number ]
  ; Concatenate both strings with space
  %result = call i8* @concat_strings_ws(i8* %left_str, i8* %right_str)
  ; Box the result
  %boxed = call %struct.BoxedValue* @boxString(i8* %result)
  ret %struct.BoxedValue* %boxed
}

define void @freeBoxedValue(%struct.BoxedValue* %box) {
entry:
  %box_ptr = bitcast %struct.BoxedValue* %box to i8*
  call void @free(i8* %box_ptr)
  ret void
}

define %struct.BoxedValue* @copyBoxedValue(%struct.BoxedValue* %box) {
entry:
  ret %struct.BoxedValue* %box
}

define %struct.BoxedValue* @convertToString(%struct.BoxedValue* %box) {
entry:
  ; Check if it's already a string
  %is_string = call i1 @isString(%struct.BoxedValue* %box)
  br i1 %is_string, label %return_original, label %check_number
return_original:
  ret %struct.BoxedValue* %box
check_number:
  %is_number = call i1 @isNumber(%struct.BoxedValue* %box)
  br i1 %is_number, label %convert_number, label %check_boolean
convert_number:
  %num = call double @unboxNumber(%struct.BoxedValue* %box)
  %str = call i8* @double_to_string(double %num)
  %boxed = call %struct.BoxedValue* @boxString(i8* %str)
  ret %struct.BoxedValue* %boxed
check_boolean:
  %is_boolean = call i1 @isBoolean(%struct.BoxedValue* %box)
  br i1 %is_boolean, label %convert_boolean, label %return_error
convert_boolean:
  %bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %box)
  %bool_str = call i8* @bool_to_string(i1 %bool_val)
  %bool_boxed = call %struct.BoxedValue* @boxString(i8* %bool_str)
  ret %struct.BoxedValue* %bool_boxed
return_error:
  ; Return error string for unsupported types
  %error_str = call i8* @malloc(i32 8)
  %error_format = getelementptr [8 x i8], [8 x i8]* @.str_error, i32 0, i32 0
  %error_result = call i32 (i8*, i8*, ...) @sprintf(i8* %error_str, i8* %error_format)
  %error_boxed = call %struct.BoxedValue* @boxString(i8* %error_str)
  ret %struct.BoxedValue* %error_boxed
}

define %struct.BoxedValue* @convertToNumber(%struct.BoxedValue* %box) {
entry:
  %is_string = call i1 @isString(%struct.BoxedValue* %box)
  br i1 %is_string, label %convert_string, label %return_original
convert_string:
  %str = call i8* @unboxString(%struct.BoxedValue* %box)
  %num = call double @strtod(i8* %str, i8** null)
  %boxed = call %struct.BoxedValue* @boxNumber(double %num)
  ret %struct.BoxedValue* %boxed
return_original:
  ret %struct.BoxedValue* %box
}

define %struct.BoxedValue* @convertToBoolean(%struct.BoxedValue* %box) {
entry:
  %is_number = call i1 @isNumber(%struct.BoxedValue* %box)
  br i1 %is_number, label %convert_number, label %return_original
convert_number:
  %num = call double @unboxNumber(%struct.BoxedValue* %box)
  %bool = fcmp one double %num, 0.0
  %boxed = call %struct.BoxedValue* @boxBoolean(i1 %bool)
  ret %struct.BoxedValue* %boxed
return_original:
  ret %struct.BoxedValue* %box
}

; Print functions for different types
define double @print_double(double %value) {
entry:
  %format = getelementptr [4 x i8], [4 x i8]* @.str_double, i32 0, i32 0
  %result = call i32 (i8*, ...) @printf(i8* %format, double %value)
  ret double %value
}

define double @print_bool(i1 %value) {
entry:
  %format_true = getelementptr [6 x i8], [6 x i8]* @.str_bool_true, i32 0, i32 0
  %format_false = getelementptr [7 x i8], [7 x i8]* @.str_bool_false, i32 0, i32 0
  %format = select i1 %value, i8* %format_true, i8* %format_false
  %result = call i32 (i8*, ...) @printf(i8* %format)
  %value_double = uitofp i1 %value to double
  ret double %value_double
}

define double @print_string(i8* %value) {
entry:
  %format = getelementptr [4 x i8], [4 x i8]* @.str_string, i32 0, i32 0
  %result = call i32 (i8*, ...) @printf(i8* %format, i8* %value)
  ret double 0.0
}

define double @print_boxed(%struct.BoxedValue* %value) {
entry:
  %is_number = call i1 @isNumber(%struct.BoxedValue* %value)
  br i1 %is_number, label %print_number, label %check_string
print_number:
  %num_value = call double @unboxNumber(%struct.BoxedValue* %value)
  %result1 = call double @print_double(double %num_value)
  ret double %result1
check_string:
  %is_string = call i1 @isString(%struct.BoxedValue* %value)
  br i1 %is_string, label %print_string, label %check_boolean
print_string:
  %str_value = call i8* @unboxString(%struct.BoxedValue* %value)
  %result2 = call double @print_string(i8* %str_value)
  ret double %result2
check_boolean:
  %is_boolean = call i1 @isBoolean(%struct.BoxedValue* %value)
  br i1 %is_boolean, label %print_boolean, label %print_object
print_boolean:
  %bool_value = call i1 @unboxBoolean(%struct.BoxedValue* %value)
  %result3 = call double @print_bool(i1 %bool_value)
  ret double %result3
print_object:
  %obj_value = call i8* @unboxObject(%struct.BoxedValue* %value)
  %result4 = call double @print_string(i8* %obj_value)
  ret double %result4
}

define double @print(double %value) {
entry:
  %result = call double @print_double(double %value)
  ret double %result
}

; String conversion functions
define i8* @double_to_string(double %value) {
entry:
  %buffer = call i8* @malloc(i32 64)
  %format = getelementptr [4 x i8], [4 x i8]* @.str_double_format, i32 0, i32 0
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, double %value)
  ret i8* %buffer
}

define i8* @bool_to_string(i1 %value) {
entry:
  %buffer = call i8* @malloc(i32 8)
  %true_str = getelementptr [5 x i8], [5 x i8]* @.str_true, i32 0, i32 0
  %false_str = getelementptr [6 x i8], [6 x i8]* @.str_false, i32 0, i32 0
  %str_to_copy = select i1 %value, i8* %true_str, i8* %false_str
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %str_to_copy)
  ret i8* %buffer
}

define i8* @object_to_string(i8* %obj) {
entry:
  %buffer = call i8* @malloc(i32 32)
  %format = getelementptr [12 x i8], [12 x i8]* @.str_object_format, i32 0, i32 0
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %obj)
  ret i8* %buffer
}

define i8* @concat_strings(i8* %str1, i8* %str2) {
entry:
  %buffer = call i8* @malloc(i32 256)
  %format = getelementptr [7 x i8], [7 x i8]* @.str_concat_format, i32 0, i32 0
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)
  ret i8* %buffer
}

define i8* @concat_strings_ws(i8* %str1, i8* %str2) {
entry:
  %buffer = call i8* @malloc(i32 256)
  %format = getelementptr [10 x i8], [10 x i8]* @.str_concat_ws_format, i32 0, i32 0
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)
  ret i8* %buffer
}

@.str_double = private unnamed_addr constant [4 x i8] c"%f\0A\00"
@.str_bool_true = private unnamed_addr constant [6 x i8] c"true\0A\00"
@.str_bool_false = private unnamed_addr constant [7 x i8] c"false\0A\00"
@.str_string = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@.str_double_format = private unnamed_addr constant [3 x i8] c"%f\00"
@.str_true = private unnamed_addr constant [5 x i8] c"true\00"
@.str_false = private unnamed_addr constant [6 x i8] c"false\00"
@.str_concat_format = private unnamed_addr constant [5 x i8] c"%s%s\00"
@.str_concat_ws_format = private unnamed_addr constant [6 x i8] c"%s %s\00"
@.str_null = private unnamed_addr constant [5 x i8] c"null\00"
@.str_object_format = private unnamed_addr constant [12 x i8] c"Object: %s\0A\00"
@.str_error = private unnamed_addr constant [8 x i8] c"[ERROR]\00"

define i32 @main() {
entry:
  %bool_conv_1 = fcmp one double 1.0, 0.0
  %call_0 = call double @print_bool(i1 %bool_conv_1)
  %bool_conv_3 = fcmp one double 0.0, 0.0
  %call_2 = call double @print_bool(i1 %bool_conv_3)
  %boxed_5 = call %struct.BoxedValue* @boxBoolean(i1 true)
  %boxed_6 = call %struct.BoxedValue* @boxString(i8* @.str.3658472883277130625)
  %boxed_op_4 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_5, %struct.BoxedValue* %boxed_6)
  %call_7 = call double @print_string(i8* %boxed_op_4)
  ret i32 0
}

