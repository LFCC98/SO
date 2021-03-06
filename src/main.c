/**
@file main.c
Ficheiro com a função principal
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "notebook.h"

/**
\brief Função que testa se uma string acaba em ".nb"
@param a String a verificar
@returns 0 para falso, caso contrário verdadeiro
*/
int wrongFile(char a[]){
	int l = strlen(a);
	int pos = l - 3;
	return (strncmp(a + pos, ".nb", 3));
}

int main(int argc, char * const argv[]){

	int file;
	int stderr;

	if(argc < 2){
		perror("Faltam argumentos\n");
		exit(-1);
	}

	if(wrongFile(argv[1])){
		perror("Não é possível ler esse tipo de ficheiros\n");
		exit(-1);
	}

	stderr = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if(stderr == -1){
		perror("Não foi possível abir o ficheiro\n");
	}
	else{
		dup2(stderr, 2);
		close(stderr);
	}

	file = open(argv[1], O_RDWR);

	if(file == -1){
		perror("Não foi possível abir o ficheiro\n");
		exit(-1);
	}

	processa(file, argv[1]);

	return 0;
}