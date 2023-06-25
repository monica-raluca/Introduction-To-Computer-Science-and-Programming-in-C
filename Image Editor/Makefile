# Copyright Birladeanu Raluca-Monica 315CAb 2022-2023

# compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm

#define targets
TARGETS=image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) image_editor.c $(CFLAGS) -o image_editor

pack:
	zip -FSr 315CA_BirladeanuRalucaMonica.zip README Makefile *.c *.h
    
clean:
	rm -f $(TARGETS)

.PHONY: pack clean
