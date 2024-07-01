#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "municipio.h"
#include "parser.h"
#include "hash.h"
#include "kdtree.h"


int get_keyi(void * reg){
    return (*(municipio *)reg).codigo_ibge;
}

int get_keys(void * reg){
    int sum = 0;
    for(int i = 0; (*(municipio*)reg).nome[i] != '\0'; i++)
    {
        sum += (*(municipio*)reg).nome[i];
    }
    return sum;
}

int get_keystr(char* str)
{
    int sum = 0;
    for(int i = 0; str[i] != '\0'; i++)
    {
        sum += str[i];
    }
    return sum;
}

void printKD(kdtree* kd)
{
    printf("%d\n", kd->node->codigo_ibge);
    printKD(kd->left);
    printKD(kd->right);
}

int main()
{
    int running = 1, option = -1, n = 0;
    char nome[20];
    float pos[2];
    municipio *selected = NULL;
    municipio **result_set;

    const char *str = readFile("municipios.json");
    municipio *arr = makeArray(str);
    
    thash tabelaCodigo;
    thash tabelaNome;
    hash_constroi(&tabelaCodigo, 8192, get_keyi);
    hash_constroi(&tabelaNome, 8192, get_keys);

    kdtree *kd = NULL;

    for(int i = 0; i < 5570; i++)
    {
        assert(hash_insere(&tabelaCodigo, &arr[i]) == EXIT_SUCCESS);
        assert(hash_insere(&tabelaNome, &arr[i]) == EXIT_SUCCESS);
        insertKdtree(&kd, &arr[i], 0);
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

            selected = hash_busca(tabelaCodigo, option);
            if(selected)
            {
                printf("codigo_ibge: %d\n", selected->codigo_ibge);
                printf("nome: %s\n", selected->nome);
                printf("latitude: %f\n", selected->coordenadas[0]);
                printf("longitude: %f\n", selected->coordenadas[1]);
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

            selected = hash_busca(tabelaCodigo, option);
            result_set =  nearestnKdtree(kd, selected, n);

            for(int i = 0; i < n; i++)
            {
                selected = result_set[i];
                if(selected) printf("%d\n",selected->codigo_ibge);
            }
            free(result_set);

            break;
        case 3:
            system("cls");
            printf("digite o nome da cidade desejada: ");
            scanf(" %[^\n]s", nome);
            printf("quantas cidades proximas deseja: ");
            scanf("%d", &n);

            selected = hash_busca(tabelaNome, get_keystr(nome));
            result_set =  nearestnKdtree(kd, selected, n);

            for(int i = 0; i < n; i++)
            {
                selected = result_set[i];
                if(selected)
                {
                    printf("\n");
                    printf("codigo_ibge: %d\n",selected->codigo_ibge);
                    printf("nome: %s\n",selected->nome);
                    printf("latitude: %f\n",selected->coordenadas[0]);
                    printf("longitude: %f\n",selected->coordenadas[1]);
                    printf("capital: %d\n",selected->capital);
                    printf("codigo_uf: %d\n",selected->codigo_uf);
                    printf("siafi_id: %d\n",selected->siafi_id);
                    printf("ddd: %d\n",selected->ddd);
                    printf("fuso_horario: %s\n",selected->fuso_horario);
                }
            }
            free(result_set);
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

