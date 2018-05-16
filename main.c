#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "notebook.h"

int wrongFile(char a[]){
	int l = strlen(a);
	int pos = l - 3;
	return (strncmp(a + pos, ".nb", 3));
}

int main(int argc, char * const argv[]){

	int file;

	if(argc < 2){
		perror("Faltam argumentos\n");
		exit(-1);
	}

	if(wrongFile(argv[1])){
		perror("Não é possível ler esse tipo de ficheiros\n");
		exit(-1);
	}

	file = open(argv[1], O_RDWR);

	if(file == -1){
		perror("Não foi possível abir o ficheiro\n");
		exit(-1);
	}

	processa(file);

	return 0;
}