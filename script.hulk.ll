; ModuleID = 'hulk_module'
source_filename = "hulk_module"

%BoxedValue = type { i32, [8 x i8] }
%struct.A = type { ptr, ptr, ptr }
%struct.B = type { ptr, ptr, ptr }

@.str.0 = private constant [6 x i8] c"true\0A\00"
@.str.1 = private constant [7 x i8] c"false\0A\00"
@.str.2 = private constant [4 x i8] c"%f\0A\00"
@.str.3 = private constant [4 x i8] c"%s\0A\00"
@PI = constant double 0x400921FB54442EEA, align 8
@E = constant double 0x4005BF0A8B145FCF, align 8
@A_vtable = constant [2 x ptr] [ptr @A_getX, ptr @A_getY]
@B_vtable = constant [2 x ptr] [ptr @B_getA, ptr @B_getB]

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
  store i32 1, ptr %3, align 4
  %4 = getelementptr inbounds nuw %BoxedValue, ptr %2, i32 0, i32 1
  store double 1.000000e+00, ptr %4, align 8
  %5 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %6 = getelementptr inbounds nuw %BoxedValue, ptr %5, i32 0, i32 0
  store i32 1, ptr %6, align 4
  %7 = getelementptr inbounds nuw %BoxedValue, ptr %5, i32 0, i32 1
  store double 2.000000e+00, ptr %7, align 8
  %A_obj = call ptr @A_constructor(ptr %2, ptr %5)
  %a1 = alloca ptr, align 8
  store ptr %A_obj, ptr %a1, align 8
  %A_obj_a = load ptr, ptr %a1, align 8
  %8 = call ptr @A_getX(ptr %A_obj_a)
  call void @print_boxed(ptr %8)
  %A_obj_a2 = load ptr, ptr %a1, align 8
  %9 = call ptr @A_getY(ptr %A_obj_a2)
  call void @print_boxed(ptr %9)
  %10 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %11 = getelementptr inbounds nuw %BoxedValue, ptr %10, i32 0, i32 0
  store i32 1, ptr %11, align 4
  %12 = getelementptr inbounds nuw %BoxedValue, ptr %10, i32 0, i32 1
  store double 3.000000e+00, ptr %12, align 8
  %13 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%BoxedValue, ptr null, i32 1) to i64))
  %14 = getelementptr inbounds nuw %BoxedValue, ptr %13, i32 0, i32 0
  store i32 1, ptr %14, align 4
  %15 = getelementptr inbounds nuw %BoxedValue, ptr %13, i32 0, i32 1
  store double 4.000000e+00, ptr %15, align 8
  %B_obj = call ptr @B_constructor(ptr %10, ptr %13)
  %b = alloca ptr, align 8
  store ptr %B_obj, ptr %b, align 8
  %B_obj_b = load ptr, ptr %b, align 8
  %16 = call ptr @B_getA(ptr %B_obj_b)
  call void @print_boxed(ptr %16)
  %B_obj_b3 = load ptr, ptr %b, align 8
  %17 = call ptr @B_getB(ptr %B_obj_b3)
  call void @print_boxed(ptr %17)
  ret i32 0
}

define ptr @A_getX(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.A, ptr %1, i32 0, i32 1
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define ptr @A_getY(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.A, ptr %1, i32 0, i32 2
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define ptr @A_constructor(ptr %0, ptr %1) {
entry:
  %x = alloca ptr, align 8
  store ptr %0, ptr %x, align 8
  %y = alloca ptr, align 8
  store ptr %1, ptr %y, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.A, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.A, ptr %2, i32 0, i32 0
  store ptr @A_vtable, ptr %3, align 8
  %var_x = load ptr, ptr %x, align 8
  %4 = getelementptr inbounds nuw %struct.A, ptr %2, i32 0, i32 1
  store ptr %var_x, ptr %4, align 8
  %var_y = load ptr, ptr %y, align 8
  %5 = getelementptr inbounds nuw %struct.A, ptr %2, i32 0, i32 2
  store ptr %var_y, ptr %5, align 8
  ret ptr %2
}

define ptr @B_getA(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.B, ptr %1, i32 0, i32 1
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define ptr @B_getB(ptr %0) {
entry:
  %self = alloca ptr, align 8
  store ptr %0, ptr %self, align 8
  %1 = load ptr, ptr %self, align 8
  %2 = getelementptr inbounds nuw %struct.B, ptr %1, i32 0, i32 2
  %3 = load ptr, ptr %2, align 8
  ret ptr %3
}

define ptr @B_constructor(ptr %0, ptr %1) {
entry:
  %a = alloca ptr, align 8
  store ptr %0, ptr %a, align 8
  %b = alloca ptr, align 8
  store ptr %1, ptr %b, align 8
  %2 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%struct.B, ptr null, i32 1) to i64))
  %3 = getelementptr inbounds nuw %struct.B, ptr %2, i32 0, i32 0
  store ptr @B_vtable, ptr %3, align 8
  %var_a = load ptr, ptr %a, align 8
  %4 = getelementptr inbounds nuw %struct.B, ptr %2, i32 0, i32 1
  store ptr %var_a, ptr %4, align 8
  %var_b = load ptr, ptr %b, align 8
  %5 = getelementptr inbounds nuw %struct.B, ptr %2, i32 0, i32 2
  store ptr %var_b, ptr %5, align 8
  ret ptr %2
}
