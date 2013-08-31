#include "musica.h"
#include "functions.h"
#include "recall.h"

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
	if(mkfifo("/tmp/musica_fifofile",0644)!=0){
		printf("/tmp/musica_fifo file exists,is there another musica running?(Y/n)");
		char ans;
		scanf(" %[YyNn]",&ans);
		if(ans=='n' || ans=='N'){
			printf("Then recreate it\n");
			system("rm /tmp/musica_fifofile");
			system("mkfifo /tmp/musica_fifofile");
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
			case 'e':
				on_play_quit();
				printf("Bye!\n");
				exit(0);
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
				       "e:exit\n"
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
