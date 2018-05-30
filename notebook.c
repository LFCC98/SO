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

#define MAXTAM 4000

struct llista{
	int num;
	int tam;
	char* linha;
	struct llista *next;
	struct llista *ant;
};

void copiaAnt(char* buffer, Lista lis, int t){
	int count = 0;
	while(t > 0){
		if(t >= MAXTAM)
			memcpy(buffer + count, lis -> linha, MAXTAM);
		else memcpy(buffer + count, lis -> linha, t);
		t = t - MAXTAM;
	}
}

char* pegaAnt(Lista lis, int indice){
	char * buffer;
	int t = 0, n;
	if(lis == NULL || lis -> next == NULL)
		return NULL;
	else{
		Lista aux = lis;
		while(aux -> next != NULL)
			aux = aux -> next;
		n = (aux -> num) - indice;
		if(n < 0)
			return NULL;
		Lista anterior = aux -> ant;
		if(aux -> num != n)
			while(anterior -> ant != NULL && anterior -> num != n){
				aux = anterior;
				anterior = anterior -> ant;
			}
		t = strlen(aux -> linha);
		while(anterior != NULL && anterior -> num == n){
			t += strlen(anterior -> linha);
			aux = anterior;
			anterior = anterior -> ant;
		}

		buffer = malloc(t);
		copiaAnt(buffer, aux, t);
	}
	return buffer;
}

Lista deuErro(){
	Lista lis = malloc(sizeof(struct llista));
	lis -> linha = NULL;
	lis -> num = -1;
	lis -> tam = 0;
	lis -> next = NULL;
	lis -> ant = NULL;
	return lis;
}

Lista processaLista(Lista lis, char* buffer, int tam, int n){
	int i = 1;
	if(lis == NULL){
		lis = malloc(sizeof(struct llista));
		lis -> linha = malloc(sizeof(char) * tam + 1);
		lis -> linha = buffer;
		lis -> num = i;
		lis -> tam = tam;
		lis -> next = NULL;
		lis -> ant = NULL;
	}
	else{
		Lista l = lis, ant;
		l = lis -> next;
		ant = lis;
		i++;
		while(l != NULL){
			i++;
			ant = l;
			l = l -> next;
		}
		l = malloc(sizeof(struct llista));
		l -> linha = malloc(sizeof(char) * tam + 1);
		l -> linha = buffer;
		if(n == 0)
			l -> num = i;
		else
			l -> num = ant -> num;
		l -> tam = tam;
		l -> next = NULL;
		l -> ant = ant;
		ant -> next = l;
	}
	return lis;
}

Lista processalinha(char * line, int n, Lista lis){

	char **palavras;
	int status = 0, tam, r = 1, indice, h = 0, pid[2], pff[2];
	char *ant, *buffer = malloc(MAXTAM);
	
	pipe(pff);
	pipe(pid);

	line[n - 1] = '\n';
	if(comand_pipe(line)){
		palavras = parteComando(line);
		indice = indiceAnt(line);
		ant = pegaAnt(lis, indice);
		if(ant == NULL){
			lis = deuErro();
			return lis;
		}
		write(pff[1], ant, strlen(ant) + 1);
		if(!fork()){
			dup2(pff[0], 0);
			close(pff[0]);
			close(pff[1]);
			dup2(pid[1], 1);
			close(pid[1]);
			close(pid[0]);
			status = execvp(palavras[1], &palavras[1]);
			perror("Erro ao executar");
			_exit(-1);
		}
		close(pff[1]);
		close(pff[0]);
		close(pid[1]);
		wait(&status);
		if(WEXITSTATUS(status))
			lis = deuErro();
		else{
			while(r && (tam = read(pid[0], buffer, MAXTAM))){
				if(tam < MAXTAM){
					buffer[tam - 1] = '\n';
					buffer[tam] = '\0';
					r = 0;
				}
				lis = processaLista(lis, buffer, tam, h);
				h = 1;
			}
		close(pid[0]);
		}
	}
	else if(!comentario(line)){
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
			while(r && (tam = read(pid[0], buffer, MAXTAM))){
				if(tam < MAXTAM){
					buffer[tam - 1] = '\n';
					buffer[tam] = '\0';
					r = 0;
				}
				lis = processaLista(lis, buffer, tam, h);
				h = 1;
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

	for(int i = 0; i < n && (lis == NULL || lis -> num != -1); i++){
		i = avancaLinhas(linhas, i);
		if(i < n)
			lis = processalinha(linhas[i], tamlinha[i], lis);
	}
	
	if(lis != NULL && lis -> num != -1){
		close(file);
		file = open(path, O_WRONLY | O_TRUNC);
		escreveFile(file, linhas, tamlinha, n, lis);
	}

	close(file);
}