#include<stdio.h>
#include<stdlib.h>
#include "avl.h"

int max(int a,int b){
    return a>b?a:b;
}

int altura(tnode *arv){
    int ret;
    if (arv==NULL){
        ret = -1;
    }else{
        ret = arv->h;
    }
    return ret;
}


void avl_insere(tnode ** parv, tnode* pai, int chave, municipio* cidade){
    if (*parv == NULL){
        *parv = (tnode *) malloc(sizeof(tnode));
        (*parv)->chave = chave;
        (*parv)->itens = (titem*) malloc(sizeof(titem));
        (*parv)->itens->municipio = cidade;
        (*parv)->itens->prox = NULL;
        (*parv)->pai = pai;
        (*parv)->esq = NULL;
        (*parv)->dir = NULL;
        (*parv)->h = 0;

    }else if((*parv)->chave == chave)
    {
        titem** temp = &(*parv)->itens->prox;
        while((*temp) != NULL) temp = &(*temp)->prox;
        (*temp) = (titem*) malloc(sizeof(titem));
        (*temp)->municipio = cidade;
        (*temp)->prox = NULL;

    }else if(((*parv)->chave - chave)>0){
        avl_insere(&(*parv)->esq, (*parv), chave, cidade);
    }else{
        avl_insere(&(*parv)->dir, (*parv), chave, cidade);
    }
    (*parv)->h = max(altura((*parv)->esq),altura((*parv)->dir)) + 1;
    _avl_rebalancear(parv);
}
void _rd(tnode **parv){
    tnode * y = *parv; 
    tnode * x = y->esq;
    tnode * A = x->esq;
    tnode * B = x->dir;
    tnode * C = y->dir;

    y->esq = B; 
    x->dir = y;
    x->pai = y->pai;
    y->pai = x;
    *parv  = x;
    y->h = max(altura(B),altura(C)) + 1;
    x->h = max(altura(A),altura(y)) + 1;
}

void _re(tnode **parv){
    tnode * x = *parv; 
    tnode * y = x->dir;
    tnode * A = x->esq;
    tnode * B = y->esq;
    tnode * C = y->dir;

    x->dir = B;
    y->esq = x;
    y->pai = x->pai;
    x->pai = y;
    *parv  = y;
    x->h = max(altura(A),altura(B)) + 1;
    y->h = max(altura(x),altura(C)) + 1;
}


void _avl_rebalancear(tnode **parv){
    int fb;
    int fbf;
    tnode * filho;
    fb = altura((*parv)->esq) - altura((*parv)->dir);

    if (fb  == -2){
        filho = (*parv)->dir;
        fbf = altura(filho->esq) - altura(filho->dir);
        if (fbf <= 0){ /* Caso 1  --> ->*/
            _re(parv);
        }else{   /* Caso 2  --> <-*/
            _rd(&(*parv)->dir);
            _re(parv);
        }
    }else if (fb == 2){  
        filho = (*parv)->esq;
        fbf = altura(filho->esq) - altura(filho->dir);
        if (fbf >=0){ /* Caso 3  <-- <-*/
            _rd(parv);
        }else{  /* Caso 4  <-- ->*/
            _re(&(*parv)->esq);
            _rd(parv);
        }
    }
}

void avl_busca(tnode* parv, int min, int max, titem** resultados)
{
    if(parv == NULL) return;

    if(parv->chave >= min && parv->chave <= max)
    {
        titem* temp = parv->itens;
        while(temp != NULL)
        {
            *resultados = (titem*) malloc(sizeof(titem));
            (*resultados)->municipio = temp->municipio;
            (*resultados)->prox = NULL;
            resultados = &((*resultados)->prox);
            temp = temp->prox;
        }
    }
    if(parv->chave <= max)
        avl_busca(parv->dir, min, max, resultados);
    while((*resultados) != NULL) resultados = &(*resultados)->prox;
    if(parv->chave >= min)
        avl_busca(parv->esq, min, max, resultados);

}