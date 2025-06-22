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
@.str.4433806826669766996 = private unnamed_addr constant [14 x i8] c"Hello, World!\00"
@.str.2257787302229234269 = private unnamed_addr constant [33 x i8] c"f(a,b), donde a y b son strings.\00"
@.str.2603192927274642682 = private unnamed_addr constant [2 x i8] c"a\00"
@.str.4947675599669400333 = private unnamed_addr constant [2 x i8] c"b\00"
@.str.16975543816661888491 = private unnamed_addr constant [7 x i8] c"f(2,3)\00"
@.str.9099277997342803831 = private unnamed_addr constant [14 x i8] c"f(true,false)\00"
@.str.10521035107279712938 = private unnamed_addr constant [56 x i8] c"Probando otra funcion recursiva, suma recursiva hasta n\00"
@.str.5169639892928245925 = private unnamed_addr constant [19 x i8] c"Probando for loops\00"
@.str.6482128353538638029 = private unnamed_addr constant [18 x i8] c"El valor de i es \00"
@.str.18086418558974010994 = private unnamed_addr constant [14 x i8] c"Moviendo i a \00"
@.str.10599700700371220778 = private unnamed_addr constant [2 x i8] c"
\00"
@.str.5849972387832478685 = private unnamed_addr constant [26 x i8] c"Probando concatenaciones:\00"
@.str.1724119774529258799 = private unnamed_addr constant [6 x i8] c"false\00"
@.str.11725737811754652148 = private unnamed_addr constant [6 x i8] c"Hola \00"
@.str.15464591742653079950 = private unnamed_addr constant [6 x i8] c"Mundo\00"
@.str.2169371982377735806 = private unnamed_addr constant [2 x i8] c"4\00"
@.str.12790462614085517521 = private unnamed_addr constant [7 x i8] c"Hola, \00"
@.str.15064472072739003433 = private unnamed_addr constant [7 x i8] c"Mundo!\00"
define double @successor(double) {
entry:
  %param_40 = alloca double
  store double %0, double* %param_40
  %load_41 = load double, double* %param_40
  %binary_42 = fadd double %load_41, 1.000000
  ret double %binary_42
}

define i8* @hw() {
entry:
  ret i8* @.str.7750308374451649530
}

define double @fib(double) {
entry:
  %param_51 = alloca double
  store double %0, double* %param_51
  %load_52 = load double, double* %param_51
  %binary_53 = fcmp ult double %load_52, 2.000000
  br i1 %binary_53, label %then_54, label %else_55

then_54:
  br label %ifcont_56

else_55:
  %load_57 = load double, double* %param_51
  %binary_58 = fsub double %load_57, 1.000000
  %call_59 = call double @fib(double %binary_58)
  %load_60 = load double, double* %param_51
  %binary_61 = fsub double %load_60, 2.000000
  %call_62 = call double @fib(double %binary_61)
  %binary_63 = fadd double %call_59, %call_62
  br label %ifcont_56

ifcont_56:
  %iftmp_64 = phi double [ 1.000000, %then_54 ], [ %binary_63, %else_55 ]
  ret double %iftmp_64
}

define i8* @f(i8*, i8*) {
entry:
  %param_69 = alloca i8*
  store i8* %0, i8** %param_69
  %param_70 = alloca i8*
  store i8* %1, i8** %param_70
  ret i8* @.str.4433806826669766996
}

define double @sum_to(double) {
entry:
  %param_81 = alloca double
  store double %0, double* %param_81
  %load_82 = load double, double* %param_81
  %binary_83 = fcmp ueq double %load_82, 1.000000
  br i1 %binary_83, label %then_84, label %else_85

then_84:
  br label %ifcont_86

else_85:
  %load_87 = load double, double* %param_81
  %load_88 = load double, double* %param_81
  %binary_89 = fsub double %load_88, 1.000000
  %call_90 = call double @sum_to(double %binary_89)
  %binary_91 = fadd double %load_87, %call_90
  br label %ifcont_86

ifcont_86:
  %iftmp_92 = phi double [ 0.000000, %then_84 ], [ %binary_91, %else_85 ]
  ret double %iftmp_92
}

