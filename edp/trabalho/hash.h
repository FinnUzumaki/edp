#ifndef __HASH__
#define __HASH__

#include <stdint.h>

typedef struct {
    uintptr_t * table;
    int size;
    int max;
    uintptr_t deleted;
    int (*get_key)(void *);
}thash;

uint32_t hashf(int n, uint32_t h);
uint32_t hash2(int n, uint32_t h);
int hash_insere(thash * h, void * bucket);
int hash_constroi(thash * h,int nbuckets, int (*get_key)(void *) );
void * hash_busca(thash h, int key);
int hash_remove(thash * h, int key);
void hash_apaga(thash *h);

#endif