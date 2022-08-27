#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"
#include "parser.h"
#include "builtins.h"
#include "lista_historico.h"
#include "lista_processos.h"

int main(int argc, char **argv)
{
    char *line = NULL;
    char *input_path = NULL;
    char *output_path = NULL;
    char **tokens;
    char c;

    int in_background = 0;
    int is_pipe = 0;
    int redirect_input = 0;
    int redirect_output = 0;
    int append_mode = 0;
    int interative_mode = 0;
    int batch_mode = 0;
    pid_t pid;
    int i = 0;
    FILE *fp;
    size_t len = 0;
    int position;
    char *initial_dir;

    if (argc == 2)
    {
        batch_mode = 1;
        fp = fopen(argv[1], "rt");
        if (fp == NULL)
            exit(EXIT_FAILURE);
        if (setvbuf(fp, NULL, _IONBF, 0) < 0)
        {
            perror("setvbuf(3) failed");
            exit(EXIT_FAILURE);
        }
        initial_dir = validate_file(argv[1], fp);
        if (chdir(initial_dir) == -1)
        {
            perror("chdir");
        }
        position = 2;
    }
    else if (argc > 2)
    {
        printf("Argumentos demais.\n");
    }
    else
    {
        interative_mode = 1;
        position = 1;
    }

    path_dirs = get_path();
    read_aliases();
    while (1)
    {
        coletar_filhos(WNOHANG);
        if (interative_mode)
        {
            show_prompt();
            line = read_line();
        }
        if (batch_mode)
        {
            if (getline(&line, &len, fp) == -1)
            {
                if (feof(fp))
                {
                    printf("Fim do arquivo em lote.\n");
                    fclose(fp);
                    break;
                }

                else
                {
                    exit(EXIT_FAILURE);
                }
            }
            if (line[0] == '#' || line[0] == '\0' || line[0] == '\n')
            {
                if (line != NULL)
                    free(line);
                line = NULL;
                continue;
            }
        }
        adicionar_historico(line);

        if (line[strlen(line) - 1] == '\0')
            continue;
        if (strchr(line, '|') != NULL)
            is_pipe = 1;
        if (strchr(line, '<') != NULL)
            redirect_input = 1;
        if (strchr(line, '>') != NULL)
            redirect_output = 1;
        if (strstr(line, ">>") != NULL)
            append_mode = 1;
        if (line[strlen(line) - position] == '&')
        {
            in_background = 1;
            line[strlen(line) - position] = '\0';
        }

        if (builtin(line))
        {
            continue;
        }
        pid = fork();
        if (pid == 0)
        {
            if (in_background)
            {
                setpgid(0, 0);
            }
            if (redirect_output)
            {
                tokens = split(line, ">");
                output_path = tokens[1];
                line = tokens[0];
            }

            if (redirect_input)
            {
                tokens = split(line, "<");
                input_path = tokens[1];
                line = tokens[0];
            }

            execute(line, input_path, output_path, append_mode);
            exit(EXIT_SUCCESS);
        }
        else
        {
            if (in_background)
            {
                printf("Processo em background [%d] foi iniciado\n", adicionar_lista(pid, line)->pid);
            }
            else
            {
                waitpid(pid, NULL, WUNTRACED);
            }
        }

        is_pipe = 0;
        in_background = 0;
        redirect_input = 0;
        redirect_output = 0;
        append_mode = 0;
        input_path = NULL;
        output_path = NULL;
        if (line != NULL && !in_background)
        {
            free(line);
        }
        line = NULL;
    }
    printf("\nEsperando os processos em background:\n");
    imprimir_lista();
    coletar_filhos(0);
    return 0;
}