define i8* @concat(i8*, i8*) {
entry:
  %param_131 = alloca i8*
  store i8* %0, i8** %param_131
  %param_132 = alloca i8*
  store i8* %1, i8** %param_132
  %load_133 = load i8*, i8** %param_131
  %load_134 = load i8*, i8** %param_132
  %binary_135 = call i8* @concat_strings(i8* %load_133, i8* %load_134)
  ret i8* %binary_135
}

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
  %call_0 = call double @print_string(i8* @.str.15911808872533315919)
  %a_1 = alloca double
  store double 2.000000, double* %a_1
  %b_2 = alloca double
  store double 3.000000, double* %b_2
  %c_3 = alloca double
  store double 4.000000, double* %c_3
  %d_4 = alloca double
  store double 5.000000, double* %d_4
  %call_5 = call double @print_string(i8* @.str.815733333669765891)
  %load_6 = load double, double* %a_1
  %load_7 = load double, double* %b_2
  %binary_8 = fadd double %load_6, %load_7
  %load_9 = load double, double* %c_3
  %binary_10 = fadd double %binary_8, %load_9
  %load_11 = load double, double* %d_4
  %binary_12 = fadd double %binary_10, %load_11
  %call_13 = call double @print_double(double %binary_12)
  %bool_conv_15 = fcmp one double 1.0, 0.0
  %call_14 = call double @print_bool(i1 %bool_conv_15)
  %bool_conv_17 = fcmp one double 0.0, 0.0
  %call_16 = call double @print_bool(i1 %bool_conv_17)
  %call_18 = call double @print_string(i8* @.str.1145924097123205603)
  %binary_19 = fcmp ult double 3.000000, 4.000000
  br i1 %binary_19, label %then_20, label %else_21

then_20:
  br label %ifcont_22

else_21:
  br label %ifcont_22

ifcont_22:
  %iftmp_23 = phi double [ 5.000000, %then_20 ], [ 10.000000, %else_21 ]
  %a_24 = alloca double
  store double %iftmp_23, double* %a_24
  %load_25 = load double, double* %a_24
  %call_26 = call double @print_double(double %load_25)
  %call_27 = call double @print_string(i8* @.str.1897382925244004874)
  %a_28 = alloca double
  store double 5.000000, double* %a_28
  br label %while_header_29

while_header_29:
  %load_32 = load double, double* %a_28
  %binary_33 = fcmp ugt double %load_32, 0.000000
  br i1 %binary_33, label %while_body_30, label %while_exit_31

while_body_30:
  %load_34 = load double, double* %a_28
  %call_35 = call double @print_double(double %load_34)
  %load_36 = load double, double* %a_28
  %binary_37 = fsub double %load_36, 1.000000
  store double %binary_37, double* %a_28
  br label %while_header_29

