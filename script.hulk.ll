; ModuleID = 'hulk_module'
source_filename = "hulk_module"

%BoxedValue = type { i32, [8 x i8] }
%struct.Point = type { ptr, ptr, ptr }
%struct.PolarPoint = type { ptr, ptr, ptr }
%struct.Animal = type { ptr, ptr, ptr }
%struct.Dog = type { ptr, ptr, ptr }
%struct.Chihuahua = type { ptr, ptr, ptr }

@.str.0 = private constant [6 x i8] c"true\0A\00"
@.str.1 = private constant [7 x i8] c"false\0A\00"
@.str.2 = private constant [4 x i8] c"%f\0A\00"
@.str.3 = private constant [4 x i8] c"%s\0A\00"
@PI = constant double 0x400921FB54442EEA, align 8
@E = constant double 0x4005BF0A8B145FCF, align 8
@Point_vtable = constant [5 x ptr] [ptr @Point_getX, ptr @Point_getY, ptr @Point_setX, ptr @Point_setY, ptr @Point_norm]
@PolarPoint_vtable = constant [5 x ptr] [ptr @Point_getX, ptr @Point_getY, ptr @Point_setX, ptr @Point_setY, ptr @Point_norm]
@.str.4 = private constant [23 x i8] c"\22regular Animal noise\22\00"
@Animal_vtable = constant [3 x ptr] [ptr @Animal_speak, ptr @Animal_getName, ptr @Animal_getAge]
@.str.5 = private constant [20 x i8] c"\22regular Dog noise\22\00"
@.str.6 = private constant [14 x i8] c"\22Woof, Woof!\22\00"
@Dog_vtable = constant [4 x ptr] [ptr @Dog_speak, ptr @Animal_getName, ptr @Animal_getAge, ptr @Dog_bark]
@.str.7 = private constant [14 x i8] c"\22Wiif, Wiif!\22\00"
@Chihuahua_vtable = constant [4 x ptr] [ptr @Animal_speak, ptr @Animal_getName, ptr @Animal_getAge, ptr @Chihuahua_bark]
@.str.8 = private constant [9 x i8] c"\22Rookie\22\00"

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare double @pow(double, double)

declare double @sin(double)

declare double @cos(double)

declare double @sqrt(double)

declare double @exp(double)

declare double @log(double)

declare i32 @rand()

declare void @srand(i32)

declare i64 @time(ptr)

declare i32 @sprintf(ptr, ptr, ...)

declare ptr @strcpy(ptr, ptr)

declare ptr @strcat(ptr, ptr)

define internal void @print_boxed(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  switch i32 %2, label %print_double [
    i32 0, label %print_bool
    i32 1, label %print_double
    i32 2, label %print_string
  ]

print_double:                                     ; preds = %entry, %entry
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %4 = load double, ptr %3, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @.str.2, double %4)
  br label %merge

print_string:                                     ; preds = %entry
  %6 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %7 = load ptr, ptr %6, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @.str.3, ptr %7)
  br label %merge

print_bool:                                       ; preds = %entry
  %9 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %10 = load i1, ptr %9, align 1
  %11 = select i1 %10, ptr @.str.0, ptr @.str.1
  %12 = call i32 (ptr, ...) @printf(ptr %11)
  br label %merge

merge:                                            ; preds = %print_string, %print_double, %print_bool
  ret void
}

define internal ptr @unbox(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  switch i32 %2, label %unbox_bool [
    i32 0, label %unbox_bool
    i32 1, label %unbox_double
    i32 2, label %unbox_string
  ]

unbox_bool:                                       ; preds = %entry, %entry
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %4 = load i1, ptr %3, align 1
  %5 = inttoptr i1 %4 to ptr
  ret ptr %5

unbox_double:                                     ; preds = %entry
  %6 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %7 = load double, ptr %6, align 8
  %8 = bitcast double %7 to i64
  %9 = inttoptr i64 %8 to ptr
  ret ptr %9

unbox_string:                                     ; preds = %entry
  %10 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %11 = load ptr, ptr %10, align 8
  ret ptr %11
}

define internal i1 @isInt(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 1
  ret i1 %3
}

define internal i1 @isDouble(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 1
  ret i1 %3
}

define internal i1 @isBool(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 0
  ret i1 %3
}

define internal i1 @isString(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 2
  ret i1 %3
}

define internal i32 @unboxInt(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %2 = load double, ptr %1, align 8
  %3 = fptosi double %2 to i32
  ret i32 %3
}

