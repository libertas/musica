#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define INPUT_LENGTH 200
#define SONGLIST_LENGTH 20
#define CONFIG_FILE_PATH ".musica_config"

char songlist[SONGLIST_LENGTH][INPUT_LENGTH];
int songlist_counter = 0;

inline int save_config()
{
	FILE *fp;
	fp = fopen(CONFIG_FILE_PATH, "w");
	for (int i = 0; i < SONGLIST_LENGTH && songlist[i][0] != (char)0; i++)
		fprintf(fp, "%s %s\n", "add", songlist[i]);
	fclose(fp);
	return 0;
}

inline int on_add()
{
	scanf("%s^[\n]", songlist[songlist_counter]);
	songlist_counter++;
	save_config();
	return 0;
}

inline int on_showlist()
{
	for (int i = 0; i < SONGLIST_LENGTH && songlist[i][0] != (char)0; i++)
		printf("%s\n", songlist[i]);
	return 0;
}

inline int on_del()
{
	if (songlist_counter != 0) {
		songlist_counter--;
		printf("\"%s\"%s\n", songlist[songlist_counter], " is deleted");
		for (int i = 0; i < INPUT_LENGTH; i++)
			songlist[songlist_counter][i] = (char)0;
		save_config();
	} else
		printf("The songlist is empty\n");
	return 0;
}

inline int on_help()
{
	printf("Need help?\n\n"
	       "help ? :Show this list\n"
	       "add :Add a new song list\n"
	       "del delete :Delete a song list\n"
	       "showlist show :Show the songlists you have added\n"
	       "exit quit bye :Get out of here\n"
	       "\nIf you want to keep your own \".musica_config\",please not to add or delete anything from this program.\n");
	return 0;
}

inline int executer(char order[INPUT_LENGTH])
{
	if (strcmp(order, "add") == 0)
		on_add();

	else if (strcmp(order, "del") == 0 || strcmp(order, "delete") == 0)
		on_del();

	else if (strcmp(order, "help") == 0 || strcmp(order, "?") == 0)
		on_help();

	else if (strcmp(order, "showlist") == 0 || strcmp(order, "show") == 0)
		on_showlist();

	else if (strcmp(order, "exit") == 0
		 || strcmp(order, "quit") == 0 || strcmp(order, "bye") == 0)
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
	printf("Welcome to Musica\n"
	       "If you don't know how to use it,entry \"help\"\n");
	
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
	      l_loop_start:
		if (stdin == stdin_backup)
			printf(">");
		for (int i = 0; i < INPUT_LENGTH; i++)
			order[i] = 0;
		scanf("%s[^\n]", order);
		executer_returned = executer(order);
		if (executer_returned && (stdin == stdin_backup))
			goto l_quit;
		else if (executer_returned && (stdin != stdin_backup)) {
			fclose(stdin);
			stdin = stdin_backup;
			goto l_loop_start;
		}
	}

      l_quit:
	printf("Bye!\n");
	return 0;
}
