#ifndef SHELL_H
#define SHELL_H

#define BUFFER_SIZE 100
#define PATH_SIZE 200


void mishell();
int execute_command(char **tab_tokens, int *taille,int *nbr);
void Command_CD(char **tab_tokens,int *nbr);
void exit_mishell();
void instructions();
char **parser(char *cmd, int *taille );
int exec_other_cmd(char **cmd, int *nbr);
void exec_piped_CMD(int *taille, char **com);
void is_piped(char **cmd, int *taille, int *piped);
void pipe_parser(char **cmd, int *taille);
void exec_piped_CMD(int *taille, char **com);
void bg_command(char **cmd, int *taille, int *bg);

void redirection(char **cmd);


#endif


