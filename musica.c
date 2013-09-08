#include "musica.h"
#include "functions.h"
#include "recall.h"

int executer(char order[INPUT_LENGTH])
{
	char name_newdir[INPUT_LENGTH];

	if (strcmp(order, "add") == 0)
		on_add();

	else if (strcmp(order, "addl") == 0)
		on_addl();

	else if (strcmp(order, "import") == 0) {
		scanf(" %[^\n]", name_newdir);
		on_import(name_newdir);
	}

	else if (strcmp(order, "importl") == 0) {
		scanf(" %[^\n]", name_newdir);
		on_importl(name_newdir);
	}

	else if (strcmp(order, "order") == 0)
		on_order();

	else if (strcmp(order, "del") == 0 || strcmp(order, "delete") == 0)
		on_del();

	else if (strcmp(order, "delall") == 0
		 || strcmp(order, "deleteall") == 0)
		on_delall();

	else if (strcmp(order, "help") == 0 || strcmp(order, "?") == 0)
		on_help();

	else if (strcmp(order, "showlist") == 0 || strcmp(order, "show") == 0)
		on_showlist();

	else if (strcmp(order, "play") == 0) {
		if (on_play('d', 0))
			printf
			    ("The songlist cannot be played.Please check the song list\n");
	}

	else if (strcmp(order, "playone") == 0) {
		int which;
		scanf("%d", &which);
		if (on_play('a', which))
			printf
			    ("The songlist cannot be played.Please check the song list\n");
	}

	else if (strcmp(order, "save") == 0)
		on_save_config();

	else if (strcmp(order, "up") == 0)
		on_up();

	else if (strcmp(order, "down") == 0)
		on_down();

	else if (strcmp(order, "exit") == 0
		 || strcmp(order, "quit") == 0
		 || strcmp(order, "bye") == 0 || strcmp(order, "q") == 0)
		return 1;

	else if (strcmp(order, "") == 0) {
		return 1;	//printf("\n");
	} else
		printf("%s%s\n", "Command not found:", order);
	return 0;
}

int main()
{
	//preparing
	char order[INPUT_LENGTH];
	int executer_returned;

	//change directory
	char *home_path;
	home_path = getenv("HOME");
	chdir(home_path);

	//read config
	FILE *stdin_backup = stdin;
	if ((stdin = fopen(CONFIG_FILE_PATH, "r")) == 0)
		stdin = stdin_backup;

	//main loop
	while (1) {
		if (stdin == stdin_backup)
			printf(">");
		for (int i = 0; i < INPUT_LENGTH; i++)
			order[i] = 0;
		scanf("%s", order);
		executer_returned = executer(order);
		if (executer_returned && (stdin == stdin_backup))
			break;
		else if (executer_returned && (stdin != stdin_backup)) {
			fclose(stdin);
			stdin = stdin_backup;
			printf("\nWelcome to Musica\n"
			       "If you don't know how to use it,entry \"help\"\n");
		}
	}

	printf("Bye!\n");
	return 0;
}
