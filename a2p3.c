#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void)
{
	char string[81];
	
	char* argv[10];

	while(1)
	{	
		printf("The Magic Conch Shell$ ");			
		
		fgets(string, 81, stdin);
			
			if(strcmp(string, "exit\n") == 0)
			{
				break;
			}
			
		pid_t pid = fork();	
		
		if(pid == 0)
		{		
			int i = 0;
			
			argv[i] = strtok(string, " \n");
			
			while(argv[i] != NULL)
			{	
				argv[++i] = strtok(NULL, " \n");
			}
			execvp(argv[0], argv);
		}
		
		else
		{
			waitpid(-1, NULL, 0);
		}
	}
	
	return 0;
}

