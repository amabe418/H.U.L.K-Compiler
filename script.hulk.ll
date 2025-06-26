%struct.BoxedValue = type { i8, [7 x i8], i64 }

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

define %struct.BoxedValue* @boxedModulo(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = frem double %left_num, %right_num
  %boxed = call %struct.BoxedValue* @boxNumber(double %result)
  ret %struct.BoxedValue* %boxed
}

define %struct.BoxedValue* @boxedPower(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  %left_num = call double @unboxNumber(%struct.BoxedValue* %left)
  %right_num = call double @unboxNumber(%struct.BoxedValue* %right)
  %result = call double @pow(double %left_num, double %right_num)
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

define %struct.BoxedValue* @boxedConcatenateWithSpace(%struct.BoxedValue* %left, %struct.BoxedValue* %right) {
entry:
  ; Check for null operands
  %left_null = icmp eq %struct.BoxedValue* %left, null
  %right_null = icmp eq %struct.BoxedValue* %right, null
  %any_null = or i1 %left_null, %right_null
  br i1 %any_null, label %handle_null, label %process_operands
handle_null:
  ; Return null if any operand is null
  %null_result = call %struct.BoxedValue* @boxNull()
  ret %struct.BoxedValue* %null_result
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
  %null_str_result = call %struct.BoxedValue* @boxNull()
  ret %struct.BoxedValue* %null_str_result
concatenate:
  ; Concatenate both strings with space
  %result = call i8* @concat_strings_ws(i8* %left_str, i8* %right_str)
  ; Check if concatenation failed
  %concat_failed = icmp eq i8* %result, null
  br i1 %concat_failed, label %handle_concat_failure, label %box_result
handle_concat_failure:
  ; Return null if concatenation failed
  %failure_result = call %struct.BoxedValue* @boxNull()
  ret %struct.BoxedValue* %failure_result
box_result:
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

@.str.4433806826669766996 = private unnamed_addr constant [14 x i8] c"Hello, World!\00"
@.str.13716856615655837043 = private unnamed_addr constant [8 x i8] c"Hello, \00"
@.str.12133625061235258632 = private unnamed_addr constant [7 x i8] c"World!\00"
@.str.15911808872533315919 = private unnamed_addr constant [57 x i8] c"Probando declaracion de variables e impresion de valores\00"
@.str.815733333669765891 = private unnamed_addr constant [13 x i8] c"hello, world\00"
@.str.1145924097123205603 = private unnamed_addr constant [77 x i8] c"Probando declaracion de variables con condicionales e impresion de su valor:\00"
@.str.1897382925244004874 = private unnamed_addr constant [21 x i8] c"Probando bucle while\00"
@.str.9354905337115868815 = private unnamed_addr constant [19 x i8] c"Probando funciones\00"
@.str.15548335941765476253 = private unnamed_addr constant [13 x i8] c"Sucesor de 0\00"
@.str.12115510654860052758 = private unnamed_addr constant [35 x i8] c"Funcion que devuelve Hello, World!\00"
@.str.7750308374451649530 = private unnamed_addr constant [13 x i8] c"Hello, World\00"
@.str.5985627218495613532 = private unnamed_addr constant [22 x i8] c"Probando recursividad\00"
@.str.10139107239403537967 = private unnamed_addr constant [28 x i8] c"n-esimo numero de fibonacci\00"
@.str.16777526998423184521 = private unnamed_addr constant [7 x i8] c"fib(5)\00"
@.str.1282047880766431650 = private unnamed_addr constant [42 x i8] c"Probando funcion con parametros dinamicos\00"
@.str.17241328489943237090 = private unnamed_addr constant [24 x i8] c"f(x,y) => Hello, World!\00"
@.str.2257787302229234269 = private unnamed_addr constant [33 x i8] c"f(a,b), donde a y b son strings.\00"
@.str.2603192927274642682 = private unnamed_addr constant [2 x i8] c"a\00"
@.str.4947675599669400333 = private unnamed_addr constant [2 x i8] c"b\00"
@.str.16975543816661888491 = private unnamed_addr constant [7 x i8] c"f(2,3)\00"
@.str.9099277997342803831 = private unnamed_addr constant [14 x i8] c"f(true,false)\00"
@.str.10599700700371220778 = private unnamed_addr constant [2 x i8] c"
\00"
@.str.14939180569931067128 = private unnamed_addr constant [70 x i8] c"Probando funcion con parametro dinamico y tipo de retorno desconocido\00"
@.str.13480934455297431090 = private unnamed_addr constant [5 x i8] c"true\00"
@.str.10521035107279712938 = private unnamed_addr constant [56 x i8] c"Probando otra funcion recursiva, suma recursiva hasta n\00"
@.str.5169639892928245925 = private unnamed_addr constant [19 x i8] c"Probando for loops\00"
@.str.6482128353538638029 = private unnamed_addr constant [18 x i8] c"El valor de i es \00"
@.str.18086418558974010994 = private unnamed_addr constant [14 x i8] c"Moviendo i a \00"
@.str.5849972387832478685 = private unnamed_addr constant [26 x i8] c"Probando concatenaciones:\00"
@.str.1724119774529258799 = private unnamed_addr constant [6 x i8] c"false\00"
@.str.11725737811754652148 = private unnamed_addr constant [6 x i8] c"Hola \00"
@.str.15464591742653079950 = private unnamed_addr constant [6 x i8] c"Mundo\00"
@.str.2169371982377735806 = private unnamed_addr constant [2 x i8] c"4\00"
@.str.12790462614085517521 = private unnamed_addr constant [7 x i8] c"Hola, \00"
@.str.15064472072739003433 = private unnamed_addr constant [7 x i8] c"Mundo!\00"
@.str.5286563105228630907 = private unnamed_addr constant [15 x i8] c"Probando tipos\00"
%struct.Car = type { %struct.BoxedValue*, %struct.BoxedValue* }
@.str.4210902055800454461 = private unnamed_addr constant [9 x i8] c"mercedes\00"
%struct.vector = type { double, double }
define double @successor(double) {
entry:
  %param_69 = alloca double
  store double %0, double* %param_69
  %load_70 = load double, double* %param_69
  %boxed_op_71 = fadd double %load_70, 1.000000
  ret double %boxed_op_71
}

define i8* @hw() {
entry:
  ret i8* @.str.7750308374451649530
}

define double @fib(double) {
entry:
  %param_80 = alloca double
  store double %0, double* %param_80
  %load_81 = load double, double* %param_80
  %boxed_op_82 = fcmp ult double %load_81, 2.000000
  br i1 %boxed_op_82, label %then_83, label %else_84

then_83:
  br label %ifcont_85

else_84:
  %load_86 = load double, double* %param_80
  %boxed_op_87 = fsub double %load_86, 1.000000
  %call_88 = call double @fib(double %boxed_op_87)
  %load_89 = load double, double* %param_80
  %boxed_op_90 = fsub double %load_89, 2.000000
  %call_91 = call double @fib(double %boxed_op_90)
  %boxed_op_92 = fadd double %call_88, %call_91
  br label %ifcont_85

ifcont_85:
  %iftmp_93 = phi double [ 1.000000, %then_83 ], [ %boxed_op_92, %else_84 ]
  ret double %iftmp_93
}

define i8* @f(%struct.BoxedValue*, %struct.BoxedValue*) {
entry:
  %param_98 = alloca %struct.BoxedValue*
  store %struct.BoxedValue* %0, %struct.BoxedValue** %param_98
  %param_99 = alloca %struct.BoxedValue*
  store %struct.BoxedValue* %1, %struct.BoxedValue** %param_99
  ret i8* @.str.4433806826669766996
}

define %struct.BoxedValue* @id(%struct.BoxedValue*) {
entry:
  %param_119 = alloca %struct.BoxedValue*
  store %struct.BoxedValue* %0, %struct.BoxedValue** %param_119
  %load_120 = load %struct.BoxedValue*, %struct.BoxedValue** %param_119
  ret %struct.BoxedValue* %load_120
}

define double @sum_to(double) {
entry:
  %param_132 = alloca double
  store double %0, double* %param_132
  %load_133 = load double, double* %param_132
  %boxed_op_134 = fcmp ueq double %load_133, 1.000000
  br i1 %boxed_op_134, label %then_135, label %else_136

then_135:
  br label %ifcont_137

else_136:
  %load_138 = load double, double* %param_132
  %load_139 = load double, double* %param_132
  %boxed_op_140 = fsub double %load_139, 1.000000
  %call_141 = call double @sum_to(double %boxed_op_140)
  %boxed_op_142 = fadd double %load_138, %call_141
  br label %ifcont_137

ifcont_137:
  %iftmp_143 = phi double [ 0.000000, %then_135 ], [ %boxed_op_142, %else_136 ]
  ret double %iftmp_143
}

define i8* @concat(i8*, i8*) {
entry:
  %param_187 = alloca i8*
  store i8* %0, i8** %param_187
  %param_188 = alloca i8*
  store i8* %1, i8** %param_188
  %load_189 = load i8*, i8** %param_187
  %load_190 = load i8*, i8** %param_188
  %boxed_op_191 = call i8* @concat_strings(i8* %load_189, i8* %load_190)
  ret i8* %boxed_op_191
}

define %struct.BoxedValue* @Car_getBrand(%struct.Car* %self) {
entry:
  %field_ptr_196 = getelementptr %struct.Car, %struct.Car* %self, i32 0, i32 1
  %getattr_195 = load %struct.BoxedValue*, %struct.BoxedValue** %field_ptr_196
  ret %struct.BoxedValue* %getattr_195
}

define %struct.BoxedValue* @Car_getWheels(%struct.Car* %self) {
entry:
  %field_ptr_198 = getelementptr %struct.Car, %struct.Car* %self, i32 0, i32 0
  %getattr_197 = load %struct.BoxedValue*, %struct.BoxedValue** %field_ptr_198
  ret %struct.BoxedValue* %getattr_197
}

define double @vector_norma(%struct.vector* %self) {
entry:
  %field_ptr_218 = getelementptr %struct.vector, %struct.vector* %self, i32 0, i32 0
  %getattr_217 = load double, double* %field_ptr_218
  %field_ptr_220 = getelementptr %struct.vector, %struct.vector* %self, i32 0, i32 1
  %getattr_219 = load double, double* %field_ptr_220
  %boxed_op_221 = fadd double %getattr_217, %getattr_219
  ret double %boxed_op_221
}

define i32 @main() {
entry:
  %call_0 = call double @print_string(i8* @.str.4433806826669766996)
  %bool_1 = add i1 0, 1
  %call_2 = call double @print_bool(i1 %bool_1)
  %bool_3 = add i1 0, 0
  %call_4 = call double @print_bool(i1 %bool_3)
  %bool_5 = add i1 0, 1
  %bool_6 = add i1 0, 0
  %boxed_op_7 = and i1 %bool_5, %bool_6
  %call_8 = call double @print_bool(i1 %boxed_op_7)
  %bool_9 = add i1 0, 0
  %bool_10 = add i1 0, 1
  %boxed_op_11 = or i1 %bool_9, %bool_10
  %call_12 = call double @print_bool(i1 %boxed_op_11)
  %boxed_op_13 = fadd double 3.000000, 2.000000
  %call_14 = call double @print_double(double %boxed_op_13)
  %boxed_op_15 = fsub double 3.000000, 2.000000
  %call_16 = call double @print_double(double %boxed_op_15)
  %boxed_op_17 = fmul double 3.000000, 2.000000
  %call_18 = call double @print_double(double %boxed_op_17)
  %boxed_op_19 = fdiv double 3.000000, 2.000000
  %call_20 = call double @print_double(double %boxed_op_19)
  %boxed_op_21 = fsub double 2.000000, 3.000000
  %call_22 = call double @print_double(double %boxed_op_21)
  %boxed_op_23 = call double @pow(double 2.000000, double 3.000000)
  %call_24 = call double @print_double(double %boxed_op_23)
  %boxed_op_25 = frem double 8.000000, 2.000000
  %call_26 = call double @print_double(double %boxed_op_25)
  %boxed_op_27 = call i8* @concat_strings(i8* @.str.13716856615655837043, i8* @.str.12133625061235258632)
  %call_28 = call double @print_string(i8* %boxed_op_27)
  %call_29 = call double @print_string(i8* @.str.15911808872533315919)
  %a_30 = alloca double
  store double 2.000000, double* %a_30
  %b_31 = alloca double
  store double 3.000000, double* %b_31
  %c_32 = alloca double
  store double 4.000000, double* %c_32
  %d_33 = alloca double
  store double 5.000000, double* %d_33
  %call_34 = call double @print_string(i8* @.str.815733333669765891)
  %load_35 = load double, double* %a_30
  %load_36 = load double, double* %b_31
  %boxed_op_37 = fadd double %load_35, %load_36
  %load_38 = load double, double* %c_32
  %boxed_op_39 = fadd double %boxed_op_37, %load_38
  %load_40 = load double, double* %d_33
  %boxed_op_41 = fadd double %boxed_op_39, %load_40
  %call_42 = call double @print_double(double %boxed_op_41)
  %bool_43 = add i1 0, 1
  %call_44 = call double @print_bool(i1 %bool_43)
  %bool_45 = add i1 0, 0
  %call_46 = call double @print_bool(i1 %bool_45)
  %call_47 = call double @print_string(i8* @.str.1145924097123205603)
  %boxed_op_48 = fcmp ult double 3.000000, 4.000000
  br i1 %boxed_op_48, label %then_49, label %else_50

then_49:
  br label %ifcont_51

else_50:
  br label %ifcont_51

ifcont_51:
  %iftmp_52 = phi double [ 5.000000, %then_49 ], [ 10.000000, %else_50 ]
  %a_53 = alloca double
  store double %iftmp_52, double* %a_53
  %load_54 = load double, double* %a_53
  %call_55 = call double @print_double(double %load_54)
  %call_56 = call double @print_string(i8* @.str.1897382925244004874)
  %a_57 = alloca double
  store double 5.000000, double* %a_57
  br label %while_header_58

while_header_58:
  %load_61 = load double, double* %a_57
  %boxed_op_62 = fcmp ugt double %load_61, 0.000000
  br i1 %boxed_op_62, label %while_body_59, label %while_exit_60

while_body_59:
  %load_63 = load double, double* %a_57
  %call_64 = call double @print_double(double %load_63)
  %load_65 = load double, double* %a_57
  %boxed_op_66 = fsub double %load_65, 1.000000
  store double %boxed_op_66, double* %a_57
  br label %while_header_58

while_exit_60:
  %call_67 = call double @print_string(i8* @.str.9354905337115868815)
  %call_68 = call double @print_string(i8* @.str.15548335941765476253)
  %call_72 = call double @successor(double 0.000000)
  %call_73 = call double @print_double(double %call_72)
  %call_74 = call double @print_string(i8* @.str.12115510654860052758)
  %call_75 = call i8* @hw()
  %call_76 = call double @print_string(i8* %call_75)
  %call_77 = call double @print_string(i8* @.str.5985627218495613532)
  %call_78 = call double @print_string(i8* @.str.10139107239403537967)
  %call_79 = call double @print_string(i8* @.str.16777526998423184521)
  %call_94 = call double @fib(double 5.000000)
  %call_95 = call double @print_double(double %call_94)
  %call_96 = call double @print_string(i8* @.str.1282047880766431650)
  %call_97 = call double @print_string(i8* @.str.17241328489943237090)
  %call_100 = call double @print_string(i8* @.str.2257787302229234269)
  %boxed_101 = call %struct.BoxedValue* @boxString(i8* @.str.2603192927274642682)
  %boxed_102 = call %struct.BoxedValue* @boxString(i8* @.str.4947675599669400333)
  %call_103 = call i8* @f(%struct.BoxedValue* %boxed_101, %struct.BoxedValue* %boxed_102)
  %call_104 = call double @print_string(i8* %call_103)
  %call_105 = call double @print_string(i8* @.str.16975543816661888491)
  %boxed_106 = call %struct.BoxedValue* @boxNumber(double 2.000000)
  %boxed_107 = call %struct.BoxedValue* @boxNumber(double 3.000000)
  %call_108 = call i8* @f(%struct.BoxedValue* %boxed_106, %struct.BoxedValue* %boxed_107)
  %call_109 = call double @print_string(i8* %call_108)
  %call_110 = call double @print_string(i8* @.str.9099277997342803831)
  %bool_111 = add i1 0, 1
  %boxed_112 = call %struct.BoxedValue* @boxBoolean(i1 %bool_111)
  %bool_113 = add i1 0, 0
  %boxed_114 = call %struct.BoxedValue* @boxBoolean(i1 %bool_113)
  %call_115 = call i8* @f(%struct.BoxedValue* %boxed_112, %struct.BoxedValue* %boxed_114)
  %call_116 = call double @print_string(i8* %call_115)
  %call_117 = call double @print_string(i8* @.str.10599700700371220778)
  %call_118 = call double @print_string(i8* @.str.14939180569931067128)
  %boxed_121 = call %struct.BoxedValue* @boxString(i8* @.str.13480934455297431090)
  %call_122 = call %struct.BoxedValue* @id(%struct.BoxedValue* %boxed_121)
  %call_123 = call double @print_boxed(%struct.BoxedValue* %call_122)
  %bool_124 = add i1 0, 1
  %boxed_125 = call %struct.BoxedValue* @boxBoolean(i1 %bool_124)
  %call_126 = call %struct.BoxedValue* @id(%struct.BoxedValue* %boxed_125)
  %call_127 = call double @print_boxed(%struct.BoxedValue* %call_126)
  %boxed_128 = call %struct.BoxedValue* @boxNumber(double 1.000000)
  %call_129 = call %struct.BoxedValue* @id(%struct.BoxedValue* %boxed_128)
  %call_130 = call double @print_boxed(%struct.BoxedValue* %call_129)
  %call_131 = call double @print_string(i8* @.str.10521035107279712938)
  %call_144 = call double @sum_to(double 100.000000)
  %call_145 = call double @print_double(double %call_144)
  %call_146 = call double @print_string(i8* @.str.5169639892928245925)
  %i_150 = alloca double
  %start_int_151 = fptosi double 1.000000 to i32
  %end_int_152 = fptosi double 7.000000 to i32
  %counter_153 = alloca i32
  store i32 %start_int_151, i32* %counter_153
  br label %for_header_147

for_header_147:
  %counter_load_154 = load i32, i32* %counter_153
  %continue_cond_155 = icmp slt i32 %counter_load_154, %end_int_152
  br i1 %continue_cond_155, label %for_body_148, label %for_exit_149

for_body_148:
  %counter_double_156 = sitofp i32 %counter_load_154 to double
  store double %counter_double_156, double* %i_150
  %load_157 = load double, double* %i_150
  %boxed_159 = call %struct.BoxedValue* @boxString(i8* @.str.6482128353538638029)
  %boxed_160 = call %struct.BoxedValue* @boxNumber(double %load_157)
  %boxed_op_158 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_159, %struct.BoxedValue* %boxed_160)
  %call_161 = call double @print_string(i8* %boxed_op_158)
  %load_162 = load double, double* %i_150
  %boxed_op_163 = fadd double %load_162, 1.000000
  %boxed_165 = call %struct.BoxedValue* @boxString(i8* @.str.18086418558974010994)
  %boxed_166 = call %struct.BoxedValue* @boxNumber(double %boxed_op_163)
  %boxed_op_164 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_165, %struct.BoxedValue* %boxed_166)
  %call_167 = call double @print_string(i8* %boxed_op_164)
  %counter_inc_168 = add i32 %counter_load_154, 1
  store i32 %counter_inc_168, i32* %counter_153
  br label %for_header_147

for_exit_149:
  %call_169 = call double @print_string(i8* @.str.10599700700371220778)
  %call_170 = call double @print_string(i8* @.str.5849972387832478685)
  %bool_171 = add i1 0, 1
  %boxed_173 = call %struct.BoxedValue* @boxBoolean(i1 %bool_171)
  %boxed_174 = call %struct.BoxedValue* @boxString(i8* @.str.1724119774529258799)
  %boxed_op_172 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_173, %struct.BoxedValue* %boxed_174)
  %call_175 = call double @print_string(i8* %boxed_op_172)
  %bool_176 = add i1 0, 1
  %boxed_178 = call %struct.BoxedValue* @boxString(i8* @.str.1724119774529258799)
  %boxed_179 = call %struct.BoxedValue* @boxBoolean(i1 %bool_176)
  %boxed_op_177 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_178, %struct.BoxedValue* %boxed_179)
  %call_180 = call double @print_string(i8* %boxed_op_177)
  %boxed_op_181 = call i8* @concat_strings(i8* @.str.11725737811754652148, i8* @.str.15464591742653079950)
  %call_182 = call double @print_string(i8* %boxed_op_181)
  %boxed_184 = call %struct.BoxedValue* @boxString(i8* @.str.2169371982377735806)
  %boxed_185 = call %struct.BoxedValue* @boxNumber(double 2.000000)
  %boxed_op_183 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_184, %struct.BoxedValue* %boxed_185)
  %call_186 = call double @print_string(i8* %boxed_op_183)
  %call_192 = call i8* @concat(i8* @.str.12790462614085517521, i8* @.str.15064472072739003433)
  %call_193 = call double @print_string(i8* %call_192)
  %call_194 = call double @print_string(i8* @.str.5286563105228630907)
  %new_obj_199 = alloca %struct.Car
  %param_200 = alloca double
  store double 4.000000, double* %param_200
  %param_201 = alloca i8*
  store i8* @.str.4210902055800454461, i8** %param_201
  %field_ptr_202 = getelementptr %struct.Car, %struct.Car* %new_obj_199, i32 0, i32 0
  %load_203 = load %struct.BoxedValue*, %struct.BoxedValue** %param_200
  store %struct.BoxedValue* %load_203, %struct.BoxedValue** %field_ptr_202
  %field_ptr_204 = getelementptr %struct.Car, %struct.Car* %new_obj_199, i32 0, i32 1
  %load_205 = load %struct.BoxedValue*, %struct.BoxedValue** %param_201
  store %struct.BoxedValue* %load_205, %struct.BoxedValue** %field_ptr_204
  %a_206 = alloca %struct.Car*
  store %struct.Car* %new_obj_199, %struct.Car** %a_206
  %call_207 = call double @print_double(double 2.000000)
  %a_208 = alloca double
  store double 42.000000, double* %a_208
  %bool_209 = add i1 0, 1
  %b_210 = alloca i1
  store i1 %bool_209, i1* %b_210
  %load_211 = load double, double* %a_208
  %load_212 = load i1, i1* %b_210
  %boxed_214 = call %struct.BoxedValue* @boxNumber(double %load_211)
  %boxed_215 = call %struct.BoxedValue* @boxBoolean(i1 %load_212)
  %boxed_op_213 = call %struct.BoxedValue* @boxedConcatenate(%struct.BoxedValue* %boxed_214, %struct.BoxedValue* %boxed_215)
  %call_216 = call double @print_string(i8* %boxed_op_213)
  ret i32 0
}

