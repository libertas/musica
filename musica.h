#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#define INPUT_LENGTH 256
#define SONGLIST_LENGTH 256
#define CONFIG_FILE_PATH ".musica_config"
#define MPLAYER "mplayer -slave -input file=/tmp/musica_fifofile"
#define MPLAYER_ENDING "&"
#define SLEEP_TIME 1

char songlist[SONGLIST_LENGTH][INPUT_LENGTH];
int songlist_counter = 0;
char play_order = 'd';
