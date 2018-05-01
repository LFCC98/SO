CC=gcc
CFLAGS=-W -Wall -ansi -pedantic -g
EXEC=programa
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(EXEC)

programa: $(OBJ)
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean: 
	rm -rf *.o
	rm -rf $(EXEC)