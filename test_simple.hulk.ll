%struct.Bird = type { i8 }
%struct.Plane = type { i8 }
%struct.Superman = type { i8 }
@.str.11491927587467200873 = private unnamed_addr constant [11 x i8] c"It's bird!\00"
@.str.16495786210110340079 = private unnamed_addr constant [14 x i8] c"It's a plane!\00"
@.str.8675640368891811931 = private unnamed_addr constant [19 x i8] c"No, it's Superman!\00"
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
  %size_1 = ptrtoint %struct.Superman* null to i64
  %new_obj_0 = call i8* @malloc(i64 %size_1)
  %new_obj_0_cast = bitcast i8* %new_obj_0 to %struct.Superman*
  %x_2 = alloca %struct.Superman*
  store %struct.Superman* %new_obj_0_cast, %struct.Superman** %x_2
  %load_3 = load %struct.Superman*, %struct.Superman** %x_2
  %is_check_4 = icmp eq i32 1, 1
  br i1 %is_check_4, label %then_5, label %else_6

then_5:
  br label %ifcont_7

else_6:
  %load_8 = load %struct.Superman*, %struct.Superman** %x_2
  %is_check_9 = icmp eq i32 1, 1
  br i1 %is_check_9, label %then_10, label %else_11

then_10:
  br label %ifcont_12

else_11:
  br label %ifcont_12

ifcont_12:
  %iftmp_13 = phi double [ @.str.16495786210110340079, %then_10 ], [ @.str.8675640368891811931, %else_11 ]
  br label %ifcont_7

ifcont_7:
  %iftmp_14 = phi double [ @.str.11491927587467200873, %then_5 ], [ %iftmp_13, %else_6 ]
  %call_15 = call double @print_string(i8* %iftmp_14)
  ret i32 0
}

