/* ex1.c
 * Copies the file "foo" into a new file "bar".
 * Uses only fopen, getc, putc and fclose as requested.
 */

#include <stdio.h>

int main(void) {
    const char *src_name = "foo";
    const char *dst_name = "bar";

    FILE *src  = fopen(src_name, "rb");  /* reading in binary */
    if (src == NULL) {
        perror("Error opening source file");
        return 1;
    }

    FILE *dst = fopen(dst_name, "wb");   /* / truncate destination     */
    if (dst == NULL) {
        perror("Error opening destination file");
        fclose(src);
        return 1;
    }

    int ch;
    while ((ch = getc(src)) != EOF) {
        if (putc(ch, dst) == EOF) {
            perror("Error writing to destination file");
            fclose(src);
            fclose(dst);
            return 1;
        }
    }

    fclose(src);
    fclose(dst);

    return 0;
}
