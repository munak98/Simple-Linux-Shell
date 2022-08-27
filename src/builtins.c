
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "parser.h"
#include "builtins.h"
#include "lista_historico.h"

int n_alias = 0;

char *builtin_str[] = {
    "historico",
    "ver",
    "exit",
    "muda",
    "cd"};

void (*builtin_func[])(char **) = {
    &imprimir_historico,
    &infos,
    &to_exit,
    &change,
};

void infos(char **args)
{
    printf("BRsh versão 1.0.0\nÚltima atualização: 31/07/2022\nAutor: Manuela Matos\n");
}

void to_exit(char **args)
{
    exit(0);
}

void change(char **args)
{
    if (chdir(args[1]) != 0)
    {
        printf("chdir failed %s\n", strerror(errno));
    }
}

int builtin(char *line)
{
    char **args;
    char *line_copy = strdup(line);
    args = split(line_copy, " \t\r\n\a");
    for (int i = 0; i < 5; i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            if (i == 4)
            {
                builtin_func[i - 1](args);
            }
            else
            {
                builtin_func[i](args);
            }
            return 1;
        }
    }
    free(args);
    free(line_copy);
    return 0;
}

void read_aliases(void)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char file[256];
    char **tokens;

    strcat(strcpy(file, getenv("HOME")), "/.BRshrc");

    fp = fopen(file, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        n_alias++;
    }

    my_aliases = (alias *)malloc(sizeof(alias) * n_alias);

    int i = 0;
    rewind(fp);
    while ((read = getline(&line, &len, fp)) != -1)
    {
        tokens = split(line, " ");
        my_aliases[i].real_name = trim(tokens[1], '\"');
        my_aliases[i].alias_name = trim(tokens[2], '\"');
        i++;
        tokens = NULL;
        line = NULL;
    }
    fclose(fp);
    return;
}

char *translate_alias(char *cmd)
{
    int counter = 0;
    while (counter < n_alias)
    {
        if (strcmp(cmd, my_aliases[counter].alias_name) == 0)
        {
            return (my_aliases[counter].real_name);
        }
        counter++;
    }
    return cmd;
}

char **get_path(void)
{
    int size = 1024; // initial buffer size
    int i = 0;       // index to the line array
    int c;           // variable to get the enter caracters
    FILE *config;
    char **tokens;
    char file[256];
    strcat(strcpy(file, getenv("HOME")), "/.BRbshrc_profile");

    char *paths = malloc(sizeof(char) * size); // array to store the command

    if ((config = fopen(file, "r")) == NULL)
    {
        perror("fopen");
    };

    if (!paths)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = fgetc(config);
        if (feof(config))
        {
            break;
        }

        if (c == EOF || c == '\n')
        {
            paths[i] = '\0';
            break;
        }
        else
        {
            paths[i] = c;
        }
        i++;

        if (i >= size)
        {
            size = size + 1024;
            paths = realloc(paths, size);
            if (!paths)
            {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    i = 0;
    tokens = split(paths, "=;");

    free(paths);
    return &tokens[1];
}

char *search_path(char *cmd)
{
    struct stat sb;
    int i = 0;
    int found = 0;
    char cwd[FILENAME_MAX];
    char new_cmd[256];
    char *path;
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
    }

    if (stat(cmd, &sb) == 0)
    {
        found = 1;
    }

    else
    {
        while (path_dirs[i] != NULL)
        {

            chdir(path_dirs[i]);

            if (stat(cmd, &sb) == 0)
            {
                path = strdup(path_dirs[i]);
                strcat(strcat(path, "/"), cmd);
                found = 1;
                cmd = path;
                break;
            }
            i++;
        }
        chdir(cwd);
    }

    if (found)
        return cmd;
    else
        return NULL;
}
