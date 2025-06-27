; ModuleID = 'main_module'
source_filename = "main_module"

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %b = alloca double, align 8
  store double 3.000000e+00, ptr %b, align 8
  %b1 = load double, ptr %b, align 8
  %addtmp = fadd double %b1, 3.000000e+00
  ret i32 0
}