define internal double @unboxDouble(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %2 = load double, ptr %1, align 8
  ret double %2
}

define internal i1 @unboxBool(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %2 = load i1, ptr %1, align 1
  ret i1 %2
}

define internal ptr @unboxString(ptr %0) {
entry:
  %1 = getelementptr inbounds nuw %BoxedValue, ptr %0, i32 0, i32 1
  %2 = load ptr, ptr %1, align 8
  ret ptr %2
}

define i32 @main() {
entry:
  %0 = call i64 @time(ptr null)
  %1 = trunc i64 %0 to i32
  call void @srand(i32 %1)
  %a = alloca double, align 8
  store double 6.000000e+00, ptr %a, align 8
  %var_a = load double, ptr %a, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %2, i32 0, i32 0
  store i32 2, ptr %3, align 4
  %4 = getelementptr inbounds nuw %BoxedValue, ptr %2, i32 0, i32 1
  store ptr @.str.8, ptr %4, align 8
  %5 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %6 = getelementptr inbounds nuw %BoxedValue, ptr %5, i32 0, i32 0
  store i32 1, ptr %6, align 4
  %7 = getelementptr inbounds nuw %BoxedValue, ptr %5, i32 0, i32 1
  store double 4.000000e+00, ptr %7, align 8
  %Dog_obj = call ptr @Dog_constructor(ptr %2, ptr %5)
  %d = alloca ptr, align 8
  store ptr %Dog_obj, ptr %d, align 8
  %Dog_obj_d = load ptr, ptr %d, align 8
  %8 = call ptr @f(ptr %Dog_obj_d)
  call void @print_boxed(ptr %8)
  ret i32 0
}

define ptr @Point_getX(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.Point, ptr %1, i32 0, i32 1
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define ptr @Point_getY(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.Point, ptr %1, i32 0, i32 2
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define ptr @Point_setX(ptr %0, ptr %1) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %x = alloca ptr, align 8
  store ptr %1, ptr %x, align 8
  %2 = load ptr, ptr %self, align 8
  %var_x = load ptr, ptr %x, align 8
  %3 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 1
  store ptr %var_x, ptr %3, align 8
  ret ptr %var_x
}

define ptr @Point_setY(ptr %0, ptr %1) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %y = alloca ptr, align 8
  store ptr %1, ptr %y, align 8
  %2 = load ptr, ptr %self, align 8
  %var_y = load ptr, ptr %y, align 8
  %3 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 2
  store ptr %var_y, ptr %3, align 8
  ret ptr %var_y
}

define ptr @Point_norm(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.Point, ptr %1, i32 0, i32 1
  %3 = load ptr, ptr %2, align 8
  %4 = call i1 @isInt(ptr %3)
  br i1 %4, label %int_case_pow_right, label %double_case_pow_right

int_case_pow_right:                               ; preds = %entry
  %5 = call i32 @unboxInt(ptr %3)
  %6 = sitofp i32 %5 to double
  %7 = call double @pow(double %6, double 2.000000e+00)
  br label %merge_pow_right

double_case_pow_right:                            ; preds = %entry
  %8 = call double @unboxDouble(ptr %3)
  %9 = call double @pow(double %8, double 2.000000e+00)
  br label %merge_pow_right

merge_pow_right:                                  ; preds = %double_case_pow_right, %int_case_pow_right
  %pow_result = phi double [ %7, %int_case_pow_right ], [ %9, %double_case_pow_right ]
  %10 = load ptr, ptr %self, align 8
  %11 = getelementptr inbounds nuw %struct.Point, ptr %10, i32 0, i32 2
  %12 = load ptr, ptr %11, align 8
  %13 = call i1 @isInt(ptr %12)
  br i1 %13, label %int_case_pow_right1, label %double_case_pow_right2

int_case_pow_right1:                              ; preds = %merge_pow_right
  %14 = call i32 @unboxInt(ptr %12)
  %15 = sitofp i32 %14 to double
  %16 = call double @pow(double %15, double 2.000000e+00)
  br label %merge_pow_right3

double_case_pow_right2:                           ; preds = %merge_pow_right
  %17 = call double @unboxDouble(ptr %12)
  %18 = call double @pow(double %17, double 2.000000e+00)
  br label %merge_pow_right3

merge_pow_right3:                                 ; preds = %double_case_pow_right2, %int_case_pow_right1
  %pow_result4 = phi double [ %16, %int_case_pow_right1 ], [ %18, %double_case_pow_right2 ]
  %19 = fadd double %pow_result, %pow_result4
  %20 = call double @pow(double %19, double 5.000000e-01)
  %21 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %22 = getelementptr inbounds nuw %BoxedValue, ptr %21, i32 0, i32 0
  store i32 1, ptr %22, align 4
  %23 = getelementptr inbounds nuw %BoxedValue, ptr %21, i32 0, i32 1
  store double %20, ptr %23, align 8
  ret ptr %21
}

