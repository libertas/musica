#include <stdio.h>
#include <string.h>
#define INPUT_LENGTH 100

int on_add()
{
	printf("on add\n");
	return 0;
}

int on_del()
{
	printf("on del\n");
	return 0;
}

int on_help()
{
	printf("Need help?\n\n"
	       "help ?:Show this list\n"
	       "add:Add a new song list\n"
	       "del delete:Delete a song list\n"
	       "exit quit bye:Get out of here\n");
	return 0;
}

int main()
{

	printf("Welcome to Musica\n"
	       "If you don't know how to use it,entry \"help\"\n");

	char order[INPUT_LENGTH];
	while (1) {
		printf(">");
		for (int i = 0; i < INPUT_LENGTH; i++)
			order[i] = 0;
		scanf("%s[^\n]", order);

		if (strcmp(order, "add") == 0)
			on_add();

		else if (strcmp(order, "del") == 0)
			on_del();

		else if (strcmp(order, "help") == 0 || strcmp(order, "?") == 0)
			on_help();

		else if (strcmp(order, "exit") == 0
			 || strcmp(order, "quit") == 0
			 || strcmp(order, "bye") == 0)
			goto l_quit;
		else if (strcmp(order, "") == 0) {
			printf("\n");
			goto l_quit;
		} else
			printf("%s%s\n", "Command not found:", order);

	}

      l_quit:
	printf("Bye!\n");
	return 0;
}
