#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

char *my_ttyname(int fd) {
    static char result[PATH_MAX];
    struct stat fd_stat;
    struct stat file_stat;
    DIR *dir;
    struct dirent *entry;
    
    if (fstat(fd, &fd_stat) == -1) {
        return NULL;
    }
    
    if (!S_ISCHR(fd_stat.st_mode)) {
        return NULL;
    }
    
    dir = opendir("/dev/pts");
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') {
                continue;
            }
            
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "/dev/pts/%s", entry->d_name);
            
            if (stat(path, &file_stat) == 0) {
                if (file_stat.st_dev == fd_stat.st_dev && 
                    file_stat.st_ino == fd_stat.st_ino) {
                    strncpy(result, path, PATH_MAX - 1);
                    result[PATH_MAX - 1] = '\0';
                    closedir(dir);
                    return result;
                }
            }
        }
        closedir(dir);
    }
    
    dir = opendir("/dev");
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') {
                continue;
            }
            
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "/dev/%s", entry->d_name);
            
            if (stat(path, &file_stat) == 0) {
                if (file_stat.st_dev == fd_stat.st_dev && 
                    file_stat.st_ino == fd_stat.st_ino) {
                    strncpy(result, path, PATH_MAX - 1);
                    result[PATH_MAX - 1] = '\0';
                    closedir(dir);
                    return result;
                }
            }
        }
        closedir(dir);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    int fd;
    
    if (argc == 2) {
        fd = atoi(argv[1]);
    } else {
        fd = 0;
    }
    
    char *tty = my_ttyname(fd);
    
    if (tty) {
        printf("File descriptor %d is associated with: %s\n", fd, tty);
        return 0;
    } else {
        printf("File descriptor %d is not associated with a terminal\n", fd);
        return 1;
    }
}

