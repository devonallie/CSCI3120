#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void tokenize(char insideString[81]);

void manyCommands(char stringpipe[81]);

void pipeline(char string[81]);

void execute(char* argv[10]);



int main(void)
{	
	char string[81];
	
	int x = 0;

	while(1)
	{	
		printf("The Magic Conch Shell$ ");			
		
		fgets(string, 81, stdin);
			
		pipeline(string);
	
	}
	return 0;
}

void tokenize(char insideString[81])
{
	char* argv[10];	
	
	int i = 0;
	
	argv[i] = strtok(insideString, " \n");
	
	while(argv[i] != NULL)
	{	
		//printf("string %d of tokenize function is: %s\n", i, argv[i]);
		argv[++i] = strtok(NULL, " \n");
		
	}
	
	for(int j = 0; j < i; j++)
	{
		if((strcmp(argv[j], "exit\n") == 0) || (strcmp(argv[j], "exit") == 0))
			{
				exit(1);
			}
	}	
	
	execute(argv);
}

void manyCommands(char stringpipe[81])
{
	char* pointer;
	
	char array[10][81];
	
	int i = 0;
	
	pointer = strtok(stringpipe, ";");
	
	while(pointer != NULL)
	{
		strcpy(array[i++], pointer);

		pointer = strtok(NULL, ";");
	}
	
	for(int j = 0; j < i; j++)
	{
		//printf("string %d of manyCommands function is: %s\n", j, array[j]);
		tokenize(array[j]);
	}
	
}


void pipeline(char string[81])
{
	char* pointer;
	
	char array[10][81];
	
	int i = 0;
			
	pointer = strtok(string, "|");
	
	while(pointer != NULL)
	{
		strcpy(array[i++], pointer);
	
		pointer = strtok(NULL, "|");
	}
	
	for(int j = 0; j < i; j++)
	{
		//printf("string %d of pipeline function is: %s\n", j, array[j]);
		manyCommands(array[j]);
	}

}

void execute(char* argv[10])
{
	int fd[2];
	
	pipe(fd);

	pid_t pid = fork();
	
	if(pid == 0)
	{
		execvp(argv[0], argv);
	}
	waitpid(-1, NULL, 0);
}





