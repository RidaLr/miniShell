#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include "mishell.h"


//Commands list
char *cmd_List[] = { "cd", "exit" };
char cwd[PATH_SIZE];

void exit_mishell()
{	
	exit(0);
}

void instructions()
{
	printf("\033[0;31m");
	printf(" We recommend you to use the user manual \n");
	printf("\t Usage : man <your_command_here>    \n\n");
	printf(" Or to use the help command              \n");
	printf("\t Usage : <your_command_here> <-h> or <-help>\n\n");
	printf("\n To show a one liner usage of the command from the manual you can use whatis\n");
	printf("\t Usage : whatis <your_command_here>    \n\n");
	printf("\033[0m");
}

//************ The parser *************
char** parser(char *cmd, int *taille )
{
	*taille = 0;
    
	char *token;
	char delim[] = " ";
	char **tok = (char **)malloc(0);
	// get the first token 
	token = strtok(cmd, delim);

	
	while (token != NULL)
	{
		(*taille)++;
		tok = (char **)realloc(tok, sizeof(char *) * (*taille));
		tok[*taille - 1] = (char *)malloc(sizeof(char) * (strlen(token)));

		int tokenLength = strlen(token);
		int index = 0;
		for (int i = 0; i < tokenLength; i++)
		{
			if (isspace(token[i]) == 0)
			{
				tok[*taille - 1][index] = token[i];
				//printf("tok = %c", tokenRes[nb_token - 1][index]);
				index++;
			}
		}            
		if (index == 0) // decrement back the nb_token because nothing was filled during this iteration
			(*taille)--;
		token = strtok(NULL, delim);
	}
	return tok;

}


//************ test if there is a pipe in the buffer *************
void is_piped(char **cmd, int *taille, int *piped)
{
	(*piped)=0;
	for (int i = 0; i < *taille; i++)
	{
	
		if (strcmp(cmd[i],"|") == 0)
		{
			(*piped) = 1;
		}
		
	}
}


//************ The pipe parser *************
void pipe_parser(char **cmd, int *taille)
{
	char **pipedcommande = (char**)malloc(0);
	
	int index = 0;
	int j = 0;
	int c = 0;
	for (int i = 0; i < *taille; i++)
	{
		pipedcommande = (char **)malloc(sizeof(char *) * (*taille));
		pipedcommande[j] = (char *)malloc(sizeof(char) * (strlen(cmd[i])));
		if (strcmp(cmd[i],"|") != 0)
		{
		
			pipedcommande[j] = cmd[i];
			j++;
		}
		else
		{
			index++;
			c++;
		}
		
	}
	(*taille) -=c; // U pdate the size after remove all the pipes <|> 
	exec_piped_CMD(taille, pipedcommande);
		
}

// Execute the piped commands
void exec_piped_CMD(int *taille, char **com) 
{    
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    for (int i = 0; i < *taille ; i++) 
	{

        int pid = fork();
        if (pid < 0) 
		{
            perror("Fork error");
        }
        if (pid == 0) 
		{
    
            if (i != ((*taille)-1)) 
			{
                int fd;
                fd = open(com[i], O_CREAT | O_TRUNC | O_WRONLY, mode);
                dup2(fd, 1);
                close(fd);
            }
            if (i != 0) 
			{
                int fdI;
                fdI = open(com[i], O_RDONLY, mode);
                fflush(stdout);
                dup2(fdI, 0);
                close(fdI);
            }

            execvp(com[0], com);
            perror("Execvp");
        } 
		else 
		{
            waitpid(pid, NULL, 0);
        }
    }
    dup2(0, 1);
}


//************ test if the command should be executed in the background *************
void bg_command(char **cmd, int *taille, int *bg)
{
	(*bg)=0;
	
	if (strcmp(cmd[*taille-1],"&") == 0)
	{
		(*bg) = 1;
	}	
}


//Execute CD command
void Command_CD(char **tab_tokens, int *nbr)
{
	
	char *dir = (char*)malloc(sizeof(char)*BUFFER_SIZE);
	
	if(*nbr<2) // if the input contains cd + space ' ' or just cd
	{
		instructions(); // show some instructions to help the user
	}
	else
		if(strcmp(tab_tokens[1], "~") == 0)
		{
			
			if(chdir(getenv("HOME")) < 0)
			{
				perror("miShell : cd ");
				instructions();
			}
		}
		else
		{
			strcpy(dir, tab_tokens[1]);
			if(chdir(dir) == -1)
			{
				perror("miShell : cd1 ");
			}
		}
}



/* execute command */
int execute_command(char **cmd, int *nbr, int *bg)
{
	int nbCmd=(int)sizeof(cmd_List) / sizeof(char *);
	int num_cmd=0;

	for (int i = 0; i < nbCmd; i++) 
	{
		if (strcmp(cmd[0], cmd_List[i]) == 0) 
		{
			//printf(" comm is %s \n",cmd[0]);
			num_cmd = i + 1;
		}
	}
	
	switch(num_cmd)
	{
		case 1:
			Command_CD(cmd,nbr);
			return 1;
		case 2:
			exit_mishell();
			return 0;
		default:
			return exec_other_cmd(cmd, bg);
			break;
	}
	return 0;
}

//Implementation of Input / Output redirections (< , > , >> , 2>)

