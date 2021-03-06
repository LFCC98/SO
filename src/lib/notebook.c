/**
@file notebook.c
Ficheiro que faz grande parte do trabalho do sistema de processamento
*/

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

int sig = 0;

/**
\brief Funçao que altera o valor da variável global sig
*/
void print(){
	sig = 1;
}

/**
\brief Estrutura que define uma lista ligada
*/
struct llista{
	int num;
	int tam;
	char* linha;
	struct llista *next;
	struct llista *ant;
};

/**
\brief Funçao que permite guardar um elemento de uma lista, mesmo se este estiver para trás
@param lis Lista na qual queremos pegar o elemento
@param indice Índice do elemento que queremos pegar
@returns Uma string, que é o elemento da lista
*/
char* pegaAnt(Lista lis, int indice){
	char * buffer;
	int t = 0, n;
	if(lis == NULL)
		return NULL;

	Lista aux = lis -> next;
	Lista anterior = lis;
	while(aux != NULL){
		aux = aux -> next;
		anterior = anterior -> next;
	}
	n = (anterior -> num) - indice;
	if(n < 0)
		return NULL;
	while(anterior -> num != n)
		anterior = anterior -> ant;

	t = strlen(anterior -> linha);
	buffer = malloc(t);
	strcpy(buffer, anterior -> linha);
	return buffer;
}

/**
\brief Funçao que dá uma lista "resetada"
@returns A lista mencionada
*/
Lista deuErro(){
	Lista lis = malloc(sizeof(struct llista));
	lis -> linha = NULL;
	lis -> num = -1;
	lis -> tam = 0;
	lis -> next = NULL;
	lis -> ant = NULL;
	return lis;
}

/**
\brief Funçao que processa e inicializa uma lista
@param lis Lista a ser processada
@param buffer String que será elemento da lista
@param tam Inteiro correspondente ao tamanho da lista
@returns A lista já processada
*/
Lista processaLista(Lista lis, char* buffer, int tam){
	if(lis == NULL){
		lis = malloc(sizeof(struct llista));
		lis -> linha = malloc(sizeof(char) * tam + 1);
		strcpy(lis -> linha, buffer);
		lis -> num = 1;
		lis -> tam = tam;
		lis -> next = NULL;
		lis -> ant = NULL;
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
		strcpy(l -> linha, buffer);
		l -> num = 1;
		l -> tam = tam;
		l -> next = NULL;
		l -> ant = ant;
		ant -> next = l;
	}
	return lis;
}

/**
\brief Funçao que insere um nodo numa lista
@param lis Lista que receberá o nodo
@param nodo Nodo a ser adicionado
@returns A lista com o nodo adicionado
*/
Lista insereNodo(Lista lis, Lista nodo){
	if(lis == NULL){
		nodo -> num = 1;
		return nodo;
	}
	Lista aux = lis;

	while(aux -> next != NULL)
		aux = aux -> next;
	aux -> next = nodo;
	nodo -> ant = aux;
	nodo -> num = aux -> num + 1;
	return lis;
}

/**
\brief Funçao que comprime um nodo caso este tenha um tamanho maior do que o máximo estabelecido
@param nodo Nodo a ser comprimido
@returns O nodo comprimido
*/
Lista comprimeNodo(Lista nodo){
	if(nodo -> tam < MAXTAM)
		return nodo;
	int t = 0, i = 0;
	Lista aux = nodo;
	while(aux){
		t += aux -> tam;
		aux = aux -> next; 
	}

	char* buf = malloc(t + 1);
	aux = nodo;
	while(aux){
		strcpy((buf + i), aux -> linha);
		i += aux -> tam;
		aux = aux -> next;
	}
	(*(buf + t)) = '\0';
	aux = malloc(sizeof(struct llista));
	aux -> tam = t;
	aux -> linha = malloc(t + 1);
	strcpy(aux -> linha, buf);
	aux -> next = NULL;
	aux -> ant = NULL;
	return aux;
}

