#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
// gcc -o test_calc test_calc.c

#define __NR_calc 549

int main(int argc, const char *argv[]) {
    int a, b, op;

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <a> <b> <op>\n", argv[0]);
        fprintf(stderr, "op: 0 - ADD, 1 - SUB, 2 - MUL, 3 - DIV\n");
        return 1;
    }

    a = atoi(argv[1]);
    b = atoi(argv[2]);
    op = atoi(argv[3]);
    
    long result = syscall(__NR_calc, a, b, op);
    printf("Syscall returned: %ld\n", result);
    return 0;
}