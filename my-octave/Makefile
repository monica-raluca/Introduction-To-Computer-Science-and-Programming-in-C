# Copyright Birladeanu Raluca-Monica 315CAb 2022-2023

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=my_octave

build: $(TARGETS)

my_octave: my_octave.c
	$(CC) $(CFLAGS) my_octave.c -o my_octave

pack:
	zip -FSr 315CA_BirladeanuRalucaMonica.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
