/*
 * implementation of stdio-like functions using system calls.
 */

#include <stdio.h>    
#include <stdlib.h>   
#include <unistd.h>  
#include <fcntl.h>     
#include <string.h>    
#include <errno.h>    

#define MY_BUFSIZ 1024
#define MY_EOF    EOF

typedef struct {
    int fd;                
    char buffer[MY_BUFSIZ];
    int buf_pos;           
    int buf_end;           
    int mode;              
    int eof;               
} MYFILE;

MYFILE *my_open(const char *filename, const char *mode) {
    MYFILE *f = malloc(sizeof(MYFILE));
    if (f == NULL) {
        perror("malloc");
        return NULL;
    }

    if (strcmp(mode, "r") == 0) {
        f->fd = open(filename, O_RDONLY);
        f->mode = 0;
    } else if (strcmp(mode, "w") == 0) {
        f->fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        f->mode = 1;
    } else {
        fprintf(stderr, "Unsupported mode: %s\n", mode);
        free(f);
        return NULL;
    }

    if (f->fd < 0) {
        perror("open");
        free(f);
        return NULL;
    }

    f->buf_pos = 0;
    f->buf_end = 0;
    f->eof = 0;

    return f;
}

int my_getc(MYFILE *f) {
    if (f->mode != 0) {
        fprintf(stderr, "File not opened in read mode.\n");
        return MY_EOF;
    }

    if (f->buf_pos >= f->buf_end) {
        ssize_t n = read(f->fd, f->buffer, MY_BUFSIZ);
        if (n <= 0) {
            f->eof = 1;
            return MY_EOF;
        }
        f->buf_end = n;
        f->buf_pos = 0;
    }

    return (unsigned char)f->buffer[f->buf_pos++];
}

int my_putc(int c, MYFILE *f) {
    if (f->mode != 1) {
        fprintf(stderr, "File not opened in write mode.\n");
        return MY_EOF;
    }

    f->buffer[f->buf_pos++] = (unsigned char)c;

    if (f->buf_pos >= MY_BUFSIZ) {
        ssize_t n = write(f->fd, f->buffer, f->buf_pos);
        if (n < 0) {
            perror("write");
            return MY_EOF;
        }
        f->buf_pos = 0;
    }

    return c;
}

int my_close(MYFILE *f) {
    int status = 0;

    if (f->mode == 1 && f->buf_pos > 0) {
        ssize_t n = write(f->fd, f->buffer, f->buf_pos);
        if (n < 0) {
            perror("write during close");
            status = -1;
        }
    }

    if (close(f->fd) < 0) {
        perror("close");
        status = -1;
    }

    free(f);
    return status;
}

int main(void) {
    MYFILE *f1 = my_open("foo1", "r");
    MYFILE *f2 = my_open("foo2", "r");
    if (!f1 || !f2) {
        fprintf(stderr, "Error opening files.\n");
        exit(EXIT_FAILURE);
    }

    int c1, c2;

    while ((c1 = my_getc(f1)) != MY_EOF && (c2 = my_getc(f2)) != MY_EOF) {
        putchar(c1);
        putchar(c2);
    }

    my_close(f1);
    my_close(f2);

    return 0;
}
