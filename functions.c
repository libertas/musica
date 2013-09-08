#include "musica.h"
#include "functions.h"
#include "recall.h"

char songlist[SONGLIST_LENGTH][INPUT_LENGTH];
int songlist_counter = 0;
char play_order = 'd';

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

int write2fifo(char msg[])
{
	FILE *fifo;
	if ((fifo = fopen("/tmp/musica_fifofile", "w")) != 0) {
		fprintf(fifo, "%s\n", msg);
		fclose(fifo);
	}
	return 0;
}
