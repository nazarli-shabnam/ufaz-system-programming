#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[])
{
    char *path_env = getenv("PATH");

    if (!path_env)
    {
        fprintf(stderr, "PATH environment variable not set\n");
        return 1;
    }

    printf("PATH environment variable:\n%s\n\n", path_env);
    printf("Split into directories:\n");

    int count;
    char **dirs = split_path(path_env, &count);

    if (!dirs)
    {
        fprintf(stderr, "Failed to split PATH\n");
        return 1;
    }

    for (int i = 0; i < count; i++)
    {
        printf("%d: %s\n", i + 1, dirs[i]);
    }

    free_path_array(dirs, count);

    return 0;
}
