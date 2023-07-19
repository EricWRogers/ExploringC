#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>

#include "arena.h"

int main(int argc, char* argv[]) {
    // array example
    int wholeNumbers[5];

    wholeNumbers[0] = 0;

    printf("%d\n", wholeNumbers[0]);

    // arena 
    Arena arena;
    arena = arena_init(1024*8);

    double* number = arena_alloc(&arena, sizeof(double));
    *number = 10.0;
    double* number1 = arena_alloc(&arena, sizeof(double));
    *number1 = 10.0;
    double* number2 = arena_alloc(&arena, sizeof(double));
    *number2 = 10.0;
    
    printf("number %lf\n", *number);

    arena_dealloc(&arena, number);
    number = NULL;
    arena_dealloc(&arena, number2);
    number2 = NULL;
    arena_dealloc(&arena, number1);
    number1 = NULL;

    arena_free(&arena);
    return 0;
}
