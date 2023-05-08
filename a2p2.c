#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(void)
{
	char string[80];
	
	char* token;
	
	printf("Enter the string\n");
	
	while(1)
	{
		fgets(string, 80, stdin);
		
		if(strcmp(string, "exit\n") == 0)
		{
			break;
		}
		
		token = strtok(string, " ");
	
		while(token != NULL)
		{
			printf("%s\n", token);
			
			token = strtok(NULL, " ");
		}
	}
	return 0;
}
	
