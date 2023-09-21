.global start

start:
   mov %rsp, %rbp
   mov 0(%rbp), %rdi
   lea 8(%rbp), %rsi
   call __wrap_main
   mov %rax, %rdi
   mov $0x2000001, %rax
   syscall