while_exit_31:
  %call_38 = call double @print_string(i8* @.str.9354905337115868815)
  %call_39 = call double @print_string(i8* @.str.15548335941765476253)
  %call_43 = call double @successor(double 0.000000)
  %call_44 = call double @print_double(double %call_43)
  %call_45 = call double @print_string(i8* @.str.12115510654860052758)
  %call_46 = call i8* @hw()
  %call_47 = call double @print_string(i8* %call_46)
  %call_48 = call double @print_string(i8* @.str.5985627218495613532)
  %call_49 = call double @print_string(i8* @.str.10139107239403537967)
  %call_50 = call double @print_string(i8* @.str.16777526998423184521)
  %call_65 = call double @fib(double 5.000000)
  %call_66 = call double @print_double(double %call_65)
  %call_67 = call double @print_string(i8* @.str.1282047880766431650)
  %call_68 = call double @print_string(i8* @.str.17241328489943237090)
  %call_71 = call double @print_string(i8* @.str.2257787302229234269)
  %call_72 = call i8* @f(i8* @.str.2603192927274642682, i8* @.str.4947675599669400333)
  %call_73 = call double @print_string(i8* %call_72)
  %call_74 = call double @print_string(i8* @.str.16975543816661888491)
  %call_75 = call i8* @f(double 2.000000, double 3.000000)
  %call_76 = call double @print_string(i8* %call_75)
  %call_77 = call double @print_string(i8* @.str.9099277997342803831)
  %call_78 = call i8* @f(double 1.0, double 0.0)
  %call_79 = call double @print_string(i8* %call_78)
  %call_80 = call double @print_string(i8* @.str.10521035107279712938)
  %call_93 = call double @sum_to(double 100.000000)
  %call_94 = call double @print_double(double %call_93)
  %call_95 = call double @print_string(i8* @.str.5169639892928245925)
  %i_99 = alloca double
  %start_int_100 = fptosi double 1.000000 to i32
  %end_int_101 = fptosi double 7.000000 to i32
  %counter_102 = alloca i32
  store i32 %start_int_100, i32* %counter_102
  br label %for_header_96

for_header_96:
  %counter_load_103 = load i32, i32* %counter_102
  %continue_cond_104 = icmp slt i32 %counter_load_103, %end_int_101
  br i1 %continue_cond_104, label %for_body_97, label %for_exit_98

for_body_97:
  %counter_double_105 = sitofp i32 %counter_load_103 to double
  store double %counter_double_105, double* %i_99
  %load_106 = load double, double* %i_99
  %right_conv_108 = call i8* @double_to_string(double %load_106)
  %binary_107 = call i8* @concat_strings(i8* @.str.6482128353538638029, i8* %right_conv_108)
  %call_109 = call double @print_string(i8* %binary_107)
  %load_110 = load double, double* %i_99
  %binary_111 = fadd double %load_110, 1.000000
  %right_conv_113 = call i8* @double_to_string(double %binary_111)
  %binary_112 = call i8* @concat_strings(i8* @.str.18086418558974010994, i8* %right_conv_113)
  %call_114 = call double @print_string(i8* %binary_112)
  %counter_inc_115 = add i32 %counter_load_103, 1
  store i32 %counter_inc_115, i32* %counter_102
  br label %for_header_96

for_exit_98:
  %call_116 = call double @print_string(i8* @.str.10599700700371220778)
  %call_117 = call double @print_string(i8* @.str.5849972387832478685)
  %bool_conv_120 = fcmp one double 1.0, 0.0
  %left_conv_119 = call i8* @bool_to_string(i1 %bool_conv_120)
  %binary_118 = call i8* @concat_strings(i8* %left_conv_119, i8* @.str.1724119774529258799)
  %call_121 = call double @print_string(i8* %binary_118)
  %bool_conv_124 = fcmp one double 1.0, 0.0
  %right_conv_123 = call i8* @bool_to_string(i1 %bool_conv_124)
  %binary_122 = call i8* @concat_strings(i8* @.str.1724119774529258799, i8* %right_conv_123)
  %call_125 = call double @print_string(i8* %binary_122)
  %binary_126 = call i8* @concat_strings(i8* @.str.11725737811754652148, i8* @.str.15464591742653079950)
  %call_127 = call double @print_string(i8* %binary_126)
  %right_conv_129 = call i8* @double_to_string(double 2.000000)
  %binary_128 = call i8* @concat_strings(i8* @.str.2169371982377735806, i8* %right_conv_129)
  %call_130 = call double @print_string(i8* %binary_128)
  %call_136 = call i8* @concat(i8* @.str.12790462614085517521, i8* @.str.15064472072739003433)
  %call_137 = call double @print_string(i8* %call_136)
  ret i32 0
}

