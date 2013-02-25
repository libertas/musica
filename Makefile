CC=cc
CFLAGS = -O2 -std=c99 -march=native -Wall
objects = musica.o
destination= /usr/local/bin/musica

musica:$(objects)
	$(CC) $(CFLAGS)   -o musica $(objects)


.PHONY:clean indent install
clean:
	-rm $(objects)
	-rm musica
indent:
	indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -o musica.c musica.c

install:
	cp musica $(destination)

uninstall:
	rm $(destination)
