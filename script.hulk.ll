%struct.BoxedValue = type { i8, [7 x i8], i64 }
%struct.Object = type { i8 }

; Built-in function declarations
declare double @sqrt(double)
declare double @sin(double)
declare double @cos(double)
declare double @tan(double)
declare double @exp(double)
declare double @log(double)
declare double @pow(double, double)
declare i32 @rand()
declare i32 @printf(i8*, ...)
declare i32 @sprintf(i8*, i8*, ...)
declare i8* @malloc(i32)
declare void @free(i8*)
declare double @strtod(i8*, i8**)

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
  ; Check for null pointer
  %is_null = icmp eq %struct.BoxedValue* %box, null
  br i1 %is_null, label %return_false, label %check_type
return_false:
  ret i1 false
check_type:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %type_int = zext i8 %type to i32
  %result = icmp eq i8 %type, 0
  %result_int = zext i1 %result to i32
  ret i1 %result
}

define i1 @isString(%struct.BoxedValue* %box) {
entry:
  ; Check for null pointer
  %is_null = icmp eq %struct.BoxedValue* %box, null
  br i1 %is_null, label %return_false, label %check_type
return_false:
  ret i1 false
check_type:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %type_int = zext i8 %type to i32
  %result = icmp eq i8 %type, 1
  %result_int = zext i1 %result to i32
  ret i1 %result
}

define i1 @isBoolean(%struct.BoxedValue* %box) {
entry:
  ; Check for null pointer
  %is_null = icmp eq %struct.BoxedValue* %box, null
  br i1 %is_null, label %return_false, label %check_type
return_false:
  ret i1 false
check_type:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %type_int = zext i8 %type to i32
  %result = icmp eq i8 %type, 2
  %result_int = zext i1 %result to i32
  ret i1 %result
}

define i1 @isObject(%struct.BoxedValue* %box) {
entry:
  ; Check for null pointer
  %is_null = icmp eq %struct.BoxedValue* %box, null
  br i1 %is_null, label %return_false, label %check_type
return_false:
  ret i1 false
check_type:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %type_int = zext i8 %type to i32
  %result = icmp eq i8 %type, 3
  %result_int = zext i1 %result to i32
  ret i1 %result
}

define i1 @isNull(%struct.BoxedValue* %box) {
entry:
  ; Check for null pointer
  %is_null = icmp eq %struct.BoxedValue* %box, null
  br i1 %is_null, label %return_true, label %check_type
return_true:
  ret i1 true
check_type:
  %type_ptr = getelementptr %struct.BoxedValue, %struct.BoxedValue* %box, i32 0, i32 0
  %type = load i8, i8* %type_ptr
  %type_int = zext i8 %type to i32
  %result = icmp eq i8 %type, 4
  %result_int = zext i1 %result to i32
  ret i1 %result
}

define double @boxedAdd(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fadd double %left_num, %right_num
  ret double %result
}

define double @boxedSubtract(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fsub double %left_num, %right_num
  ret double %result
}

define double @boxedMultiply(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fmul double %left_num, %right_num
  ret double %result
}

define double @boxedDivide(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fdiv double %left_num, %right_num
  ret double %result
}

define double @boxedModulo(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = frem double %left_num, %right_num
  ret double %result
}

define double @boxedPower(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = call double @pow(double %left_num, double %right_num)
  ret double %result
}

define i1 @boxedEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ueq double %left_num, %right_num
  ret i1 %result
}

define i1 @boxedNotEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp une double %left_num, %right_num
  ret i1 %result
}

define i1 @boxedLessThan(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ult double %left_num, %right_num
  ret i1 %result
}

define i1 @boxedGreaterThan(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ugt double %left_num, %right_num
  ret i1 %result
}

define i1 @boxedLessEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp ule double %left_num, %right_num
  ret i1 %result
}

define i1 @boxedGreaterEqual(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = fcmp uge double %left_num, %right_num
  ret i1 %result
}

