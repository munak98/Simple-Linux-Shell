#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "builtins.h"
#include "parser.h"

void execute_cmd(int input, int output, char *cmd)
{
    char **args;
    char *real_cmd;

    args = split(cmd, " \t\r\n\a");

    if (input != STDIN_FILENO)
    {
        if (dup2(input, STDIN_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(input);
    }

    if (output != STDOUT_FILENO)
    {
        if (dup2(output, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(output);
    }

    real_cmd = translate_alias(args[0]);
    real_cmd = search_path(real_cmd);
    if (real_cmd)
    {
        execv(real_cmd, args);
        perror("execv");
    }

    else
    {
        perror("Comando não encontrado");
        exit(EXIT_FAILURE);
    }
}

void launch(int input, int output, char *cmd)
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
        perror("fork");
    if (pid == 0)
    {
        execute_cmd(input, output, cmd);
        exit(EXIT_SUCCESS);
    }

    else
    {
        if (waitpid(pid, NULL, WUNTRACED) < 0)
            perror("waitpid");
    }
}

void execute(char *line, char *input_path, char *output_path, int append)
{
    int pipefd[2];
    int PIPE_READ = 0;
    int PIPE_WRITE = 1;
    int input = STDIN_FILENO;
    int output = STDOUT_FILENO;
    int i = 0;

    char **commands;
    int count = 0;
    if (input_path != NULL)
    {
        input = open(input_path, O_RDONLY);
        if (input < 0)
            perror("open");
    }
    if (output_path != NULL)
    {
        if (append)
            output = open(output_path, O_APPEND | O_CREAT | O_WRONLY, 0777);
        else
            output = open(output_path, O_CREAT | O_WRONLY, 0777);
        if (output < 0)
            perror("open");
    }

    commands = split(line, "|");
    while (commands[count] != NULL)
        ++count;

    for (i = 0; i < count - 1; i++)
    {
        if (pipe(pipefd) < 0)
            perror("pipe");

        launch(input, pipefd[PIPE_WRITE], commands[i]);
        close(pipefd[PIPE_WRITE]);
        input = pipefd[PIPE_READ];
    }

    execute_cmd(input, output, commands[i]);
    free(commands);
}
