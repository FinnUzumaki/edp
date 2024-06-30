#ifndef __AVL__
#define  __AVL__

#include "municipio.h"

typedef struct _item
{
    municipio* municipio;
    struct _item* prox;
}titem;

typedef struct _node{
    int chave;
    titem* itens;
    struct _node *pai;
    struct _node *esq;
    struct _node *dir;
    int h;
}tnode;

void avl_insere(tnode ** parv, tnode* pai, int chave, municipio* cidade);
void avl_busca(tnode* parv, int min, int max, titem** resultados);

void _rd(tnode ** pparv);
void _re(tnode ** pparv);
void _avl_rebalancear(tnode ** pparv);

#endif