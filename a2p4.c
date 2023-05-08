#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void tokenize(char insideString[81]);


int main(void)
{
	char* command;
	
	char string[81], insideString[81];


	while(1)
	{	
		printf("The Magic Conch Shell$ ");			
		
		fgets(string, 81, stdin);
			
			if(strcmp(string, "exit\n") == 0)
			{
				break;
			}
			
		command = strtok(string, ";\n");
		
		while(command != NULL)
		{
			strcpy(insideString, command);
			
			printf("%s\n", insideString);
				
			pid_t pid = fork();	
			
			if(pid == 0)
			{
				tokenize(insideString);
			}
			
			else
			{
				waitpid(-1, NULL, 0);
			}
			
			command = strtok(NULL, ";\n");
		}
	}
	
	return 0;
}

void tokenize(char insideString[81])
{
	int i = 0;
	
	char* argv[10];
	
	argv[i] = strtok(insideString, " \n");
	
	while(argv[i] != NULL)
	{	
		argv[++i] = strtok(NULL, " \n");
		
	}
	
	execvp(argv[0], argv);
}










