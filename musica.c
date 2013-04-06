#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#define INPUT_LENGTH 256
#define SONGLIST_LENGTH 256
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

int format_dir_for_import(char *name_newdir)
{
	//if the last char is not '/' then it will be added
	char *ptr;
	for (ptr = name_newdir; *ptr != 0; ptr++) ;
	if (*(ptr - 1) != '/') {
		*ptr = '/';
		*(ptr + 1) = 0;
	}
	return 0;
}

int import(char *name_newdir)
{
	DIR *p_newdir;
	if ((p_newdir = opendir(name_newdir)) == 0) {
		printf("%s:the directory doesn't exist\n", name_newdir);
		return 1;
	} else {
		format_dir_for_import(name_newdir);

		//if exist
		for (int i = 0; i < SONGLIST_LENGTH; i++)
			if (strcmp(songlist[i], name_newdir) == 0)
				return 1;

		//if all the things are right
		strcpy(songlist[songlist_counter], name_newdir);
		songlist_counter++;
		closedir(p_newdir);
		printf("%s is imported\n", name_newdir);
		return 0;
	}
}

int getdirname_loop(DIR * dirp_root, char *name_root)
{
	//finding directories
	{
		import(name_root);
		//int i = 0;
		for (struct dirent * entry = readdir(dirp_root); entry != 0;
		     entry = readdir(dirp_root)) {
			if ((int)(entry->d_type) == 4	/*"4" means it is a directory */
			    && strcmp(entry->d_name, ".") != 0
			    && strcmp(entry->d_name, "..") != 0) {

				char name_new[INPUT_LENGTH];
				format_dir_for_import(name_root);
				strcpy(name_new, name_root);
				strcat(name_new, entry->d_name);
				format_dir_for_import(name_new);
				import(name_new);

				//recursion
				DIR *dirp;
				if ((dirp = opendir(name_new)) != 0) {
					getdirname_loop(dirp, name_new);
					closedir(dirp);
				}
			}
		}
	}

	return 0;
}

int on_importl(char *name_newdir)
{
	DIR *p_newdir;
	if ((p_newdir = opendir(name_newdir)) == 0) {
		printf("%s:the directory doesn't exist\n", name_newdir);
		return 1;
	} else {
		getdirname_loop(p_newdir, name_newdir);
		closedir(p_newdir);
	}
	return 0;
}

int on_add()
{
	char name_newdir[INPUT_LENGTH];
	scanf(" %[^\n]", name_newdir);
	import(name_newdir);
	on_save_config();
	return 0;
}

int on_addl()
{
	char name_newdir[INPUT_LENGTH];
	scanf(" %[^\n]", name_newdir);
	on_importl(name_newdir);
	on_save_config();
	return 0;
}

int on_showlist()
{
	for (int i = 0; i < SONGLIST_LENGTH && songlist[i][0] != (char)0; i++)
		printf("[%d] %s\n", i, songlist[i]);
	return 0;
}

int on_delall()
{
	for (int i = 0; i < songlist_counter; i++)
		songlist[i][0] = (char)0;
	songlist_counter = 0;
	on_save_config();
	return 0;
}

int on_del()
{
	if (songlist_counter != 0) {
		int which;
		scanf("%d", &which);	//delete which
		for (int i = which; i < SONGLIST_LENGTH; i++)
			strcpy(songlist[i], songlist[i + 1]);
		//for (int i = 0; i < INPUT_LENGTH; i++)
		//songlist[songlist_counter][i] = (char)0;
		songlist[songlist_counter][0] = (char)0;	//faster
		songlist_counter--;
		on_save_config();
		return 0;
	} else
		printf("The songlist is empty\n");
	return 1;
}

int on_help()
{
	printf("Need help?\n\n"
	       "help ? :Show this list\n"
	       "play :Play the songs in the song list\n"
	       "playone :Play one of the directories in the list(input a number follow it)\n"
	       "add :Add a new song list and save it into the config file\n"
	       "import :Add a new song list without saving it\n"
	       "importl :the same as import but import all the direcotries in the directory\n"
	       "addl :addl is to add what importl is to import\n"
	       "del delete :Delete a song list(must be followed by a number)\n"
	       "delall deleteall :Delete all the things in the song list\n"
	       "showlist show :Show the songlists you have added\n"
	       "save :Save the options into the config file\n"
	       "order :set play order(will not save unless run 'save' after it),'d' stands for default,'r' stands for ramdom\n"
	       "up :move the list up\n"
	       "down :move the list down\n"
	       "exit quit bye q :Get out of here\n"
	       "\nIf you want to keep your own \".musica_config\",please not to add or delete anything from this program.\n"
	       "\nCurrent order:%c\n", play_order);
	return 0;
}

