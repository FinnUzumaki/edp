#ifndef HEAP
#define HEAP


int pai(int n);
int filho_esq(int n);
int filho_dir(int n);
void troca(int *a,int *b);
void desce(int v[], int n, int max_size);
void constroi_heap(int v[], int max_size);
void sobe(int v[], int pos);

int acessa_max(int v[]);

int extrai_max(int v[], int * tam);
int insere_elemento(int v[], int *tam, int max, int novo);
int altera_prioridade(int v[],int tam, int pos, int novo);
void heapsort(int v[], int tam);

#endif