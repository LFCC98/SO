#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "notebook.h"
#include "copia.h"

/*struct llista{
	char* linha;
	struct llista next;
};*/

int numEspacos(char* line){

	int conta = 0;
	for(int i = 0; *(line + i) != '\n'; i++){
		if(*(line + i) == ' ')
			conta++;
	}
	return conta;
}

int numLinhas(char* buf){

	int conta = 0;
	for(int i = 0; *(buf + i) != '\0'; i++){
		if(*(buf + i) == '\n')
			conta++;
	}
	return conta + 1;
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

int processalinha(int file, char * line, int n){

	char * m = ">>>\n";
	char * x = "<<<\n";
	char **palavras;
	int status;
	
	line[n - 1] = '\n';
	write(file, line, n);
	if(!comentario(line)){
		write(file, m, 4);
		palavras = parteComando(line);
		if(!fork()){
			dup2(file, 1);
			close(file);
			execvp(palavras[1], &palavras[1]);
		}
		//if(!WIFEXITED(NULL))
		//	return 0;
		wait(&status);
		write(file, x, 4);
	}
	return 1;
}
int avancaLinhas(char* linhas[], int i){
	int r = (strcmp(">>>\n", linhas[i]));
	while(!r){
		if(!strncmp("<<<", linhas[i], 3)){
			r = 1;
			i++;
		}
		else i++;
	}
	return i;
}

void processa(int file, char * path){

	int tam = lseek(file, 0, SEEK_END) + 1;
	lseek(file, 0, SEEK_SET);

	char *buf = (char*)malloc(tam);
	read(file, buf, tam);

	lseek(file, 0, SEEK_SET);

	int n = numLinhas(buf), r = 1;
	char *linhas[n];
	int tamlinha[n];
	int l = 0;
	int dif = 0;
	char * end = malloc(1);
	end = 0;

	for(int i = 0; i < n; i++){
		char *c = copiaLinha(buf, &l);
		tamlinha[i] = l - dif;
		linhas[i] = c;
		dif = l;
	}

	close(file);
	file = open(path, O_WRONLY | O_TRUNC);
	if(file == -1){
		write(file, buf, tam);
		perror("Não foi possível abir o ficheiro\n");
		exit(-1);
	}

	int i;
	for(i = 0; i < n && r != 0; i++){
		i = avancaLinhas(linhas, i);
		if(i < n)
			r = processalinha(file, linhas[i], tamlinha[i]);
	}
	write(file, end, 1);
	if(i < n){
		lseek(file, 0, SEEK_SET);
		write(file, buf, tam);
	}
}
