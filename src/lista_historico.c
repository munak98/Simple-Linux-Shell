#include "lista_historico.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

t_historico *cabeca_historico = NULL;
int n_his = 0;

t_historico *criar_lista_historico(char *cmd)
{
    t_historico *p = (t_historico *)malloc(sizeof(t_historico));

    if (p == NULL)
    {
        printf("\nFalha ao alocar memoria\n");
        return NULL;
    }

    p->cmd = strdup(cmd);

    cabeca_historico = p;
    return p;
}

t_historico *adicionar_historico(char *cmd)
{
    if (cabeca_historico == NULL)
    {
        return criar_lista_historico(cmd);
    }

    t_historico *p = (t_historico *)malloc(sizeof(t_historico));

    if (p == NULL)
    {
        printf("\nFalha ao alocar memoria\n");
        return NULL;
    }

    p->cmd = strdup(cmd);
    p->prox = cabeca_historico;
    cabeca_historico = p;
    n_his++;

    return p;
}

void imprimir_historico(char **args)
{
    int n;
    if (args[1] != NULL)
    {
        n = atoi(args[1]);
        if (n < 1 || n > 7)
        {
            printf("[erro] argumento inválido para histórico\n");
        }
    }
    else
    {
        n = 7;
    }
    t_historico *aux = cabeca_historico->prox;

    int idx = 1;
    while (idx < n_his + 1 && n > 0)
    {
        printf("%d %s\n", idx, aux->cmd);
        aux = aux->prox;
        n--;
        idx++;
    }
}

int historico_vazio()
{
    if (cabeca_historico == NULL)
        return 1;
    return 0;
}

void destroi_historico()
{
    if (historico_vazio() == 1)
        return;

    t_historico *aux = cabeca_historico;
    t_historico *prox = NULL;

    while (aux != NULL)
    {
        prox = aux->prox;
        free(aux->cmd);
        free(aux);
        aux = prox;
    }
    return;
}