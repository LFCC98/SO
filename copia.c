#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

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

int comand_pipe(char * line){
	if(!comentario(line) && ((*(line + 1)) == '|' || ((*(line + 1)) >= 48  && (*(line + 1)) <= 57)))
		return 1;
	return 0;
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

int indiceAnt(char* line){
	int i = 0, r;
	if((*(line + 1)) == '|')
		return 0;
	else{
		i++;
		while(line[i] >= 48 && line[i] <= 57)
			i++;
		char* num = malloc(i);
		i = 1;
		while(line[i] >= 48 && line[i] <= 57){
			num[i - 1] = line[i];
			i++;
		}
		num[i - 1] = '\0';
		r = atoi(num); 
	}
	return r - 1;
}

char* copiaLinha(char* buf, int *l){
	int i = 0, j = 0;

	while(buf[i + *l] != '\n' && buf[i + *l] != '\0')
		i++;
	i++;
	char* c = malloc(sizeof(char) * (i + 1));

	while(j < i){
		c[j] = buf[j + *l];
		j++;
	}
	*l += j;

	return c;
}

int contaPalavra(char* s){
	int i = 0, j = 1;

	while(s[i] != '\n' && s[i] != '\0'){
		if(s[i] == '$');
		else if(s[i] == '|');
		else if(s[i] == '$' && s[i + 1] == '|');
		else if(s[i] == '$' && (s[i + 1] >= 48 && s[i + 1] <= 57)){
			i++;
			while(s[i] >= 48 && s[i] <= 57)
				i++;
		}
		else if(s[i] == ' ' && s[i + 1] != ' ')
			j++;
		i++;
	}
	return j;
}

int tamPalavra(char *s, int *l){
	int i = 0;
	int j = *l;
	while(s[j] != ' ' && s[j] != '\0' && s[j] != '\n'){
		i++;
		j++;
	}
	return i;
}

char* retiraParte(char* s, int *l){
	int j = *l;
	while(s[j] == ' ' && s[j] != '\0')
		j++;
	*l = j;

	int i = 0;
	int tam = tamPalavra(s, l);
	char*c = malloc(tam + 1);

	while(i < tam){
		c[i] = s[i + *l];
		i++;
	}
	*l += tam;
	return c;
}

char** parteComando(char* s){
	int k = contaPalavra(s), pal = 0, a = 0;
	char **c = malloc(sizeof(char*) * (k + 1));

	while(pal < k){
		c[pal] = retiraParte(s, &a);
		pal++;
	}
	c[k] = NULL;

	return c;
}