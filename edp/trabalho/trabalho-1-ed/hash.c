#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define SEED    0x12345678

#include "hash.h"

uint32_t hashf(int n, uint32_t h){
    h ^= n;
    h *= 0x5bd1e995;
    h ^= h >> 15;
    return h;
}

uint32_t hash2(int n, uint32_t h)
{
    n += 2535373;
    return n;
}


int hash_insere(thash * h, void * bucket){
    uint32_t hash = hashf(h->get_key(bucket),SEED);
    int pos = hash %(h->max);
    /*se esta cheio*/
    if (h->max == (h->size+1)){
        free(bucket);
        return EXIT_FAILURE;
    }else{  /*fazer a insercao*/
        while(h->table[pos] != 0){
            if (h->table[pos] == h->deleted)
                break;
            pos = hash2(pos, SEED)%h->max;
        }
        h->table[pos] = (uintptr_t)bucket;
        h->size +=1;
    }
    return EXIT_SUCCESS;
}



int hash_constroi(thash * h,int nbuckets, int (*get_key)(void *) ){
    h->table = calloc(sizeof(void *),nbuckets + 1);
    if (h->table == NULL){
        return EXIT_FAILURE;
    }
    h->max = nbuckets + 1;
    h->size = 0;
    h->deleted = (uintptr_t)&(h->size);
    h->get_key = get_key;
    return EXIT_SUCCESS;
}


void * hash_busca(thash  h, int key){
    int pos = hashf(key,SEED) % (h.max);
    void * ret = NULL;
    while(h.table[pos]!=0 && ret == NULL){
        if (h.get_key((void*)h.table[pos]) == key){
            ret = (void *) h.table[pos];
        }else{
            pos = hash2(pos, SEED) % h.max;
        }
    }
    return ret;

}

int hash_remove(thash * h, int key){
    int pos = hashf(key,SEED) % (h->max);
    while(h->table[pos]!=0){
        if (h->get_key((void*)h->table[pos]) == key){ /* se achei remove*/
            free((void *)h->table[pos]);
            h->table[pos] = h->deleted;
            h->size -= 1;
            return EXIT_SUCCESS;
        }else{
            pos = hash2(pos, SEED) % h->max;
        }
    }
    return EXIT_FAILURE;
}

void hash_apaga(thash *h){
    int pos;
    for (pos =0; pos < h->max;pos++){
        if (h->table[pos] != 0){
            if (h->table[pos]!=h->deleted){
                free((void*) h->table[pos]);
            }
        }
    }
    free(h->table);
}


