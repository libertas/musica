
CC=cc
CFLAGS = -O2
objects = musica.o

musica:$(objects)
	$(CC) $(CFLAGS)   -o musica $(objects)

.PHONY:clean
clean:
	rm $(objects)
	rm musica
