#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    /* Open file */
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    /* Get file size */
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        perror("fstat");
        close(fd);
        return 1;
    }

    if (st.st_size == 0)
    {
        printf("File is empty.\n");
        close(fd);
        return 0;
    }

    /* Map file into memory */
    void *mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd); /* File descriptor no longer needed after mmap */

    char *file_data = (char *)mapped;
    size_t file_size = st.st_size;

    /* Find first line */
    printf("First line: ");
    size_t first_line_end = 0;
    while (first_line_end < file_size && file_data[first_line_end] != '\n')
    {
        putchar(file_data[first_line_end]);
        first_line_end++;
    }
    if (first_line_end < file_size)
    {
        putchar('\n');
    }
    else
    {
        printf("\n(File has only one line or no newline at end)\n");
    }

    /* Find last line */
    printf("\nLast line: ");
    size_t last_line_start = file_size;

    /* Find the start of the last line */
    if (file_size > 0 && file_data[file_size - 1] == '\n')
    {
        /* If file ends with newline, find the previous newline */
        last_line_start = file_size - 1;
        while (last_line_start > 0 && file_data[last_line_start - 1] != '\n')
        {
            last_line_start--;
        }
    }
    else
    {
        /* File doesn't end with newline, find the last newline */
        while (last_line_start > 0 && file_data[last_line_start - 1] != '\n')
        {
            last_line_start--;
        }
    }

    /* Print last line */
    for (size_t i = last_line_start; i < file_size && file_data[i] != '\n'; i++)
    {
        putchar(file_data[i]);
    }
    putchar('\n');

    /* Unmap memory */
    if (munmap(mapped, st.st_size) == -1)
    {
        perror("munmap");
        return 1;
    }

    return 0;
}

