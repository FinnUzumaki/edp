#include <stdio.h>
#include <stdlib.h>
#include "../include/test_hash.h"
#include "../include/test_heap.h"

int main()
{
    test_pai();
    test_filho_esq();
    test_filho_dir();
    test_desce();
    test_constroi_heap();
    test_sobe();
    test_acessa_max();
    test_extrai_max();
    test_insere_elemento();
    test_altera_prioridade();
    test_heapsort();

    test_insere();
    //test_search();
    //test_remove();

    printf("funciona\n");
}