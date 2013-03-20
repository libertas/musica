#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#define INPUT_LENGTH 200
#define SONGLIST_LENGTH 20
#define CONFIG_FILE_PATH ".musica_config"
#define MPLAYER "mplayer -slave -input file=/tmp/musica_fifofile"
#define MPLAYER_ENDING "&"
#define SLEEP_TIME 1

char songlist[SONGLIST_LENGTH][INPUT_LENGTH];
int songlist_counter = 0;
char play_order = 'd';

int on_save_config()
{
	FILE *fp;
	fp = fopen(CONFIG_FILE_PATH, "w");

	//save songlist
	for (int i = 0; i < SONGLIST_LENGTH && songlist[i][0] != (char)0; i++)
		fprintf(fp, "%s %s\n", "import", songlist[i]);

	//save play_order
	fprintf(fp, "order %c\n", play_order);

	fclose(fp);
	return 0;
}

int on_import()
{
	char name_newdir[INPUT_LENGTH];
	DIR *p_newdir;
	scanf("%s^[\n]", name_newdir);
	if ((p_newdir = opendir(name_newdir)) == 0)
		printf("%s:the directory doesn't exist\n", name_newdir);
	else {
		//if the last char is not '/' then it will be added
		char *ptr;
		for (ptr = name_newdir; *ptr != 0; ptr++) ;
		if (*(ptr - 1) != '/') {
			*ptr = '/';
			*(ptr + 1) = 0;
		}
		printf("%s is imported\n", name_newdir);

		strcpy(songlist[songlist_counter], name_newdir);
		songlist_counter++;
		closedir(p_newdir);
		return 0;
	}
	return 1;
}

int on_add()
{
	on_import();
	on_save_config();
	return 0;
}

int on_showlist()
{
	for (int i = 0; i < SONGLIST_LENGTH && songlist[i][0] != (char)0; i++)
		printf("[%d] %s\n", i,songlist[i]);
	return 0;
}

int on_del()
{
	if (songlist_counter != 0) {
		songlist_counter--;
		printf("\"%s\"%s\n", songlist[songlist_counter], " is deleted");
		for (int i = 0; i < INPUT_LENGTH; i++)
			songlist[songlist_counter][i] = (char)0;
		on_save_config();
	} else
		printf("The songlist is empty\n");
	return 0;
}

int on_help()
{
	printf("Need help?\n\n"
	       "help ? :Show this list\n"
	       "play:Play the songs in the song list\n"
	       "add :Add a new song list and save it into the config file\n"
	       "import :Add a new song list without saving it\n"
	       "del delete :Delete a song list\n"
	       "showlist show :Show the songlists you have added\n"
	       "save :Save the options into the config file\n"
	       "order :set play order(will not save unless run 'save' after it),'d' stands for default,'r' stands for ramdom\n"
	       "exit quit bye q :Get out of here\n"
	       "\nIf you want to keep your own \".musica_config\",please not to add or delete anything from this program.\n"
	       "\nCurrent order:%c\n", play_order);
	return 0;
}

int write2fifo(char msg[])
{
	FILE *fifo = fopen("/tmp/musica_fifofile", "w");
	fprintf(fifo, "%s\n", msg);
	fclose(fifo);
	return 0;
}

int on_play_quit()
{
	write2fifo("quit");
	system("rm /tmp/musica_fifofile");
	sleep(SLEEP_TIME);
	return 0;
}

int on_play()
{
	char command[strlen(MPLAYER) + INPUT_LENGTH * SONGLIST_LENGTH +
		     strlen(MPLAYER_ENDING)];

	//checking if the songlist is empty
	if (songlist[0][0] == (char)0)
		return 1;

	//preparing playlist
	DIR *songdir;
	struct dirent *entry;
	FILE *playlist_file = fopen(".musica_playlist", "w");
	for (int i = 0; i < songlist_counter; i++) {
		songdir = opendir(songlist[i]);

		entry = readdir(songdir);
		while (entry) {
			fprintf(playlist_file, "%s%s\n", songlist[i],
				entry->d_name);
			entry = readdir(songdir);
		}

		closedir(songdir);
	}
	fclose(playlist_file);
	if (play_order == 'r')
		system
		    ("sort <.musica_playlist >.musica_playlist.backup -R;mv .musica_playlist.backup .musica_playlist");

	//playing
	sprintf(command, "%s %s %s", MPLAYER, "-playlist .musica_playlist",
		MPLAYER_ENDING);

	system("mkfifo /tmp/musica_fifofile");

	int n = 1;
	char control;
	printf
	    ("\033[31mIf you want to see the key bindings,please inpu 's'\n\033[0m");
	sleep(SLEEP_TIME);
	if (system(command) == 0) {
		write2fifo("volume 100 q");
		do {
			control = getchar();
			switch (control) {
			case 'q':
				on_play_quit();
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
		on_play_quit();
		return 1;
	}

	printf("All the things are finished\n");
	on_play_quit();
	return 0;

}

int on_order()
{
	char order_inputed;
	getchar();
	order_inputed = getchar();
	if (order_inputed == 'r' || order_inputed == 'd')
		play_order = order_inputed;
	return 0;
}

int executer(char order[INPUT_LENGTH])
{
	if (strcmp(order, "add") == 0)
		on_add();

	else if (strcmp(order, "import") == 0)
		on_import();

	else if (strcmp(order, "order") == 0)
		on_order();

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

	else if (strcmp(order, "save") == 0)
		on_save_config();

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
			if (stdin == stdin_backup)
				printf(">");
			for (int i = 0; i < INPUT_LENGTH; i++)
				order[i] = 0;
			scanf("%s[^\n]", order);
			executer_returned = executer(order);
			if (executer_returned && (stdin == stdin_backup)) {
				system("rm /tmp/musica.lck");
				break;
			} else if (executer_returned && (stdin != stdin_backup)) {
				fclose(stdin);
				stdin = stdin_backup;
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
	printf("Bye!\n");
	return 0;
}
