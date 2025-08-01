#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
// gcc -o reverse_string reverse_string.c

#define __NR_reverse_string 550

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }
    
    long result = syscall(__NR_reverse_string, argv[1], strlen(argv[1]));
    printf("Syscall returned: %ld\n", result);
    printf("Reversed string: %s\n", argv[1]);
    return 0;
}