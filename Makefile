CC=gcc
CFLAGS=-W -Wall -g
EXEC=notebook
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(EXEC)

notebook: $(OBJ)
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean: 
	rm -rf *.o
	rm -rf $(EXEC)