#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "municipio.h"
#include "parser.h"
#include "hash.h"
#include "kdtree.h"


int get_key(void * reg){
    return (*((municipio *)reg)).codigo_ibge;
}

void * aloca(municipio* muni){
    municipio * m = malloc(sizeof(municipio));
    m->codigo_ibge = muni->codigo_ibge;
    m->nome = muni->nome;
    m->latitude = muni->latitude;
    m->longitude = muni->longitude;
    m->capital = muni->capital;
    m->codigo_uf = muni->codigo_uf;
    m->siafi_id = muni->siafi_id;
    m->ddd = muni->ddd;
    m->fuso_horario = muni->fuso_horario;
    return m;
}

int main()
{
    int running = 1, option = -1, n = 0;
    void *result_set;
    float pos[2];
    municipio *selected = NULL;

    const char *str = readFile("municipios.json");
    municipio *arr = makeArray(str);
    
    thash tabela;
    hash_constroi(&tabela, 8192, get_key);

    struct kdtree *kd = kd_create(2);

    for(int i = 0; i < 5570; i++)
    {
        assert(hash_insere(&tabela,aloca(&arr[i])) == EXIT_SUCCESS);

        pos[0] = arr[i].latitude;
        pos[1] = arr[i].longitude;
        kd_insertf(kd, pos, &arr[i]);
    }

    while(running)
    {
        while(option < 0 || option > 3)
        {
            system("cls");
            printf("qual tarefa deseja checar?\n1 - Tarefa 1\n2 - Tarefa 2\n3 - Tarefa 3\n0 - sair\n");
            scanf("%d", &option);
        }

        switch (option)
        {
        case 0:
            running = 0;
            break;
        case 1:
            system("cls");
            printf("digite o código ibge da cidade que quer achar: ");
            scanf("%d", &option);
            printf("\n");

            selected = hash_busca(tabela, option);
            if(selected)
            {
                printf("codigo_ibge: %d\n", selected->codigo_ibge);
                printf("nome: %s\n", selected->nome);
                printf("latitude: %f\n", selected->latitude);
                printf("longitude: %f\n", selected->longitude);
                printf("capital: %d\n", selected->capital);
                printf("codigo_uf: %d\n", selected->codigo_uf);
                printf("siafi_ide: %d\n", selected->siafi_id);
                printf("ddd: %d\n", selected->ddd);
                printf("fuso_horario: %d\n", selected->fuso_horario);
            } else
            {
                printf("cidade não encontrada.");
            }
            break;
        case 2:
            system("cls");
            printf("digite o codigo ibge da cidade desejada: ");
            scanf("%d", &option);
            printf("quantas cidades proximas deseja: ");
            scanf("%d", &n);

            selected = hash_busca(tabela, option);
            pos[0] = selected->latitude;
            pos[1] = selected->longitude;
            result_set =  kd_nearest_nf(kd, pos, n);

            while(1)
            {
                selected = (municipio*)kd_res_item(result_set, NULL);
                if(selected) printf("%d\n",selected->codigo_ibge);

                if(!kd_res_next(result_set)) break;
            }
            kd_res_free(result_set);

            break;
        case 3:
            system("cls");
            printf("digite o codigo ibge da cidade desejada: ");
            scanf("%d", &option);
            printf("quantas cidades proximas deseja: ");
            scanf("%d", &n);

            selected = hash_busca(tabela, option);
            pos[0] = selected->latitude;
            pos[1] = selected->longitude;
            result_set =  kd_nearest_nf(kd, pos, n);

            while(1)
            {
                selected = (municipio*)kd_res_item(result_set, NULL);
                if(selected)
                {
                    printf("\n");
                    printf("codigo_ibge: %d\n",selected->codigo_ibge);
                    printf("nome: %s\n",selected->nome);
                    printf("latitude: %f\n",selected->latitude);
                    printf("longitude: %f\n",selected->longitude);
                    printf("capital: %d\n",selected->capital);
                    printf("codigo_uf: %d\n",selected->codigo_uf);
                    printf("siafi_id: %d\n",selected->siafi_id);
                    printf("ddd: %d\n",selected->ddd);
                    printf("fuso_horario: %s\n",selected->fuso_horario);
                }
                if(!kd_res_next(result_set)) break;
            }
            kd_res_free(result_set);
            break;
        }

        if(option != 0)
        {
            fflush(stdin);
            getchar();
        }

        option = -1;
    }

}

