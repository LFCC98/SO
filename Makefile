CC=gcc
CFLAGS=-W -Wall -g
EXEC=TP-MIEI45
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(EXEC)

TP-MIEI45: $(OBJ)
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean: 
	rm -rf *.o
	rm -rf $(EXEC)