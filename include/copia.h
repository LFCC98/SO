int numEspacos(char* line);

int numLinhas(char* buf);

int comentario(char *line);

int comand_pipe(char * line);

void colocaPalavras(char * word[], char * buf);	

int indiceAnt(char* line);

char* copiaLinha(char* buf, int *l);

char** parteComando(char* s);

char* retiraParte(char* s, int *l);

int tamPalavra(char *s, int *l);

int contaPalavra(char* s);