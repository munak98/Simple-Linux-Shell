#include <stdio.h>
#include "parser.h"
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <regex.h>
#include <string.h>

void show_prompt(void)
{
    char cwd[FILENAME_MAX];

    printf("BRsh-%s-%s-> ", getenv("USERNAME"), getcwd(cwd, sizeof(cwd)));
}

void show_terminal_cwd(void)
{
    char cwd[FILENAME_MAX];

    printf("%s->", getcwd(cwd, sizeof(cwd)));
}

char *read_line(void)
{
    int size = 1024;                          // initial buffer size
    int i = 0;                                // index to the line array
    char *line = malloc(sizeof(char) * size); // array to store the command
    int c;                                    // variable to get the enter caracters

    if (!line)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();

        if (c == EOF || c == '\n')
        {
            line[i] = '\0';
            return line;
        }
        else
        {
            line[i] = c;
        }
        i++;

        if (i >= size)
        {
            size = size + 1024;
            line = realloc(line, size);
            if (!line)
            {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char *ltrim(char *s, char to_trim)
{
    while (*s == to_trim)
        s++;
    return s;
}

char *rtrim(char *s, char to_trim)
{
    char *back = s + strlen(s);
    while (*--back == to_trim)
        ;
    *(back + 1) = '\0';
    return s;
}

char *trim(char *s, char to_trim)
{
    return rtrim(ltrim(s, to_trim), to_trim);
}

char **split(char *line, char *delimiter)
{
    int size = 64;
    int i = 0;
    char **tokens = malloc(size * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, delimiter);
    while (token != NULL)
    {
        tokens[i] = trim(token, ' ');
        tokens[i] = trim(tokens[i], '\n');
        i++;

        if (i >= size)
        {
            size += 64;
            tokens = realloc(tokens, size * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, delimiter);
    }
    tokens[i] = NULL;
    return tokens;
}

char *validate_file(char *filename, FILE *fp)
{
    size_t len = 0;
    ssize_t read = 0;
    char *line = NULL;
    regex_t regex_filename, regex_line;
    int reti_filename, reti_line;
    char **tokens;

    if ((read = getline(&line, &len, fp)) == -1)
        exit(EXIT_FAILURE);

    reti_filename = regcomp(&regex_filename, "[.BRsh$]", 0);
    reti_line = regcomp(&regex_line, "[#! ./BRsh$]", 0);
    if (reti_line || reti_filename)
    {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    reti_line = regexec(&regex_line, line, 0, NULL, 0);
    reti_filename = regexec(&regex_filename, filename, 0, NULL, 0);

    if (!reti_line || !reti_filename)
    {
        regfree(&regex_filename);
        regfree(&regex_line);
    }
    else if (reti_filename == REG_NOMATCH || reti_line == REG_NOMATCH)
    {
        perror("O arquivo passado não é válido.");
    }
    else
    {
        fprintf(stderr, "Não deu match.");
        exit(1);
    }

    tokens = split(line, "!");
    tokens[1][strlen(tokens[1]) - 4] = '\0';

    return tokens[1];
}