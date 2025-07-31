#include <stdio.h>

void write_using_syscall(const char* msg, int len) {
    long result;
    
    asm volatile (
        "movq $1, %%rax\n\t"      // __NR_write = 1 (64-bit)
        "movq $1, %%rdi\n\t"      // stdout fd = 1
        "movq %1, %%rsi\n\t"      // buffer pointer
        "movq %2, %%rdx\n\t"      // count
        "syscall\n\t"             // modern syscall
        "movq %%rax, %0"          // store result
        : "=m"(result)
        : "m"(msg), "r"((long)len)
        : "rax", "rdi", "rsi", "rdx", "rcx", "r11"
    );
    
    printf("Write returned: %ld bytes\n", result);
}

int main() {
    char message[] = "Direct syscall write call\n";
    write_using_syscall(message, sizeof(message)-1);
    return 0;
}