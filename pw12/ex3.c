#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Guard value to detect memory corruption */
#define GUARD_VALUE 0xDEADBEEF
#define GUARD_SIZE sizeof(uint32_t)

/* Structure to store allocation metadata */
typedef struct
{
    size_t size;
    uint32_t guard_before;
    uint32_t guard_after;
} alloc_header;

/* Custom malloc with memory corruption detection */
void *my_malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    /* Allocate memory for: guard + header + user data + guard */
    size_t total_size = GUARD_SIZE + sizeof(alloc_header) + size + GUARD_SIZE;
    void *ptr = malloc(total_size);

    if (ptr == NULL)
    {
        return NULL;
    }

    /* Set guard value before header */
    uint32_t *guard_before = (uint32_t *)ptr;
    *guard_before = GUARD_VALUE;

    /* Set header */
    alloc_header *header = (alloc_header *)((char *)ptr + GUARD_SIZE);
    header->size = size;
    header->guard_before = GUARD_VALUE;
    header->guard_after = GUARD_VALUE;

    /* Set guard value after user data */
    uint32_t *guard_after = (uint32_t *)((char *)ptr + GUARD_SIZE + sizeof(alloc_header) + size);
    *guard_after = GUARD_VALUE;

    /* Return pointer to user data area */
    return (char *)ptr + GUARD_SIZE + sizeof(alloc_header);
}

/* Custom free with memory corruption detection */
void my_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    /* Calculate pointer to allocation start */
    void *alloc_start = (char *)ptr - GUARD_SIZE - sizeof(alloc_header);

    /* Check guard before header */
    uint32_t *guard_before = (uint32_t *)alloc_start;
    if (*guard_before != GUARD_VALUE)
    {
        fprintf(stderr, "ERROR: Memory corruption detected! Guard before header is corrupted.\n");
        fprintf(stderr, "Expected: 0x%08X, Found: 0x%08X\n", GUARD_VALUE, *guard_before);
        abort();
    }

    /* Get header */
    alloc_header *header = (alloc_header *)((char *)alloc_start + GUARD_SIZE);

    /* Check guards in header */
    if (header->guard_before != GUARD_VALUE)
    {
        fprintf(stderr, "ERROR: Memory corruption detected! Header guard_before is corrupted.\n");
        abort();
    }

    if (header->guard_after != GUARD_VALUE)
    {
        fprintf(stderr, "ERROR: Memory corruption detected! Header guard_after is corrupted.\n");
        abort();
    }

    /* Check guard after user data */
    uint32_t *guard_after = (uint32_t *)((char *)alloc_start + GUARD_SIZE + sizeof(alloc_header) + header->size);
    if (*guard_after != GUARD_VALUE)
    {
        fprintf(stderr, "ERROR: Memory corruption detected! Guard after user data is corrupted.\n");
        fprintf(stderr, "Expected: 0x%08X, Found: 0x%08X\n", GUARD_VALUE, *guard_after);
        fprintf(stderr, "Allocated size was: %zu bytes\n", header->size);
        abort();
    }

    /* All guards intact - free the memory */
    free(alloc_start);
}

/* Test program */
int main(void)
{
    printf("Testing my_malloc and my_free with corruption detection:\n");
    printf("=========================================================\n\n");

    /* Test 1: Normal allocation and deallocation */
    printf("Test 1: Normal allocation and deallocation\n");
    int *p1 = (int *)my_malloc(10 * sizeof(int));
    if (p1)
    {
        printf("Allocated 10 integers successfully.\n");
        for (int i = 0; i < 10; i++)
        {
            p1[i] = i;
        }
        printf("Filled array with values.\n");
        my_free(p1);
        printf("Freed memory successfully.\n");
    }
    printf("\n");

    /* Test 2: Memory corruption detection (commented out to avoid crash) */
    printf("Test 2: Memory corruption detection\n");
    printf("(Uncomment the code below to test corruption detection)\n");
    /*
    int *p2 = (int *)my_malloc(5 * sizeof(int));
    if (p2)
    {
        printf("Allocated 5 integers.\n");
        // Corrupt memory by writing beyond allocated area
        p2[10] = 999; // This should trigger corruption detection
        my_free(p2);  // This should detect the corruption
    }
    */
    printf("\n");

    printf("All tests completed.\n");
    return 0;
}

