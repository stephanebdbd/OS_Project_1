CC=gcc
OPT=-Wall -Wextra -std=c11 -O2
OBJS=

all: img-search

img-search: main.c $(OBJS)
	$(CC) $(OPT) $(OPT) main.c -o img-search $(OBJS)

%.o: %.c %.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@
