.global _start
.type _start,@function

_start:
   mov %rsp, %rbp
   mov 0(%rbp), %rdi
   lea 8(%rbp), %rsi
   call _wrap_main
   mov %rax, %rdi
   mov $60, %rax
   syscall