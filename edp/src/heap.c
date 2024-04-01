#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include "../include/heap.h"

typedef struct{
    void ** v;
    int tam;
    void  (*cmp)(void *, void *);
}theap;


int pai(int n){
    return (n-1)/2;
}

int filho_esq(int n){
    return 2*n+1;
}
int filho_dir(int n){
    return 2*n+2;
}

void troca(int *a,int *b){
    int aux = *a;
    *a = *b;
    *b = aux;
}
void desce(int v[],int n, int max_size){
    int e,d,maior;
    e = filho_esq(n);
    d = filho_dir(n);
    maior = n;
    if (e < max_size && v[e] > v[maior] ) maior = e; 
    if (d < max_size && v[d] > v[maior] ) maior = d; 
    if (maior!=n){
        troca(&v[maior],&v[n]);
        desce(v,maior,max_size);
    }
}

void constroi_heap(int v[], int max_size){
    int i;
    for (i=pai(max_size-1);i>=0;i--)
        desce(v,i,max_size);
}

void sobe(int v[], int pos){
    while(v[pai(pos)] < v[pos]){
        troca(&v[pai(pos)],&v[pos]);
        pos = pai(pos);
    }
}

int acessa_max(int v[]){
    return v[0];
}

int extrai_max(int v[], int * tam){
    int ret = v[0];
    *tam = *tam -1;
    v[0] = v[*tam];
    desce(v,0,*tam);
    return ret;
}
int insere_elemento(int v[], int *tam, int max, int novo){
    int ret = EXIT_FAILURE;
    if (*tam <= max){
        v[*tam] = novo;
        sobe(v,*tam);
        *tam += 1;
        ret = EXIT_SUCCESS;
    }
    return ret;
}
int altera_prioridade(int v[],int tam, int pos, int novo){
    if (v[pos] < novo){ /*sobe*/
        v[pos] = novo;
        sobe(v,pos);
    }else if (v[pos] > novo){ /*desce*/
        v[pos] = novo;
        desce(v,pos,tam);
    }
    return EXIT_SUCCESS;
}
void heapsort(int v[], int tam){
    int i;
    for (i = tam-1;i>=0;i--){
        troca(&v[0],&v[i]);
        desce(v,0,i);
    }
}