void redirection(char **cmd)
{
	int i = 0;
	int fdI, fdO;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	while (cmd[i]) 
		{
            if (*cmd[i] == '>' && cmd[i+1] != NULL) 
			{
                if ((fdO = open (cmd[i+1],O_WRONLY | O_CREAT, mode)) == -1) 
				{
                    perror ("mishel output > : ");
                }
                dup2 (fdO, 1);
                close (fdO);
                while (cmd[i]) 
				{
                    cmd[i] = cmd[i+2];
                    i++; 
                }
                break;
            }
            else if (*cmd[i] == '<' && cmd[i+1] != NULL)  
			{
				//cmd[i+1] contain the name of file desciptor
                if ((fdI = open (cmd[i+1], O_RDONLY)) == -1) 
				{
                    perror ("mishel input < : ");
                }
                dup2 (fdI, 0);
                close (fdI);
                while (cmd[i]) 
				{
                    cmd[i] = cmd[i+2];
                    i++; 
                }
                break;
            }
            else if (strcmp(cmd[i], ">>") == 0 && cmd[i+1] != NULL)  
			{
				//cmd[i+1] contain the name of file desciptor
                if ((fdO = open (cmd[i+1], O_APPEND, mode)) == -1) 
				{
                    perror ("mishel input >> : ");
                }
                dup2 (fdO, 1);
                close (fdO);
                while (cmd[i]) 
				{
                    cmd[i] = cmd[i+2];
                    i++; 
                }
                break;
            }
			//stderr
			else if (strcmp(cmd[i], "2>") == 0 && cmd[i+1] != NULL)  
			{
				
				if((fdO=open(cmd[i+1], O_WRONLY | O_TRUNC, mode)) <0)
				{
					if((fdO=creat(cmd[i+1], O_WRONLY | O_TRUNC)) < 0)
					{
						perror("mishell >2 ");
					}
				}
				dup2 (fdO, 1);
                close (fdO);
                while (cmd[i]) 
				{
                    cmd[i] = cmd[i+2];
                    i++; 
                }
                break;
			}
            i++;
        }
}

//execute the other commands except cd and exit that we defined before, using fork and execvp
int exec_other_cmd(char **cmd, int *bg)
{
	
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0) 
	{
		// Child process (pid = 0)

		redirection(cmd);
		
		if (execvp(cmd[0], cmd) == -1) //show an error message when the fonction execvp return -1
		{
			perror("mishell : ");
		}
		exit(EXIT_FAILURE);
	} 
	else 
		if (pid < 0) 
		{
			//error forking
			perror("mishell : ");
		} 
		else 
		{
			// Parent process (pid >0)
			if (*bg==1)
			{
				wait(NULL);
			}
			else
			{
				waitpid(pid, &status, WUNTRACED);
			}
				
			
		}

	return 1;
}



void mishell(){
	printf("\e[2J\e[H");  //Clear the screen (clear)
	printf("\n\033[1;33m*************************************************************\033[0m");
	 printf(R"EOF(
	
  __  __   _____        ____   _    _   _____   _       _      
 |  \/  | |_   _|     / ____| | |  | | |  ___| | |     | |     
 | \  / |   | |  __  | (___   | |__| | | |__   | |     | |     
 | |\/| |   | | |__|  \___ \  |  __  | |  __|  | |     | |     
 | |  | |  _| |_       ___) | | |  | | | |___  | |___  | |___ 
 |_|  |_| |_____|     |____/  |_|  |_| |_____| |_____| |_____|
	)EOF");

	printf("\n\n\033[1;33m====================== MISHELL ==============================\033[0m");
	printf("\n\033[1;33mADVANCED OPERATING SYSTEMS -PROJECT | ISTY 2020 IATIC4         \033[0m");
	printf("\n\033[1;33m                 R I D A       L A K S I R                     \033[0m");
	printf("\n\033[1;33m=============================================================\033[0m\n");
	printf("\n\033[1;33mPlease wait .........\033[0m\n");
	sleep(4);
	printf("\e[2J\e[H");//Clear the screen (clear)

	char buffer[BUFFER_SIZE]="";	// buffer to hold command line
	char **commande = (char**)malloc(sizeof(char*)*BUFFER_SIZE);

	//get the username from the enveronement variable USER
	char *p=getenv("USER");
	
	char *percent="%";
	
	//Prompt and read the input
   	while(1)
   	{
    	
    	printf("\033[1;32m%s@isty :\033[0m ",p);//show user_name@isty
    	printf("\033[1;34m%s\033[0m",getcwd(cwd, 200)); // show the current directory like (/home/user)
    	printf("%s ",percent);
    	
		fflush(stdout);
		
		if ( fgets(buffer,BUFFER_SIZE,stdin) == NULL )
		{
		    if (feof(stdin))
		    {
				//END OF FILE : when the user type CTRL+D
				printf("\n CTRL + D : END OF FILE <EOF>\n");
				printf("\n Exit \n");
			   	
			}
			else
			{
				perror("standard input error : while reading stdin");
			}
		   	break;
		}
		
		int *taille = (int*)malloc(sizeof(int));
		int *piped = (int*)malloc(sizeof(int));
		int *bg = (int*)malloc(sizeof(int));
		commande = parser(buffer, taille);
		 
		bg_command(commande, taille,bg);
		is_piped(commande, taille, piped);

		
		if (*piped == 0)
		{
			execute_command(commande, taille, bg);
		}
		else if(*piped == 1)
		{
			pipe_parser(commande, taille);
		}
	   

	}
}
