#include "functions.h"

int on_save_config();

int (*on_import)(char *name_newdir);

int on_importl(char *name_newdir);

int on_add();

int on_addl();

int on_showlist();

int on_delall();

int on_del();

int on_help();

int on_play(char setting, int which);

int on_order();

int on_up();

int on_down();
