#include<stdio.h>
#include<string.h>

int main(void)
{
	char string[80];
	
	printf("Enter the string\n");
	
	fgets(string, 80, stdin);
	
	char* token = strtok(string, " ");
	
	while(token != NULL)
	{
		printf("%s\n", token);
		
		token = strtok(NULL, " ");
	}
	return 0;
}
	
