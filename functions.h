
int format_dir_for_import(char *name_newdir);

int import(char *name_newdir);

int getdirname_loop(DIR * dirp_root, char *name_root);

int write2fifo(char msg[]);
