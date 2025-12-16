#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

char **split_path(const char *path_str, int *count)
{
    if (!path_str || !count)
    {
        return NULL;
    }

    *count = 0;

    const char *p = path_str;
    while (*p)
    {
        if (*p == ':')
        {
            (*count)++;
        }
        p++;
    }
    (*count)++;

    char **dirs = (char **)malloc(*count * sizeof(char *));
    if (!dirs)
    {
        return NULL;
    }

    int idx = 0;
    const char *start = path_str;
    p = path_str;

    while (*p)
    {
        if (*p == ':')
        {
            int len = p - start;
            dirs[idx] = (char *)malloc((len + 1) * sizeof(char));
            if (!dirs[idx])
            {
                for (int i = 0; i < idx; i++)
                {
                    free(dirs[i]);
                }
                free(dirs);
                return NULL;
            }
            strncpy(dirs[idx], start, len);
            dirs[idx][len] = '\0';
            idx++;
            start = p + 1;
        }
        p++;
    }

    int len = p - start;
    dirs[idx] = (char *)malloc((len + 1) * sizeof(char));
    if (!dirs[idx])
    {
        for (int i = 0; i < idx; i++)
        {
            free(dirs[i]);
        }
        free(dirs);
        return NULL;
    }
    strncpy(dirs[idx], start, len);
    dirs[idx][len] = '\0';

    return dirs;
}

void free_path_array(char **dirs, int count)
{
    if (dirs)
    {
        for (int i = 0; i < count; i++)
        {
            free(dirs[i]);
        }
        free(dirs);
    }
}

int is_executable(const char *path)
{
    struct stat st;
    if (stat(path, &st) == -1)
    {
        return 0;
    }
    return S_ISREG(st.st_mode) && ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH));
}

char *find_command(const char *cmd)
{
    char *path_env = getenv("PATH");

    if (!path_env)
    {
        return NULL;
    }

    int count;
    char **dirs = split_path(path_env, &count);

    if (!dirs)
    {
        return NULL;
    }

    char *result = NULL;
    char full_path[1024];

    for (int i = 0; i < count; i++)
    {
        if (dirs[i][0] == '\0')
        {
            continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", dirs[i], cmd);
        if (is_executable(full_path))
        {
            result = strdup(full_path);
            break;
        }
    }

    free_path_array(dirs, count);

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1;
    }

    char *path = find_command(argv[1]);

    if (path)
    {
        printf("%s\n", path);
        free(path);
        return 0;
    }
    else
    {
        fprintf(stderr, "%s: command not found\n", argv[1]);
        return 1;
    }
}
