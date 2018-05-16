#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

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
		else if(s[i] == ' ' && s[i + 1] != ' ')
			j++;
		i++;
	}
	return j;
}

int tamPalavra(char *s, int *l){
	int i = 0;
	int j = *l;
	while(s[j] != ' ' && s[j] != '\0'){
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
	char *c = malloc(tam + 1);

	while(i < tam){
		c[i] = s[i + *l];
		i++;
	}
	*l += tam;
	return c;
}

char** parteComando(char* s){
	int k = contaPalavra(s), pal = 0, a = 0, tam;
	char **c;

	while(pal < k){
		c[pal] = retiraParte(s, &a);
		pal++;
	}
	return c;
}