define i1 @boxedLogicalAnd(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_bool = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %right_bool = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %result = and i1 %left_bool, %right_bool
  ret i1 %result
}

define i1 @boxedLogicalOr(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_bool = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %right_bool = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %result = or i1 %left_bool, %right_bool
  ret i1 %result
}

define i1 @boxedLogicalNot(%struct.BoxedValue* %operand) {
entry:
  %bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %operand)
  %result = xor i1 %bool_val, true
  ret i1 %result
}

define i8* @boxedConcatenate(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  ; Check for null operands
  %left_null = icmp eq %struct.BoxedValue* %left, null
  %right_null = icmp eq %struct.BoxedValue* %right, null
  %any_null = or i1 %left_null, %right_null
  br i1 %any_null, label %handle_null, label %process_operands
handle_null:
  ; Return null if any operand is null
  ret i8* null
process_operands:
  ; Check left operand type and convert to string if needed
  %left_is_string = call i1 @isString(%struct.BoxedValue* %left)
  br i1 %left_is_string, label %left_is_str, label %left_check_number
left_is_str:
  %left_str_1 = call i8* @unboxString(%struct.BoxedValue* %left)
  br label %left_done
left_check_number:
  %left_is_number = call i1 @isNumber(%struct.BoxedValue* %left)
  br i1 %left_is_number, label %left_convert_number, label %left_check_bool
left_convert_number:
  %left_num_val = call double @unboxNumber(%struct.BoxedValue* %left)
  %left_str_2 = call i8* @double_to_string(double %left_num_val)
  br label %left_done
left_check_bool:
  %left_is_bool = call i1 @isBoolean(%struct.BoxedValue* %left)
  br i1 %left_is_bool, label %left_convert_bool, label %left_convert_object
left_convert_bool:
  %left_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %left_str_3 = call i8* @bool_to_string(i1 %left_bool_val)
  br label %left_done
left_convert_object:
  %left_obj_val = call i8* @unboxObject(%struct.BoxedValue* %left)
  %left_str_4 = call i8* @object_to_string(i8* %left_obj_val)
  br label %left_done
left_done:
  %left_str = phi i8* [ %left_str_1, %left_is_str ], [ %left_str_2, %left_convert_number ], [ %left_str_3, %left_convert_bool ], [ %left_str_4, %left_convert_object ]
  ; Check right operand type and convert to string if needed
  %right_is_string = call i1 @isString(%struct.BoxedValue* %right)
  br i1 %right_is_string, label %right_is_str, label %right_check_number
right_is_str:
  %right_str_1 = call i8* @unboxString(%struct.BoxedValue* %right)
  br label %right_done
right_check_number:
  %right_is_number = call i1 @isNumber(%struct.BoxedValue* %right)
  br i1 %right_is_number, label %right_convert_number, label %right_check_bool
right_convert_number:
  %right_num_val = call double @unboxNumber(%struct.BoxedValue* %right)
  %right_str_2 = call i8* @double_to_string(double %right_num_val)
  br label %right_done
right_check_bool:
  %right_is_bool = call i1 @isBoolean(%struct.BoxedValue* %right)
  br i1 %right_is_bool, label %right_convert_bool, label %right_convert_object
right_convert_bool:
  %right_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %right_str_3 = call i8* @bool_to_string(i1 %right_bool_val)
  br label %right_done
right_convert_object:
  %right_obj_val = call i8* @unboxObject(%struct.BoxedValue* %right)
  %right_str_4 = call i8* @object_to_string(i8* %right_obj_val)
  br label %right_done
right_done:
  %right_str = phi i8* [ %right_str_1, %right_is_str ], [ %right_str_2, %right_convert_number ], [ %right_str_3, %right_convert_bool ], [ %right_str_4, %right_convert_object ]
  ; Check for null strings after conversion
  %left_str_null = icmp eq i8* %left_str, null
  %right_str_null = icmp eq i8* %right_str, null
  %any_str_null = or i1 %left_str_null, %right_str_null
  br i1 %any_str_null, label %handle_str_null, label %concatenate
handle_str_null:
  ; If any string is null after conversion, return null
  ret i8* null
concatenate:
  ; Concatenate both strings
  %result = call i8* @concat_strings(i8* %left_str, i8* %right_str)
  ; Check if concatenation failed
  %concat_failed = icmp eq i8* %result, null
  br i1 %concat_failed, label %handle_concat_failure, label %return_result
handle_concat_failure:
  ; Return null if concatenation failed
  ret i8* null
return_result:
  ; Return the concatenated string directly
  ret i8* %result
}