define ptr @Point_constructor(ptr %0, ptr %1) {
entry:
  %x = alloca ptr, align 8
  store ptr %0, ptr %x, align 8
  %y = alloca ptr, align 8
  store ptr %1, ptr %y, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.Point, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 0
  store ptr @Point_vtable, ptr %3, align 8
  %var_x = load ptr, ptr %x, align 8
  %4 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 1
  store ptr %var_x, ptr %4, align 8
  %var_y = load ptr, ptr %y, align 8
  %5 = getelementptr inbounds nuw %struct.Point, ptr %2, i32 0, i32 2
  store ptr %var_y, ptr %5, align 8
  ret ptr %2
}

define ptr @PolarPoint_constructor(ptr %0, ptr %1) {
entry:
  %phi = alloca ptr, align 8
  store ptr %0, ptr %phi, align 8
  %rho = alloca ptr, align 8
  store ptr %1, ptr %rho, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.PolarPoint, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.PolarPoint, ptr %2, i32 0, i32 0
  store ptr @PolarPoint_vtable, ptr %3, align 8
  %var_rho = load ptr, ptr %rho, align 8
  %var_phi = load ptr, ptr %phi, align 8
  %4 = getelementptr inbounds nuw %BoxedValue, ptr %var_phi, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = getelementptr inbounds nuw %BoxedValue, ptr %var_phi, i32 0, i32 1
  %7 = load double, ptr %6, align 8
  %8 = call double @sin(double %7)
  %9 = call i1 @isInt(ptr %var_rho)
  br i1 %9, label %int_case_mul, label %double_case_mul

int_case_mul:                                     ; preds = %entry
  %10 = call i32 @unboxInt(ptr %var_rho)
  %11 = sitofp i32 %10 to double
  %12 = fmul double %8, %11
  br label %merge_mul

double_case_mul:                                  ; preds = %entry
  %13 = call double @unboxDouble(ptr %var_rho)
  %14 = fmul double %8, %13
  br label %merge_mul

merge_mul:                                        ; preds = %double_case_mul, %int_case_mul
  %mul_result = phi double [ %12, %int_case_mul ], [ %14, %double_case_mul ]
  %15 = getelementptr inbounds nuw %struct.PolarPoint, ptr %2, i32 0, i32 1
  %16 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %17 = getelementptr inbounds nuw %BoxedValue, ptr %16, i32 0, i32 0
  store i32 1, ptr %17, align 4
  %18 = getelementptr inbounds nuw %BoxedValue, ptr %16, i32 0, i32 1
  store double %mul_result, ptr %18, align 8
  store ptr %16, ptr %15, align 8
  %var_rho1 = load ptr, ptr %rho, align 8
  %var_phi2 = load ptr, ptr %phi, align 8
  %19 = getelementptr inbounds nuw %BoxedValue, ptr %var_phi2, i32 0, i32 0
  %20 = load i32, ptr %19, align 4
  %21 = getelementptr inbounds nuw %BoxedValue, ptr %var_phi2, i32 0, i32 1
  %22 = load double, ptr %21, align 8
  %23 = call double @cos(double %22)
  %24 = call i1 @isInt(ptr %var_rho1)
  br i1 %24, label %int_case_mul3, label %double_case_mul4

int_case_mul3:                                    ; preds = %merge_mul
  %25 = call i32 @unboxInt(ptr %var_rho1)
  %26 = sitofp i32 %25 to double
  %27 = fmul double %23, %26
  br label %merge_mul5

double_case_mul4:                                 ; preds = %merge_mul
  %28 = call double @unboxDouble(ptr %var_rho1)
  %29 = fmul double %23, %28
  br label %merge_mul5

merge_mul5:                                       ; preds = %double_case_mul4, %int_case_mul3
  %mul_result6 = phi double [ %27, %int_case_mul3 ], [ %29, %double_case_mul4 ]
  %30 = getelementptr inbounds nuw %struct.PolarPoint, ptr %2, i32 0, i32 2
  %31 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %32 = getelementptr inbounds nuw %BoxedValue, ptr %31, i32 0, i32 0
  store i32 1, ptr %32, align 4
  %33 = getelementptr inbounds nuw %BoxedValue, ptr %31, i32 0, i32 1
  store double %mul_result6, ptr %33, align 8
  store ptr %31, ptr %30, align 8
  ret ptr %2
}

