#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define INPUT_LENGTH 200
#define SONGLIST_LENGTH 20
#define CONFIG_FILE_PATH ".musica_config"
#define MPLAYER "mplayer -slave -input file=/tmp/musica_fifofile "
#define MPLAYER_ENDING " &"
#define SLEEP_TIME 3

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
	       "play:Play the songs in the song list\n"
	       "add :Add a new song list\n"
	       "del delete :Delete a song list\n"
	       "showlist show :Show the songlists you have added\n"
	       "exit quit bye q :Get out of here\n"
	       "\nIf you want to keep your own \".musica_config\",please not to add or delete anything from this program.\n");
	return 0;
}

inline int write2fifo(char msg[])
{
	FILE *fifo = fopen("/tmp/musica_fifofile", "w");
	fprintf(fifo, "%s\n", msg);
	fclose(fifo);
	return 0;
}

inline int on_play()
{
	char command[strlen(MPLAYER) + INPUT_LENGTH * SONGLIST_LENGTH +
		     strlen(MPLAYER_ENDING)];

	strcpy(command, MPLAYER);
	for (int i = 0; i < SONGLIST_LENGTH && songlist[i][0] != (char)0; i++) {
		strcat(command, songlist[i]);
		strcat(command, "/* ");
	}
	strcat(command, MPLAYER_ENDING);

	system("mkfifo /tmp/musica_fifofile");

	int n = 1;
	char control;
	printf("If you want to see the key bindings,please inpu 's'\n");
	sleep(SLEEP_TIME);
	if (!system(command)) {
		write2fifo("volume 100 q");
		do {
			control = getchar();
			switch (control) {
			case 'q':
				write2fifo("quit");
				system("rm /tmp/musica_fifofile");
				n = 0;
				break;
			case 'n':
				write2fifo("seek 9999");
				break;
			case 'm':
				write2fifo("m");
				break;
			case 'p':
				write2fifo("pause");
			case 's':
				printf("Playing....\n"
				       "The following commands are available:\n"
				       "q:stop\n" "p:pause\n" "n:next\n"
				       "m:mute\n" "s:show this menu\n");
				break;

			}
		} while (n);
		return 0;
	} else {
		write2fifo("quit");
		system("rm /tmp/musica_fifofile");
		return 1;
	}
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

	else if (strcmp(order, "play") == 0) {
		if (on_play())
			printf
			    ("The songlist cannot be played.Please check the song list\n");
	}

	else if (strcmp(order, "exit") == 0
		 || strcmp(order, "quit") == 0 || strcmp(order, "bye") == 0
		 || strcmp(order, "q") == 0)
		return 1;

	else if (strcmp(order, "") == 0) {
		return 1;	//printf("\n");
	} else
		printf("%s%s\n", "Command not found:", order);
	return 0;
}

int main()
{
	//checking lock file
	FILE *lock;
	if ((lock = fopen("/tmp/musica.lck", "r")) == 0) {
	      l_checking:
		system("echo \"1\">/tmp/musica.lck");

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
			if (executer_returned && (stdin == stdin_backup)) {
				system("rm /tmp/musica.lck");
				goto l_quit;
			} else if (executer_returned && (stdin != stdin_backup)) {
				fclose(stdin);
				stdin = stdin_backup;
				goto l_loop_start;
			}
		}
	} else {
		fclose(lock);
		printf
		    ("Another musica is running.If not,please delete /tmp/musica.lck\n"
		     "Do you want to delete the file and run musica now?(y/N)");
		if (getchar() == 'y' || getchar() == 'Y') {
			system("rm /tmp/musica.lck");
			goto l_checking;
		}
	}
      l_quit:
	printf("Bye!\n");
	return 0;
}