define i8* @boxedConcatenateWithSpace(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  ; Check for null operands
  %left_null = icmp eq %struct.BoxedValue* %left, null
  %right_null = icmp eq %struct.BoxedValue* %right, null
  %any_null = or i1 %left_null, %right_null
  br i1 %any_null, label %handle_null, label %process_operands
handle_null:
  ; Return null if any operand is null
  ret i8* null
process_operands:
  ; Check left operand type and convert to string if needed
  %left_is_string = call i1 @isString(%struct.BoxedValue* %left)
  br i1 %left_is_string, label %left_is_str, label %left_check_number
left_is_str:
  %left_str_1 = call i8* @unboxString(%struct.BoxedValue* %left)
  br label %left_done
left_check_number:
  %left_is_number = call i1 @isNumber(%struct.BoxedValue* %left)
  br i1 %left_is_number, label %left_convert_number, label %left_check_bool
left_convert_number:
  %left_num_val = call double @unboxNumber(%struct.BoxedValue* %left)
  %left_str_2 = call i8* @double_to_string(double %left_num_val)
  call double @print_string(i8* %left_str_2)
  br label %left_done
left_check_bool:
  %left_is_bool = call i1 @isBoolean(%struct.BoxedValue* %left)
  br i1 %left_is_bool, label %left_convert_bool, label %left_convert_object
left_convert_bool:
  %left_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %left)
  %left_str_3 = call i8* @bool_to_string(i1 %left_bool_val)
  call double @print_string(i8* %left_str_3)
  br label %left_done
left_convert_object:
  %left_obj_val = call i8* @unboxObject(%struct.BoxedValue* %left)
  %left_str_4 = call i8* @object_to_string(i8* %left_obj_val)
  br label %left_done
left_done:
  %left_str = phi i8* [ %left_str_1, %left_is_str ], [ %left_str_2, %left_convert_number ], [ %left_str_3, %left_convert_bool ], [ %left_str_4, %left_convert_object ]
  ; Check right operand type and convert to string if needed
  %right_is_string = call i1 @isString(%struct.BoxedValue* %right)
  br i1 %right_is_string, label %right_is_str, label %right_check_number
right_is_str:
  %right_str_1 = call i8* @unboxString(%struct.BoxedValue* %right)
  br label %right_done
right_check_number:
  %right_is_number = call i1 @isNumber(%struct.BoxedValue* %right)
  br i1 %right_is_number, label %right_convert_number, label %right_check_bool
right_convert_number:
  %right_num_val = call double @unboxNumber(%struct.BoxedValue* %right)
  %right_str_2 = call i8* @double_to_string(double %right_num_val)
  br label %right_done
right_check_bool:
  %right_is_bool = call i1 @isBoolean(%struct.BoxedValue* %right)
  br i1 %right_is_bool, label %right_convert_bool, label %right_convert_object
right_convert_bool:
  %right_bool_val = call i1 @unboxBoolean(%struct.BoxedValue* %right)
  %right_str_3 = call i8* @bool_to_string(i1 %right_bool_val)
  br label %right_done
right_convert_object:
  %right_obj_val = call i8* @unboxObject(%struct.BoxedValue* %right)
  %right_str_4 = call i8* @object_to_string(i8* %right_obj_val)
  br label %right_done