define ptr @Animal_speak(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %2 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 0
  store i32 2, ptr %2, align 4
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 1
  store ptr @.str.4, ptr %3, align 8
  ret ptr %1
}

define ptr @Animal_getName(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  ret ptr null
}

define ptr @Animal_getAge(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  ret ptr null
}

define ptr @Animal_constructor(ptr %0, ptr %1) {
entry:
  %name = alloca ptr, align 8
  store ptr %0, ptr %name, align 8
  %age = alloca ptr, align 8
  store ptr %1, ptr %age, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.Animal, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.Animal, ptr %2, i32 0, i32 0
  store ptr @Animal_vtable, ptr %3, align 8
  %var_name = load ptr, ptr %name, align 8
  %4 = getelementptr inbounds nuw %struct.Animal, ptr %2, i32 0, i32 1
  store ptr %var_name, ptr %4, align 8
  %var_age = load ptr, ptr %age, align 8
  %5 = getelementptr inbounds nuw %struct.Animal, ptr %2, i32 0, i32 2
  store ptr %var_age, ptr %5, align 8
  ret ptr %2
}

define ptr @Dog_speak(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %2 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 0
  store i32 2, ptr %2, align 4
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 1
  store ptr @.str.5, ptr %3, align 8
  ret ptr %1
}

define ptr @Dog_bark(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %2 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 0
  store i32 2, ptr %2, align 4
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 1
  store ptr @.str.6, ptr %3, align 8
  ret ptr %1
}

define ptr @Dog_constructor(ptr %0, ptr %1) {
entry:
  %name = alloca ptr, align 8
  store ptr %0, ptr %name, align 8
  %age = alloca ptr, align 8
  store ptr %1, ptr %age, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.Dog, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.Dog, ptr %2, i32 0, i32 0
  store ptr @Dog_vtable, ptr %3, align 8
  %var_name = load ptr, ptr %name, align 8
  %4 = getelementptr inbounds nuw %struct.Dog, ptr %2, i32 0, i32 1
  store ptr %var_name, ptr %4, align 8
  %var_age = load ptr, ptr %age, align 8
  %5 = getelementptr inbounds nuw %struct.Dog, ptr %2, i32 0, i32 2
  store ptr %var_age, ptr %5, align 8
  ret ptr %2
}

define ptr @Chihuahua_bark(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %2 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 0
  store i32 2, ptr %2, align 4
  %3 = getelementptr inbounds nuw %BoxedValue, ptr %1, i32 0, i32 1
  store ptr @.str.7, ptr %3, align 8
  ret ptr %1
}

define ptr @Chihuahua_constructor(ptr %0, ptr %1) {
entry:
  %name = alloca ptr, align 8
  store ptr %0, ptr %name, align 8
  %age = alloca ptr, align 8
  store ptr %1, ptr %age, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.Chihuahua, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.Chihuahua, ptr %2, i32 0, i32 0
  store ptr @Chihuahua_vtable, ptr %3, align 8
  %var_name = load ptr, ptr %name, align 8
  %4 = getelementptr inbounds nuw %struct.Chihuahua, ptr %2, i32 0, i32 1
  store ptr %var_name, ptr %4, align 8
  %var_age = load ptr, ptr %age, align 8
  %5 = getelementptr inbounds nuw %struct.Chihuahua, ptr %2, i32 0, i32 2
  store ptr %var_age, ptr %5, align 8
  ret ptr %2
}

define ptr @f(ptr %0) {
entry:
  %x = alloca ptr, align 8
  store ptr %0, ptr %x, align 8
  %var_x = load ptr, ptr %x, align 8
  %vtable_ptr = getelementptr inbounds nuw %struct.Chihuahua, ptr %var_x, i32 0, i32 0
  %vtable = load ptr, ptr %vtable_ptr, align 8
  %method_ptr_ptr = getelementptr [4 x ptr], ptr %vtable, i32 0, i32 0
  %method_ptr = load ptr, ptr %method_ptr_ptr, align 8
  %dynamic_call_result = call ptr %method_ptr(ptr %var_x)
  ret ptr %dynamic_call_result
}
