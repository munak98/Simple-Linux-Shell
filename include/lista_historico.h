typedef struct historico
{
    char *cmd;
    struct historico *prox;
} t_historico;

extern t_historico *cabeca_historico;
extern int n_his;

t_historico *criar_lista_historico(char *);

t_historico *adicionar_historico(char *);

void imprimir_historico(char **);

int historico_vazio();

void destroi_historico();