right_done:
  %right_str = phi i8* [ %right_str_1, %right_is_str ], [ %right_str_2, %right_convert_number ], [ %right_str_3, %right_convert_bool ], [ %right_str_4, %right_convert_object ]
  ; Check for null strings after conversion
  %left_str_null = icmp eq i8* %left_str, null
  %right_str_null = icmp eq i8* %right_str, null
  %any_str_null = or i1 %left_str_null, %right_str_null
  br i1 %any_str_null, label %handle_str_null, label %concatenate
handle_str_null:
  ; If any string is null after conversion, return null
  ret i8* null
concatenate:
  ; Concatenate both strings with space
  %result = call i8* @concat_strings_ws(i8* %left_str, i8* %right_str)
  ; Check if concatenation failed
  %concat_failed = icmp eq i8* %result, null
  br i1 %concat_failed, label %handle_concat_failure, label %return_result
handle_concat_failure:
  ; Return null if concatenation failed
  ret i8* null
return_result:
  ; Return the concatenated string directly
  ret i8* %result
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
  %error_src = getelementptr [8 x i8], [8 x i8]* @.str_error, i32 0, i32 0
  %format = getelementptr [3 x i8], [3 x i8]* @.str_copy_format, i32 0, i32 0
  %error_result = call i32 (i8*, i8*, ...) @sprintf(i8* %error_str, i8* %format, i8* %error_src)
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

define %struct.Object* @construct_Object() {
entry:
  %obj = call i8* @malloc(i32 8)
  %obj_ptr = bitcast i8* %obj to %struct.Object*
  %dummy_field = getelementptr %struct.Object, %struct.Object* %obj_ptr, i32 0, i32 0
  store i8 0, i8* %dummy_field
  ret %struct.Object* %obj_ptr
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
  %format = getelementptr [3 x i8], [3 x i8]* @.str_copy_format, i32 0, i32 0
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str_to_copy)
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
  %format = getelementptr [5 x i8], [5 x i8]* @.str_concat_format, i32 0, i32 0
  %result = call i32 (i8*, i8*, ...) @sprintf(i8* %buffer, i8* %format, i8* %str1, i8* %str2)
  ret i8* %buffer
}

