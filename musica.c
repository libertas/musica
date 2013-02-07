#include <stdio.h>
#include <string.h>
#define INPUT_LENGTH 100

int on_add()
{
	return 0;
}

int on_command(char *src)
{
	if(strcmp(src,'add')!=0) on_add();
	return 0;
}

int main()
{

	printf("Welcome to Musica\n"
	       "If you don't know how to use it,entry \"help\"\n");

	char command[INPUT_LENGTH];
	while (1) {
		printf(">");
		for (int i = 0; i < INPUT_LENGTH; i++)
			command[i] = 0;
		scanf("%s[^\n]", command);
		fflush(stdin);	//May be useless
		on_command(command);
	}

	return 0;
}
