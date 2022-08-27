typedef struct aliases
{
    char *real_name;
    char *alias_name;
} alias;

char **path_dirs;
alias *my_aliases;

extern int n_alias;

void infos(char **);

void to_exit(char **);

void change(char **);

extern char *builtin_str[];

int builtin(char *);

void read_aliases(void);
char *translate_alias(char *);

char **get_path(void);
char *search_path(char *);