define i8* @concat_strings_ws(i8* %str1, i8* %str2) {
entry:
  %buffer = call i8* @malloc(i32 256)
  %format = getelementptr [6 x i8], [6 x i8]* @.str_concat_ws_format, i32 0, i32 0
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
@.str_copy_format = private unnamed_addr constant [3 x i8] c"%s\00"
@.str_concat_format = private unnamed_addr constant [5 x i8] c"%s%s\00"
@.str_concat_ws_format = private unnamed_addr constant [6 x i8] c"%s %s\00"
@.str_null = private unnamed_addr constant [5 x i8] c"null\00"
@.str_object_format = private unnamed_addr constant [12 x i8] c"Object: %s\0A\00"
@.str_error = private unnamed_addr constant [8 x i8] c"[ERROR]\00"

@.str.3336901784445824262 = private unnamed_addr constant [12 x i8] c"Hello World\00"
@.str.11682392978016093793 = private unnamed_addr constant [23 x i8] c"The meaning of life is\00"
@.str.10617451631850171135 = private unnamed_addr constant [5 x i8] c"Even\00"
@.str.2824908122688297375 = private unnamed_addr constant [4 x i8] c"odd\00"
@.str.5490211973749801689 = private unnamed_addr constant [5 x i8] c"even\00"
@.str.38878643711491148 = private unnamed_addr constant [4 x i8] c"Odd\00"
@.str.12699613912716938786 = private unnamed_addr constant [6 x i8] c"Magic\00"
@.str.14959822726224643544 = private unnamed_addr constant [5 x i8] c"Woke\00"
@.str.7315302353919506659 = private unnamed_addr constant [5 x i8] c"Dumb\00"
define %struct.BoxedValue* @a(%struct.BoxedValue*) {
entry:
  %param_0 = alloca %struct.BoxedValue*
  store %struct.BoxedValue* %0, %struct.BoxedValue** %param_0
  %load_1 = load %struct.BoxedValue*, %struct.BoxedValue** %param_0
  ret %struct.BoxedValue* %load_1
}

define double @fib(double) {
entry:
  %param_2 = alloca double
  store double %0, double* %param_2
  %load_3 = load double, double* %param_2
  %boxed_op_4 = fcmp ueq double %load_3, 0.000000
  %load_5 = load double, double* %param_2
  %boxed_op_6 = fcmp ueq double %load_5, 1.000000
  %boxed_op_7 = or i1 %boxed_op_4, %boxed_op_6
  br i1 %boxed_op_7, label %then_8, label %else_9

then_8:
  br label %ifcont_10

else_9:
  %load_11 = load double, double* %param_2
  %boxed_op_12 = fsub double %load_11, 1.000000
  %call_13 = call double @fib(double %boxed_op_12)
  %load_14 = load double, double* %param_2
  %boxed_op_15 = fsub double %load_14, 2.000000
  %call_16 = call double @fib(double %boxed_op_15)
  %boxed_op_17 = fadd double %call_13, %call_16
  br label %ifcont_10

ifcont_10:
  %iftmp_18 = phi double [ 1.000000, %then_8 ], [ %boxed_op_17, %else_9 ]
  ret double %iftmp_18
}

define double @tan(double) {
entry:
  %param_19 = alloca double
  store double %0, double* %param_19
  %load_20 = load double, double* %param_19
  %call_21 = call double @sin(double %load_20)
  %load_22 = load double, double* %param_19
  %call_23 = call double @cos(double %load_22)
  %boxed_op_24 = fdiv double %call_21, %call_23
  ret double %boxed_op_24
}

define double @cot(double) {
entry:
  %param_25 = alloca double
  store double %0, double* %param_25
  %load_26 = load double, double* %param_25
  %call_27 = call double @tan(double %load_26)
  %boxed_op_28 = fdiv double 1.000000, %call_27
  ret double %boxed_op_28
}

define double @operate(double, double) {
entry:
  %param_35 = alloca double
  store double %0, double* %param_35
  %param_36 = alloca double
  store double %1, double* %param_36
  %load_37 = load double, double* %param_35
  %load_38 = load double, double* %param_36
  %boxed_op_39 = fadd double %load_37, %load_38
  %call_40 = call double @print_double(double %boxed_op_39)
  %load_41 = load double, double* %param_35
  %load_42 = load double, double* %param_36
  %boxed_op_43 = fsub double %load_41, %load_42
  %call_44 = call double @print_double(double %boxed_op_43)
  %load_45 = load double, double* %param_35
  %load_46 = load double, double* %param_36
  %boxed_op_47 = fmul double %load_45, %load_46
  %call_48 = call double @print_double(double %boxed_op_47)
  %load_49 = load double, double* %param_35
  %load_50 = load double, double* %param_36
  %boxed_op_51 = fdiv double %load_49, %load_50
  %call_52 = call double @print_double(double %boxed_op_51)
  ret double %call_52
}

define double @h(double, double) {
entry:
  %param_57 = alloca double
  store double %0, double* %param_57
  %param_58 = alloca double
  store double %1, double* %param_58
  %load_59 = load double, double* %param_57
  %load_60 = load double, double* %param_58
  %boxed_op_61 = fadd double %load_59, %load_60
  ret double %boxed_op_61
}

define double @g(double, double) {
entry:
  %param_62 = alloca double
  store double %0, double* %param_62
  %param_63 = alloca double
  store double %1, double* %param_63
  %load_64 = load double, double* %param_62
  %load_65 = load double, double* %param_63
  %call_66 = call double @h(double %load_64, double %load_65)
  ret double %call_66
}

define double @f(double, double) {
entry:
  %param_67 = alloca double
  store double %0, double* %param_67
  %param_68 = alloca double
  store double %1, double* %param_68
  %load_69 = load double, double* %param_67
  %load_70 = load double, double* %param_68
  %call_71 = call double @g(double %load_69, double %load_70)
  ret double %call_71
}

define i32 @main() {
entry:
  %call_29 = call double @tan(double %boxed_op_28)
  %boxed_op_30 = call double @pow(double %call_29, double 2.000000)
  %call_31 = call double @cot(double %boxed_op_30)
  %boxed_op_32 = call double @pow(double %call_31, double 2.000000)
  %boxed_op_33 = fadd double %boxed_op_30, %boxed_op_32
  %call_34 = call double @print_double(double %boxed_op_33)
  %boxed_op_53 = fmul double 90.000000, 3.140000
  %boxed_op_54 = fdiv double %boxed_op_53, 180.000000
  %call_55 = call double @sin(double %boxed_op_54)
  %call_56 = call double @print_double(double %call_55)
  %x_72 = alloca double
  store double 2.000000, double* %x_72
  %load_73 = load double, double* %x_72
  %msg_74 = alloca i8*
  store i8* @.str.3336901784445824262, i8** %msg_74
  %load_75 = load i8*, i8** %msg_74
  %call_76 = call double @print_string(i8* %load_75)
  %number_77 = alloca double
  store double 42.000000, double* %number_77
  %text_78 = alloca i8*
  store i8* @.str.11682392978016093793, i8** %text_78
  %load_79 = load i8*, i8** %text_78
  %load_80 = load double, double* %number_77
  %boxed_82 = call %struct.BoxedValue* @boxString(i8* %load_79)
  %boxed_83 = call %struct.BoxedValue* @boxNumber(double %load_80)
  %boxed_op_81 = call i8* @boxedConcatenate(%struct.BoxedValue* %boxed_82, %struct.BoxedValue* %boxed_83)
  %call_84 = call double @print_string(i8* %boxed_op_81)
  %a_85 = alloca double
  store double 6.000000, double* %a_85
  %load_86 = load double, double* %a_85
  %boxed_op_87 = fmul double %load_86, 7.000000
  %b_88 = alloca double
  store double %boxed_op_87, double* %b_88
  %load_89 = load double, double* %b_88
  %call_90 = call double @print_double(double %load_89)
  %a_91 = alloca double
  store double 5.000000, double* %a_91
  %b_92 = alloca double
  store double 10.000000, double* %b_92
  %c_93 = alloca double
  store double 20.000000, double* %c_93
  %load_94 = load double, double* %a_91
  %load_95 = load double, double* %b_92
  %boxed_op_96 = fadd double %load_94, %load_95
  %call_97 = call double @print_double(double %boxed_op_96)
  %load_98 = load double, double* %b_92
  %load_99 = load double, double* %c_93
  %boxed_op_100 = fmul double %load_98, %load_99
  %call_101 = call double @print_double(double %boxed_op_100)
  %load_102 = load double, double* %c_93
  %load_103 = load double, double* %a_91
  %boxed_op_104 = fdiv double %load_102, %load_103
  %call_105 = call double @print_double(double %boxed_op_104)
  %a_106 = alloca double
  store double 42.000000, double* %a_106
  %load_107 = load double, double* %a_106
  %boxed_op_108 = frem double %load_107, 2.000000
  %boxed_op_109 = fcmp ueq double %boxed_op_108, 0.000000
  br i1 %boxed_op_109, label %then_110, label %else_111

then_110:
  %call_113 = call double @print_string(i8* @.str.10617451631850171135)
  br label %ifcont_112

else_111:
  %call_114 = call double @print_string(i8* @.str.2824908122688297375)
  br label %ifcont_112

ifcont_112:
  %iftmp_115 = phi double [ %call_113, %then_110 ], [ %call_114, %else_111 ]
  %a_116 = alloca double
  store double 42.000000, double* %a_116
  %load_117 = load double, double* %a_116
  %boxed_op_118 = frem double %load_117, 2.000000
  %boxed_op_119 = fcmp ueq double %boxed_op_118, 0.000000
  br i1 %boxed_op_119, label %then_120, label %else_121

then_120:
  br label %ifcont_122

else_121:
  br label %ifcont_122

ifcont_122:
  %iftmp_123 = phi double [ @.str.5490211973749801689, %then_120 ], [ @.str.2824908122688297375, %else_121 ]
  %call_124 = call double @print_boxed(%struct.BoxedValue* %iftmp_123)
  %a_125 = alloca double
  store double 42.000000, double* %a_125
  %load_126 = load double, double* %a_125
  %boxed_op_127 = frem double %load_126, 2.000000
  %boxed_op_128 = fcmp ueq double %boxed_op_127, 0.000000
  br i1 %boxed_op_128, label %then_129, label %else_130

then_129:
  %load_132 = load double, double* %a_125
  %call_133 = call double @print_double(double %load_132)
  %call_134 = call double @print_string(i8* @.str.10617451631850171135)
  br label %ifcont_131

else_130:
  %call_135 = call double @print_string(i8* @.str.38878643711491148)
  br label %ifcont_131

ifcont_131:
  %iftmp_136 = phi double [ %call_134, %then_129 ], [ %call_135, %else_130 ]
  %a_137 = alloca double
  store double 42.000000, double* %a_137
  %load_138 = load double, double* %a_137
  %boxed_op_139 = frem double %load_138, 3.000000
  %mod_140 = alloca double
  store double %boxed_op_139, double* %mod_140
  %load_141 = load double, double* %mod_140
  %boxed_op_142 = fcmp ueq double %load_141, 0.000000
  br i1 %boxed_op_142, label %then_143, label %else_144

then_143:
  br label %ifcont_145

else_144:
  %load_146 = load double, double* %mod_140
  %boxed_op_147 = frem double %load_146, 3.000000
  %boxed_op_148 = fcmp ueq double %boxed_op_147, 1.000000
  br i1 %boxed_op_148, label %then_149, label %else_150

then_149:
  br label %ifcont_151

else_150:
  br label %ifcont_151

ifcont_151:
  %iftmp_152 = phi double [ @.str.14959822726224643544, %then_149 ], [ @.str.7315302353919506659, %else_150 ]
  br label %ifcont_145

ifcont_145:
  %iftmp_153 = phi double [ @.str.12699613912716938786, %then_143 ], [ %iftmp_152, %else_144 ]
  %call_154 = call double @print_boxed(%struct.BoxedValue* %iftmp_153)
  %a_155 = alloca double
  store double 20.000000, double* %a_155
  %load_156 = load double, double* %a_155
  %call_157 = call double @print_double(double %load_156)
  %a_158 = alloca double
  store double 42.000000, double* %a_158
  %load_159 = load double, double* %a_158
  %call_160 = call double @print_double(double %load_159)
  %a_161 = alloca double
  store double 7.000000, double* %a_161
  %boxed_op_162 = fmul double 7.000000, 6.000000
  %a_163 = alloca double
  store double %boxed_op_162, double* %a_163
  %load_164 = load double, double* %a_163
  %call_165 = call double @print_double(double %load_164)
  %a_166 = alloca double
  store double 0.000000, double* %a_166
  %load_167 = load double, double* %a_166
  %call_168 = call double @print_double(double %load_167)
  store double 1.000000, double* %a_166
  %load_169 = load double, double* %a_166
  %call_170 = call double @print_double(double %load_169)
  %a_171 = alloca double
  store double 0.000000, double* %a_171
  store double 1.000000, double* %a_171
  %b_172 = alloca double
  store double 1.000000, double* %b_172
  %load_173 = load double, double* %a_171
  %call_174 = call double @print_double(double %load_173)
  %load_175 = load double, double* %b_172
  %call_176 = call double @print_double(double %load_175)
  ret i32 0
}

