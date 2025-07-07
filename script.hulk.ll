; ModuleID = 'hulk_module'
source_filename = "hulk_module"

%BoxedValue = type { i32, [8 x i8] }

@.str.0 = private constant [6 x i8] c"true\0A\00"
@.str.1 = private constant [7 x i8] c"false\0A\00"
@.str.2 = private constant [4 x i8] c"%d\0A\00"
@.str.3 = private constant [4 x i8] c"%f\0A\00"
@.str.4 = private constant [4 x i8] c"%s\0A\00"
@.str.5 = private constant [5 x i8] c"true\00"
@.str.6 = private constant [4 x i8] c"dsa\00"

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare double @pow(double, double)

declare i32 @sprintf(ptr, ptr, ...)

declare ptr @strcpy(ptr, ptr)

declare ptr @strcat(ptr, ptr)

define internal void @print_boxed(ptr %0) {
entry:
  %1 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  switch i32 %2, label %print_int [
    i32 0, label %print_bool
    i32 1, label %print_int
    i32 2, label %print_double
    i32 3, label %print_string
  ]

print_int:                                        ; preds = %entry, %entry
  %3 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 1
  %4 = load i32, ptr %3, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @.str.2, i32 %4)
  br label %merge

print_double:                                     ; preds = %entry
  %6 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 1
  %7 = load double, ptr %6, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @.str.3, double %7)
  br label %merge

print_string:                                     ; preds = %entry
  %9 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 1
  %10 = load ptr, ptr %9, align 8
  %11 = call i32 (ptr, ...) @printf(ptr @.str.4, ptr %10)
  br label %merge

print_bool:                                       ; preds = %entry
  %12 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 1
  %13 = load i1, ptr %12, align 1
  %14 = select i1 %13, ptr @.str.0, ptr @.str.1
  %15 = call i32 (ptr, ...) @printf(ptr %14)
  br label %merge

merge:                                            ; preds = %print_string, %print_double, %print_int, %print_bool
  ret void
}

define internal i1 @isInt(ptr %0) {
entry:
  %1 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 1
  ret i1 %3
}

define internal i1 @isDouble(ptr %0) {
entry:
  %1 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 2
  ret i1 %3
}

define internal i1 @isBool(ptr %0) {
entry:
  %1 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 0
  ret i1 %3
}

define internal i1 @isString(ptr %0) {
entry:
  %1 = getelementptr inbounds %BoxedValue, ptr %0, i32 0, i32 0
  %2 = load i32, ptr %1, align 4
  %3 = icmp eq i32 %2, 3
  ret i1 %3
}

define i32 @main() {
entry:
  %a1 = alloca i1, align 1
  %a = alloca ptr, align 8
  br i1 false, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %0 = phi ptr [ @.str.5, %then ], [ @.str.6, %else ]
  store ptr %0, ptr %a, align 8
  %1 = load ptr, ptr %a, align 8
  call void @print_boxed(ptr %1)
  store i1 false, ptr %a1, align 1
  %2 = load i1, ptr %a1, align 1
  %print_boxed_val = alloca %BoxedValue, align 8
  %3 = getelementptr inbounds %BoxedValue, ptr %print_boxed_val, i32 0, i32 0
  store i32 0, ptr %3, align 4
  %4 = getelementptr inbounds %BoxedValue, ptr %print_boxed_val, i32 0, i32 1
  store i1 %2, ptr %4, align 1
  call void @print_boxed(ptr %print_boxed_val)
  ret i32 0
}
