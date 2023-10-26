CC=gcc
OPT=-Wall -Wextra -O2
DBG_OPT= #-g -fsanitize=address,undefined
LIBS=-lm
OBJS=bmp.o pHash.o verbose.o

all: img-dist

img-dist: main-img-dist.c $(OBJS)
	$(CC) $(OPT) $(DBG_OPT) main-img-dist.c -o img-dist $(OBJS) $(LIBS)

%.o: %.c %.h
	$(CC) $(OPT) $(DBG_OPT) -c $< -o $@
