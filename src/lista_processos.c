#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include "lista_processos.h"

t_processo *cabeca_processos = NULL;
t_processo *corrente_processos = NULL;

t_processo *criar_lista(int pid, char *cmd)
{
    t_processo *p = (t_processo *)malloc(sizeof(t_processo));

    if (p == NULL)
    {
        printf("\nFalha ao alocar memoria\n");
        return NULL;
    }

    p->pid = pid;
    p->cmd = strdup(cmd);

    cabeca_processos = p;
    corrente_processos = cabeca_processos;
    return p;
}

t_processo *adicionar_lista(int pid, char *cmd)
{
    if (cabeca_processos == NULL)
    {
        return criar_lista(pid, cmd);
    }

    t_processo *p = (t_processo *)malloc(sizeof(t_processo));

    if (p == NULL)
    {
        printf("\nFalha ao alocar memoria\n");
        return NULL;
    }

    p->pid = pid;
    p->cmd = strdup(cmd);
    p->prox = NULL;
    corrente_processos->prox = p;
    corrente_processos = p;
    return p;
}

void imprimir_lista()
{
    t_processo *aux = cabeca_processos;

    while (aux != NULL)
    {
        printf("\t[%d] %s\n", aux->pid, aux->cmd);
        aux = aux->prox;
    }
}

int lista_vazia()
{
    if (cabeca_processos == NULL)
        return 1;
    return 0;
}

t_processo *buscar_elemento(int pid, t_processo **ant)
{

    if (lista_vazia() == 1)
        return NULL;

    t_processo *p = cabeca_processos;
    t_processo *aux_ant = NULL;
    int achou = 0;

    while (p != NULL)
    {
        if (p->pid == pid)
        {
            achou = 1;
            break;
        }
        aux_ant = p;
        p = p->prox;
    }

    if (achou == 1)
    {
        if (ant)
            *ant = aux_ant;
        return p;
    }
    return NULL;
}

int remover_elemento(int pid)
{

    t_processo *ant = NULL;

    t_processo *elem = buscar_elemento(pid, &ant);

    if (elem == NULL)
        return 0;

    if (ant != NULL)
        ant->prox = elem->prox;

    if (elem == corrente_processos)
        corrente_processos = ant;

    if (elem == cabeca_processos)
        cabeca_processos = elem->prox;

    free(elem->cmd);
    free(elem);
    elem = NULL;

    return 1;
}

void coletar_filhos(int flag)
{
    pid_t pid2;
    while ((pid2 = waitpid(-1, NULL, flag)) > 0)
    {

        t_processo *elem = buscar_elemento(pid2, NULL);
        if (elem == NULL)
            printf("Processo em background [%d] não existe.\n", pid2);
        else
            printf("Processo em background [%d] [executado] \"%s\"\n", elem->pid, elem->cmd);

        remover_elemento(pid2);
    };
}