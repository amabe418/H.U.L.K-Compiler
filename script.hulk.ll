%struct.A = type { i8 }
%struct.B = type { i8 }
%struct.C = type { i8 }
@.str.11514100687449187254 = private unnamed_addr constant [8 x i8] c"se pudo\00"
@.str.13097573105356038884 = private unnamed_addr constant [11 x i8] c"no se pudo\00"
; Built-in function declarations
declare double @sqrt(double)
declare double @pow(double, double)
declare double @sin(double)
declare double @cos(double)
declare double @tan(double)
declare double @exp(double)
declare double @log(double)
declare i32 @rand()
declare i32 @printf(i8*, ...)
declare i32 @sprintf(i8*, i8*, ...)
declare i8* @malloc(i64)
declare void @free(i8*)

; Global constants
@PI = global double 3.141592653589793

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

define i32 @main() {
entry:
  %call_0 = call double @rand()
  %binary_1 = fcmp ult double %call_0, 0.500000
  br i1 %binary_1, label %then_2, label %else_3

then_2:
  %size_6 = ptrtoint %struct.B* null to i64
  %new_obj_5 = call i8* @malloc(i64 %size_6)
  %new_obj_5_cast = bitcast i8* %new_obj_5 to %struct.B*
  br label %ifcont_4

else_3:
  %size_8 = ptrtoint %struct.C* null to i64
  %new_obj_7 = call i8* @malloc(i64 %size_8)
  %new_obj_7_cast = bitcast i8* %new_obj_7 to %struct.C*
  br label %ifcont_4

ifcont_4:
  %iftmp_9 = phi double [ %new_obj_5_cast, %then_2 ], [ %new_obj_7_cast, %else_3 ]
  %x_10 = alloca double
  store double %iftmp_9, double* %x_10
  %load_11 = load %struct.A*, %struct.A** %x_10
  %is_check_12 = icmp eq i32 1, 1
  br i1 %is_check_12, label %then_13, label %else_14

then_13:
  %load_16 = load %struct.A*, %struct.A** %x_10
  %as_cast_17 = call i8* @hulk_downcast(%load_16, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.type_name_18, i32 0, i32 0))
  %y_19 = alloca %struct.B*
  store %struct.B* %as_cast_17, %struct.B** %y_19
  %call_20 = call double @print_string(i8* @.str.11514100687449187254)
  br label %ifcont_15

else_14:
  %call_21 = call double @print_string(i8* @.str.13097573105356038884)
  br label %ifcont_15

ifcont_15:
  %iftmp_22 = phi double [ %call_20, %then_13 ], [ %call_21, %else_14 ]
  ret i32 0
}

