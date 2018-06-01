typedef struct llista* Lista;

char* pegaAnt(Lista lis, int indice);

Lista deuErro();

Lista processaLista(Lista lis, char* buffer, int tam);

Lista insereNodo(Lista lis, Lista nodo);

Lista comprimeNodo(Lista nodo);

Lista processalinha(char * line, int n, Lista lis);

int avancaLinhas(char* linhas[], int i);

void escreveFile(int file, char* linhas[], int tamlinha[], int n, Lista l);

void processa(int file, char* path);
