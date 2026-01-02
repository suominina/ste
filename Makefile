CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic `pkg-config --cflags raylib`
LIBS = `pkg-config --libs raylib`

ste: src/ste.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
