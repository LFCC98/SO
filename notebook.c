#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "notebook.h"

int numespacos(char* line){

	int conta = 0;
	int i;
	for(i = 0; *(line + i) != '\n'; i++){
		if(*(line + i) == ' ')
			conta++;
	}
	return conta;
}

int comentario(char *line){
	if(*(line) == '$')
		return 0;
	return 1;
}

void colocaPalavras(char * word[], char * buf){

	int i, j;
	int pal = 0;
	char *palavra;
	for(i = 0; buf[i] != '\n' && buf[i] != '\0'; i++){
		for(j = i; buf[j] != ' '; j++);
		palavra = malloc(j + 1);
		strncpy(palavra, buf, j);
		palavra[j - 1] = '\0';
		word[pal] = palavra;
		pal++;
		i = j;
	}
}

void processalinha(int tempfile, char * line, int n){

	char * m = ">>>\n";
	char * x = "<<<\n";
	int espacos = numespacos(line) + 2;
	char * words[espacos];

	colocaPalavras(words, line);
	words[espacos - 1] = NULL;

	if(comentario(line)){
		write(tempfile, line, n);
	}
	else{
		write(tempfile, m, 4);
		if(!fork()){
			dup2(1, tempfile);
			execvp(words[1], &words[1]);
		}
		wait(NULL);
		write(tempfile, x, 4);
	}
}

ssize_t readFileLine(int f, char * buf){

	int x, y = 0;
	while(y < TAM && (x = read(f, &buf[y], 1)) > 0){
		y += x;
		if(buf[y - 1] == '\n')
			break;
	}

	return y;
}

void processa(int file){

	int tam = lseek(file, 0, SEEK_END) + 1;
	lseek(file, 0, SEEK_SET);

	char *buf = (char*) malloc(tam);

	int n;
	n = read(file, buf, tam);
	write(1, buf, n);
}