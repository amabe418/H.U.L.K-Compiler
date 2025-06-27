%struct.G = type { i8 }
%struct.Holis = type { i8 }
%struct.A = type { i8 }
%struct.B = type { i8 }
%struct.C = type { i8 }
%struct.D = type { i8 }
define double @G_foo(%struct.G* %self) {
entry:
  ret double 5.000000
}

define double @A_foo(%struct.A* %self) {
entry:
  ret double 10.000000
}

define double @B_foo(%struct.B* %self) {
entry:
  ret double 20.000000
}

define double @C_foo(%struct.C* %self) {
entry:
  ret double 40.000000
}

define double @D_foo(%struct.D* %self) {
entry:
  ret double 50.000000
}

define %struct.A* @bar(double %x) {
entry:
  %param_0 = alloca double
  store double %x, double* %param_0
  %load_1 = load double, double* %param_0
  %binary_2 = fcmp ugt double %load_1, 5.000000
  br i1 %binary_2, label %then_3, label %else_4

then_3:
  %size_7 = ptrtoint %struct.B* null to i64
  %new_obj_6 = call i8* @malloc(i64 %size_7)
  %new_obj_6_cast = bitcast i8* %new_obj_6 to %struct.B*
  br label %ifcont_5

else_4:
  %load_8 = load double, double* %param_0
  %binary_9 = fcmp ugt double %load_8, 3.000000
  br i1 %binary_9, label %then_10, label %else_11

then_10:
  %size_14 = ptrtoint %struct.C* null to i64
  %new_obj_13 = call i8* @malloc(i64 %size_14)
  %new_obj_13_cast = bitcast i8* %new_obj_13 to %struct.C*
  br label %ifcont_12

else_11:
  %size_16 = ptrtoint %struct.D* null to i64
  %new_obj_15 = call i8* @malloc(i64 %size_16)
  %new_obj_15_cast = bitcast i8* %new_obj_15 to %struct.D*
  br label %ifcont_12

ifcont_12:
  %iftmp_17 = phi double [ %new_obj_13_cast, %then_10 ], [ %new_obj_15_cast, %else_11 ]
  br label %ifcont_5

ifcont_5:
  %iftmp_18 = phi double [ %new_obj_6_cast, %then_3 ], [ %iftmp_17, %else_4 ]
  ret %struct.A* %iftmp_18
}

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
  %call_19 = call %struct.A* @bar(double 5.000000)
  %method_call_20 = call double @A_foo(%struct.A* %call_19)
  %call_21 = call double @print_double(double %method_call_20)
  %call_22 = call %struct.A* @bar(double 9.000000)
  %method_call_23 = call double @A_foo(%struct.A* %call_22)
  %call_24 = call double @print_double(double %method_call_23)
  ret i32 0
}

