typedef struct processo
{
    char *cmd;
    int pid;
    struct processo *prox;
} t_processo;

extern t_processo *cabeca_processos;
extern t_processo *corrente_processos;

t_processo *criar_lista(int, char *);

t_processo *adicionar_lista(int, char *);

void imprimir_lista();

int lista_vazia();

t_processo *buscar_elemento(int, t_processo **);

int remover_elemento(int);

void coletar_filhos(int);