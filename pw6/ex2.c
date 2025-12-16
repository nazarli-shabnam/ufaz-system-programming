#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

char **split_cdpath(const char *cdpath_str, int *count)
{
    if (!cdpath_str || !count)
    {
        return NULL;
    }

    *count = 0;

    const char *p = cdpath_str;
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
    const char *start = cdpath_str;
    p = cdpath_str;

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

void free_cdpath_array(char **dirs, int count)
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

int directory_exists(const char *path)
{
    struct stat st;
    if (stat(path, &st) == -1)
    {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

char *find_in_cdpath(const char *dir_name)
{
    char *cdpath_env = getenv("CDPATH");

    if (!cdpath_env)
    {
        return NULL;
    }

    int count;
    char **dirs = split_cdpath(cdpath_env, &count);

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
        snprintf(full_path, sizeof(full_path), "%s/%s", dirs[i], dir_name);
        if (directory_exists(full_path))
        {
            result = strdup(full_path);
            break;
        }
    }

    free_cdpath_array(dirs, count);

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        fprintf(stderr, "\nNote: This command cannot work as a standalone program\n");
        fprintf(stderr, "because changing directory in a child process does not affect\n");
        fprintf(stderr, "the parent shell. The 'cd' command must be implemented as a\n");
        fprintf(stderr, "shell builtin to change the current working directory of the\n");
        fprintf(stderr, "shell process itself.\n");
        return 1;
    }

    char *target_dir = NULL;

    if (argv[1][0] == '/')
    {
        target_dir = strdup(argv[1]);
    }
    else
    {
        target_dir = find_in_cdpath(argv[1]);

        if (!target_dir)
        {
            if (directory_exists(argv[1]))
            {
                target_dir = strdup(argv[1]);
            }
        }
    }

    if (!target_dir)
    {
        fprintf(stderr, "%s: No such file or directory\n", argv[1]);
        return 1;
    }

    if (chdir(target_dir) == -1)
    {
        perror("chdir");
        free(target_dir);
        return 1;
    }

    printf("Changed to: %s\n", target_dir);
    free(target_dir);

    return 0;
}
