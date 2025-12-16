#include <stdio.h>
#include <stdlib.h>

/* Structure definition */
struct example_struct
{
    char c;
    char *pc;
    int i;
};

int main(void)
{
    printf("Size of various types:\n");
    printf("======================\n\n");

    printf("Size of int: %zu bytes\n", sizeof(int));
    printf("Size of char: %zu bytes\n", sizeof(char));
    printf("Size of char *: %zu bytes\n", sizeof(char *));
    printf("Size of struct { char c; char *pc; int i; }: %zu bytes\n",
           sizeof(struct example_struct));

    printf("\nDetailed structure layout:\n");
    printf("==========================\n");
    struct example_struct s;
    printf("Address of structure s: %p\n", (void *)&s);
    printf("Address of field c: %p (offset: %zu bytes)\n",
           (void *)&s.c, (size_t)((char *)&s.c - (char *)&s));
    printf("Address of field pc: %p (offset: %zu bytes)\n",
           (void *)&s.pc, (size_t)((char *)&s.pc - (char *)&s));
    printf("Address of field i: %p (offset: %zu bytes)\n",
           (void *)&s.i, (size_t)((char *)&s.i - (char *)&s));

    printf("\nExplanation:\n");
    printf("============\n");
    printf("The size of the structure (%zu bytes) is larger than the sum of\n", sizeof(struct example_struct));
    printf("its members (%zu + %zu + %zu = %zu bytes) due to structure padding.\n",
           sizeof(char), sizeof(char *), sizeof(int),
           sizeof(char) + sizeof(char *) + sizeof(int));
    printf("\nStructure padding occurs because:\n");
    printf("1. The compiler aligns structure members to memory addresses that are\n");
    printf("   multiples of their size for better performance.\n");
    printf("2. The 'c' field (1 byte) is followed by padding to align 'pc' (pointer)\n");
    printf("   to its natural alignment boundary.\n");
    printf("3. The 'i' field (int) is also aligned to its natural boundary.\n");
    printf("4. The total structure size is rounded up to a multiple of the largest\n");
    printf("   member's alignment requirement.\n");

    return 0;
}

