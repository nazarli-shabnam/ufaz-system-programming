#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* Structure definition from exercise 1 */
struct example_struct
{
    char c;
    char *pc;
    int i;
};

/* Macro to calculate relative address of field c in structure s */
#define RELATIVE_ADDRESS_OF(s, c) ((size_t)((char *)&((s).c) - (char *)&(s)))

/* Alternative definition using offsetof (standard library) */
#define RELATIVE_ADDRESS_OF_OFFSETOF(s, c) offsetof(typeof(s), c)

int main(void)
{
    struct example_struct v;

    printf("Relative addresses of structure fields:\n");
    printf("======================================\n\n");

    printf("Structure variable v address: %p\n", (void *)&v);
    printf("\n");

    /* Display relative address of field c */
    size_t offset_c = RELATIVE_ADDRESS_OF(v, c);
    printf("Relative address of field 'c': %zu bytes\n", offset_c);
    printf("  Absolute address: %p\n", (void *)&v.c);
    printf("  Calculation: (char *)&v.c - (char *)&v = %zu\n", offset_c);

    /* Display relative address of field pc */
    size_t offset_pc = RELATIVE_ADDRESS_OF(v, pc);
    printf("\nRelative address of field 'pc': %zu bytes\n", offset_pc);
    printf("  Absolute address: %p\n", (void *)&v.pc);
    printf("  Calculation: (char *)&v.pc - (char *)&v = %zu\n", offset_pc);

    /* Display relative address of field i */
    size_t offset_i = RELATIVE_ADDRESS_OF(v, i);
    printf("\nRelative address of field 'i': %zu bytes\n", offset_i);
    printf("  Absolute address: %p\n", (void *)&v.i);
    printf("  Calculation: (char *)&v.i - (char *)&v = %zu\n", offset_i);

    printf("\nSummary:\n");
    printf("========\n");
    printf("Field 'c'  offset: %zu bytes\n", offset_c);
    printf("Field 'pc' offset: %zu bytes\n", offset_pc);
    printf("Field 'i'  offset: %zu bytes\n", offset_i);
    printf("Total structure size: %zu bytes\n", sizeof(struct example_struct));

    printf("\nNote: The macro RELATIVE_ADDRESS_OF(s, c) calculates the offset\n");
    printf("of field 'c' within structure 's' by subtracting the base address\n");
    printf("of the structure from the address of the field.\n");

    return 0;
}