int write2fifo(char msg[])
{
	FILE *fifo;
	if ((fifo = fopen("/tmp/musica_fifofile", "w")) != 0) {
		fprintf(fifo, "%s\n", msg);
		fclose(fifo);
	}
	return 0;
}

int on_play_quit()
{
	write2fifo("quit");
	system("rm /tmp/musica_fifofile");
	sleep(SLEEP_TIME);
	return 0;
}

int on_play(char setting, int which)
{
	//checking if musica_fifofile exists
	FILE *fifo;
	if ((fifo = fopen("/tmp/musica_fifofile", "r")) != 0){
		printf("/tmp/musica_fifo file exists,is there another musica running?(Y/n)");
		char ans;
		scanf(" %[YyNn]",&ans);
		if(ans=='n' || ans=='N'){
			printf("Then recreate it\n");
			system("rm /tmp/musica_fifofile");
		}
		else{
			printf("musica cannot playing two songs at the same time\n");
			return 1;
		}
	}
	
	char command[strlen(MPLAYER) + INPUT_LENGTH * SONGLIST_LENGTH +
		     strlen(MPLAYER_ENDING)];

	//checking if the songlist is empty
	if (songlist[0][0] == (char)0)
		return 1;

	//preparing playlist
	{
		DIR *songdir;
		struct dirent *entry;
		FILE *playlist_file = fopen(".musica_playlist", "w");
		if (setting == 'a') ;
		else
			which = 0;
		for (int i = which; i < songlist_counter; i++) {
			songdir = opendir(songlist[i]);
			entry = readdir(songdir);
			while (entry) {
				fprintf(playlist_file, "%s%s\n",
					songlist[i], entry->d_name);
				entry = readdir(songdir);
			}
			closedir(songdir);
			if (which != 0)
				break;
		}
		fclose(playlist_file);
	}
	if (play_order == 'r')
		system
		    ("sort <.musica_playlist >.musica_playlist.backup -R;mv .musica_playlist.backup .musica_playlist");

	//playing
	sprintf(command, "%s %s %s", MPLAYER,
		"-playlist .musica_playlist", MPLAYER_ENDING);
	system("mkfifo /tmp/musica_fifofile");

	printf
	    ("\033[31mIf you want to see the key bindings,please inpu 's'\n\033[0m");
	sleep(SLEEP_TIME);
	if (system(command) == 0) {
		write2fifo("volume 100 q");
		char control;
		int n = 1;
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
}

int on_order()
{
	char order_inputed;
	scanf(" %c",&order_inputed);
	if (order_inputed == 'r' || order_inputed == 'd')
		play_order = order_inputed;
	return 0;
}

int on_up()
{
	int which;
	char buffer[INPUT_LENGTH];
	scanf("%d",&which);
	if(which<=0 || which>songlist_counter) return 1;
	strcpy(buffer,songlist[which]);
	strcpy(songlist[which],songlist[which-1]);
	strcpy(songlist[which-1],buffer);
	return 0;
}

int on_down()
{
	int which;
	char buffer[INPUT_LENGTH];
	scanf("%d",&which);
	if(which<0 || which>=songlist_counter-1) return 1;
	strcpy(buffer,songlist[which]);
	strcpy(songlist[which],songlist[which+1]);
	strcpy(songlist[which+1],buffer);
	return 0;
}

int executer(char order[INPUT_LENGTH])
{
	char name_newdir[INPUT_LENGTH];

	if (strcmp(order, "add") == 0)
		on_add();

	else if (strcmp(order, "addl") == 0)
		on_addl();

	else if (strcmp(order, "import") == 0) {
		scanf(" %[^\n]", name_newdir);
		import(name_newdir);
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
