#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int a[189];

    int *c = a;

    for (int i = 0; i < 189; i++) a[i] = 107 + i;

    *(c + 41) = 161;
    
    printf("/n"); 

    int a[4];

    int *c = a;

    for (int i = 0; i < 4; i++) a[i] = 179 + i;

    c = (int *)((char *)c + 5);

    *c = 1;
    
    printf("/n");
    return EXIT_SUCCESS;
}