/**
\brief Funçao processa cada elemento (linha) de uma lista
@param line String que corresponde a um elemento da lista
@param n Int correspondente ao tamanho da linha
@param lis Lista cujos elementos serão processados
@returns A lista após o processamento dos elementos
*/
Lista processalinha(char * line, int n, Lista lis){

	char **palavras;
	int status = 0, tam, r = 1, indice, pid[2], pff[2];
	char *ant, *buffer = malloc(MAXTAM);
	pid_t p;
	int x = pipe(pff);
	int y = pipe(pid);
	if(x == -1 || y == -1){
		perror("Não deu para criar o pipe");
		lis = deuErro();
		return lis;
	}
	Lista nodo = NULL;
	line[n - 1] = '\n';

	if(comentario(line))
		return lis;
	else if(comand_pipe(line)){
		palavras = parteComando(line);
		indice = indiceAnt(line);
		ant = pegaAnt(lis, indice);
		if(ant == NULL){
			lis = deuErro();
			return lis;
		}
		write(pff[1], ant + 1, strlen(ant + 1));
		p = fork();
		if(p == -1){
			perror("Não foi possível criar o processo");
			lis = deuErro();
			return lis;
		}
		if(!p){
			char c = '\n';
			dup2(pff[0], 0);
			close(pff[0]);
			close(pff[1]);
			dup2(pid[1], 1);
			close(pid[1]);
			close(pid[0]);
			write(1, &c, 1);
			status = execvp(palavras[1], &palavras[1]);
			perror("Erro ao executar");
			_exit(-1);
		}
		close(pff[1]);
		close(pff[0]);
		close(pid[1]);
	}
	else{
		palavras = parteComando(line);
		p = fork();
		if(p == -1){
			perror("Não foi possível criar o processo");
			lis = deuErro();
			return lis;
		}
		if(!p){
			char c = '\n';
			dup2(pid[1], 1);
			close(pid[1]);
			write(1, &c, 1);
			status = execvp(palavras[1], &palavras[1]);
			perror("Erro ao executar");
			_exit(-1);
		}
	}
	wait(&status);
	if(sig == 1)
		kill(SIGKILL, p);
	else if(WEXITSTATUS(status)){
		lis = deuErro();
		return lis;
	}
	else{
		while(r && (tam = read(pid[0], buffer, MAXTAM))){
			if(tam < MAXTAM){
				buffer[tam - 1] = '\n';
				buffer[tam] = '\0';
				r = 0;
				}
			nodo = processaLista(nodo, buffer, tam);
			}
		close(pid[0]);
		nodo = comprimeNodo(nodo);
		lis = insereNodo(lis, nodo);
		}
	return lis;
}

/**
\brief Funçao que avança do início para o fim do output
@param linhas Linhas a serem avançadas
@param i Índice da linha de onde se começou
@returns Linha para onde se avançou
*/
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

/**
\brief Funçao que escreve o ficheiro
@param file Ficheiro a ser escrito
@param linhas Linhas a serem escritas
@param tamLinha Tamanho das linhas a serem escritas
@param n Número de linhas
@param l Lista com as linhas
*/
void escreveFile(int file, char* linhas[], int tamlinha[], int n, Lista l){
	int i = 0;
	char * m = ">>>";
	char * x = "<<<\n";
	
	while(i < n){
		i = avancaLinhas(linhas, i);
		write(file, linhas[i], tamlinha[i]);
		if(!comentario(linhas[i])){
			write(file, m, 3);
			write(file, l -> linha, l -> tam);
			write(file, x, 4);
			l = l -> next;
		}
		i++;		
	}
}

/**
\brief Funçao que processa o ficheiro
@param file Ficheiro a ser processado
@param path String com o caminho para o ficheiro
*/
void processa(int file, char * path){
	int tam = lseek(file, 0, SEEK_END) + 1;
	lseek(file, 0, SEEK_SET);

	char *buf = (char*)malloc(tam);
	int t = read(file, buf, tam);

	buf[tam -1] = '\n';

	if(t < tam - 1){
		perror("Nao consegui ler o ficheiro corretamente");
		exit(-1);
	}

	int n = numLinhas(buf);
	int tamlinha[n], l = 0, dif = 0;
	char *linhas[n];
	Lista lis = NULL;

	signal(SIGINT, print);

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
	
	if(lis != NULL && lis -> num != -1 && !sig){
		close(file);
		file = open(path, O_WRONLY | O_TRUNC);
		escreveFile(file, linhas, tamlinha, n, lis);
	}
	close(file);
}