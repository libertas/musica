CC=cc
CFLAGS = -O2 -std=c99 -march=native -Wall
objects = musica.o recall.o functions.o
destination= /usr/local/bin/musica

musica:$(objects)
	$(CC) $(CFLAGS)   -o musica $(objects)


.PHONY:clean indent install uninstall
clean:
	-rm $(objects)
	-rm musica
indent:
	indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -o musica.c musica.c
	indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -o recall.c recall.c
	indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -o functions.c functions.c

install:musica
	cp musica $(destination)

uninstall:
	rm $(destination)
