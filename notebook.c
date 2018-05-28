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

struct llista{
	int line;
	int tam;
	char* linha;
	struct llista *next;
};

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

Lista deuErro(){
	Lista lis = malloc(sizeof(struct llista));
	lis -> linha = NULL;
	lis -> line = -1;
	lis -> tam = 0;
	lis -> next = NULL;
	return lis;
}

Lista processaLista(Lista lis, char* buffer, int tam, int n){
	int i = 0;
	if(lis == NULL){
		lis = malloc(sizeof(struct llista));
		lis -> linha = malloc(sizeof(char) * tam + 1);
		lis -> linha = buffer;
		lis -> line = 0;
		lis -> tam = tam;
		lis -> next = NULL;
	}
	else{
		Lista l = lis, ant;
		l = lis -> next;
		ant = lis;
		while(l != NULL){
			ant = l;
			l = l -> next;
		}
		l = malloc(sizeof(struct llista));
		l -> linha = malloc(sizeof(char) * tam + 1);
		l -> linha = buffer;
		if(!n)
			l -> line = i;
		else
			l -> line = ant -> line;
		l -> tam = tam;
		l -> next = NULL;
		ant -> next = l;
	}
	return lis;
}

Lista processalinha(char * line, int n, Lista lis){

	char **palavras;
	int status = 0, tam, r = 1, h = 0;
	int pid[2];
	char * buffer = malloc(4000);
	
	pipe(pid);

	line[n - 1] = '\n';
	if(!comentario(line)){
		palavras = parteComando(line);
		if(!fork()){
			dup2(pid[1], 1);
			close(pid[1]);
			status = execvp(palavras[1], &palavras[1]);
			perror("Erro ao executar");
			_exit(-1);
		}
		wait(&status);

		if(WEXITSTATUS(status))
			lis = deuErro();
		else{
			while(r && (tam = read(pid[0], buffer, 4000))){
				if(tam < 4000){
					buffer[tam - 1] = '\n';
					buffer[tam] = '\0';
					r = 0;
				}
				lis = processaLista(lis, buffer, tam, h);
				n = 1;
			}
		}
	}
	return lis;
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

void escreveFile(int file, char* linhas[], int tamlinha[], int n, Lista l){
	int i = 0;
	char * m = ">>>\n";
	char * x = "<<<\n";
	
	while(i < n){
		i = avancaLinhas(linhas, i);
		write(file, linhas[i], tamlinha[i]);
		if(!comentario(linhas[i])){
			write(file, m, 4);
			write(file, l -> linha, l -> tam);
			write(file, x, 4);
			l = l -> next;
		}
		i++;		
	}
}

void processa(int file, char * path){

	int tam = lseek(file, 0, SEEK_END);
	lseek(file, 0, SEEK_SET);

	char *buf = (char*)malloc(tam);
	int t = read(file, buf, tam);

	if(t < tam){
		perror("Nao consegui ler o ficheiro corretamente");
		exit(-1);
	}

	int n = numLinhas(buf);
	char *linhas[n];
	int tamlinha[n];
	int l = 0;
	int dif = 0;
	Lista lis = NULL;

	for(int i = 0; i < n; i++){
		char *c = copiaLinha(buf, &l);
		tamlinha[i] = l - dif;
		linhas[i] = c;
		dif = l;
	}

	int i;
	for(i = 0; i < n && (lis == NULL || lis -> line != -1); i++){
		i = avancaLinhas(linhas, i);
		if(i < n)
			lis = processalinha(linhas[i], tamlinha[i], lis);
	}
	
	if(lis != NULL && lis -> line != -1){
		close(file);
		file = open(path, O_WRONLY | O_TRUNC);
		escreveFile(file, linhas, tamlinha, n, lis);
	}

	close(file);

}