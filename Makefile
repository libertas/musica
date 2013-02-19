
CC=cc
CFLAGS = -O2 -std=c99 -Wall
objects = musica.o

musica:$(objects)
	$(CC) $(CFLAGS)   -o musica $(objects)


.PHONY:clean indent
clean:
	-rm $(objects)
	-rm musica
indent:
	indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -o musica.c musica.c
