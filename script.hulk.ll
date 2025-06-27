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

%struct.Animal = type { i8 }
@.str.17073434912759574580 = private unnamed_addr constant [21 x i8] c"regular Animal noise\00"
%struct.Dog = type { %struct.Animal* }
@.str.18307680477376703391 = private unnamed_addr constant [12 x i8] c"Woof, Woof!\00"
%struct.Chihuahua = type { %struct.Dog* }
define %struct.Animal* @construct_Animal() {
entry:
  %obj_0_raw = call i8* @malloc(i32 8)
  %obj_0 = bitcast i8* %obj_0_raw to %struct.Animal*
  ret %struct.Animal* %obj_0
}

define i8* @Animal_speak(%struct.Animal* %self) {
entry:
  ret i8* @.str.17073434912759574580
}

define %struct.Dog* @construct_Dog() {
entry:
  %obj_1_raw = call i8* @malloc(i32 8)
  %obj_1 = bitcast i8* %obj_1_raw to %struct.Dog*
  %base_obj_2 = call %struct.Animal* @construct_Animal()
  %base_ptr_field_3 = getelementptr %struct.Dog, %struct.Dog* %obj_1, i32 0, i32 0
  store %struct.Animal* %base_obj_2, %struct.Animal** %base_ptr_field_3
  ret %struct.Dog* %obj_1
}

define i8* @Dog_bark(%struct.Dog* %self) {
entry:
  ret i8* @.str.18307680477376703391
}

define i8* @Dog_speak(%struct.Dog* %self) {
entry:
  %base_ptr_4 = getelementptr %struct.Dog, %struct.Dog* %self, i32 0, i32 0
  %base_obj_5 = load %struct.Animal*, %struct.Animal** %base_ptr_4
  %inherited_call_6 = call i8* @Animal_speak(%struct.Animal* %base_obj_5)
  ret i8* %inherited_call_6
}

define %struct.Chihuahua* @construct_Chihuahua() {
entry:
  %obj_7_raw = call i8* @malloc(i32 8)
  %obj_7 = bitcast i8* %obj_7_raw to %struct.Chihuahua*
  %base_obj_8 = call %struct.Dog* @construct_Dog()
  %base_ptr_field_9 = getelementptr %struct.Chihuahua, %struct.Chihuahua* %obj_7, i32 0, i32 0
  store %struct.Dog* %base_obj_8, %struct.Dog** %base_ptr_field_9
  ret %struct.Chihuahua* %obj_7
}

define i8* @Chihuahua_bark(%struct.Chihuahua* %self) {
entry:
  %base_ptr_10 = getelementptr %struct.Chihuahua, %struct.Chihuahua* %self, i32 0, i32 0
  %base_obj_11 = load %struct.Dog*, %struct.Dog** %base_ptr_10
  %inherited_call_12 = call i8* @Dog_bark(%struct.Dog* %base_obj_11)
  ret i8* %inherited_call_12
}

define i32 @main() {
entry:
  %new_obj_13 = call %struct.Dog* @construct_Dog()
  %d_14 = alloca %struct.Dog*
  store %struct.Dog* %new_obj_13, %struct.Dog** %d_14
  %load_15 = load %struct.Dog*, %struct.Dog** %d_14
  %method_call_16 = call i8* @Dog_bark(%struct.Dog* %load_15)
  %call_17 = call double @print_string(i8* %method_call_16)
  %load_18 = load %struct.Dog*, %struct.Dog** %d_14
  %method_call_19 = call i8* @Dog_speak(%struct.Dog* %load_18)
  %call_20 = call double @print_string(i8* %method_call_19)
  %new_obj_21 = call %struct.Chihuahua* @construct_Chihuahua()
  %c_22 = alloca %struct.Chihuahua*
  store %struct.Chihuahua* %new_obj_21, %struct.Chihuahua** %c_22
  %load_23 = load %struct.Chihuahua*, %struct.Chihuahua** %c_22
  %method_call_24 = call i8* @Chihuahua_bark(%struct.Chihuahua* %load_23)
  %call_25 = call double @print_string(i8* %method_call_24)
  ret i32 0
}

