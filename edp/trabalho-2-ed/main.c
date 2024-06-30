#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "avl.h"
#include "hash.h"

int getkeystr(char* str)
{
    int sum = 0;
    for(int i = 0; str[i] != '\0'; i++)
    {
        sum += str[i];
    }
    return sum;
}

int getkeyfloat(float f)
{
    return (int)(f*100000);
}

char* getkeyIBGE(void* m)
{
    char *r = (char*) malloc(sizeof(char)*8);
    snprintf(r, 8,"%d", ((municipio*)m)->codigo_ibge);
    return r;
}

void freeList(titem** i)
{
    if(*i != NULL)
    {
        freeList(&(*i)->prox);
        free(*i);
    }
}

void printMunicipio(municipio* m)
{
    printf("IBGE: %d\nNome: %s\nCoordenadas: (%.5f, %.5f)\nCapital: %s\nUF: %d\nSiafi: %d\nDDD: %d\nFuso Horário: %s\n\n",
    m->codigo_ibge, m->nome, m->coordenadas[0], m->coordenadas[1], m->capital ? "Sim" : "Nao", m->codigo_uf, m->siafi_id, m->ddd, m->fuso_horario);
}

void comparaLista(titem** lista1, titem** lista2)
{
    if((*lista1) == NULL)
    {
        *lista1 = *lista2;
        *lista2 = NULL;
        return;
    }

    int delete, i = 0, j = 0;
    titem **temp1 = lista1, **temp2, *deleter, **temp0 = NULL;

    while(temp1 != NULL)
    {   delete = 1;
        temp2 = lista2;
        while(temp2 != NULL)
        {
            if((*temp1)->municipio->codigo_ibge == (*temp2)->municipio->codigo_ibge)
            {
          
                delete = 0;
                break;
            }
            temp2 = &(*temp2)->prox;
        }
        if(delete)
        {
            if(temp0 == NULL)
                lista1 = &(*temp1)->prox;
            else
                (*temp0)->prox = (*temp1)->prox;
            free(*temp1);
            temp1 = &(*temp0)->prox;
        }else{
            temp0 = temp1;
            temp1 = &(*temp1)->prox;
        }
    }
}

int main()
{
    int selecionado, reset = 1;

    char nome1[20], nome2[20], ibge[8], tempstr[20];
    int uf1, uf2, ddd1, ddd2;
    float lat1, lat2, long1, long2;
    titem *resultado = NULL, *temp = NULL;

    char* file = readFile("./municipios.json");
    municipio* arr = makeArray(file), *tempMunicipio;

    tnode* nomes = NULL;
    tnode* latitudes = NULL;
    tnode* longitudes = NULL;
    tnode* ufs = NULL;
    tnode* ddds = NULL;

    thash h;
    hash_constroi(&h, 8192, getkeyIBGE);

    for(int i = 0; i < 5570; i++)
    {
        avl_insere(&nomes, NULL, getkeystr(arr[i].nome), &arr[i]);
        avl_insere(&latitudes, NULL, getkeyfloat(arr[i].coordenadas[0]), &arr[i]);
        avl_insere(&longitudes, NULL, getkeyfloat(arr[i].coordenadas[1]), &arr[i]);
        avl_insere(&ufs, NULL, arr[i].codigo_uf, &arr[i]);
        avl_insere(&ddds, NULL, arr[i].ddd, &arr[i]);
        hash_insere(&h, &arr[i]);
    }

    while(1)
    {
        if(reset)
        {
            temp = NULL;
            resultado = NULL;
            uf1 = INT32_MIN, uf2 = INT32_MAX, ddd1 = INT32_MIN, ddd2 = INT32_MAX;
            lat1 = -__FLT_MAX__, lat2 = __FLT_MAX__, long1 = -__FLT_MAX__, long2 = __FLT_MAX__;
            strcpy(nome1, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
            strcpy(nome2, "ZZZZZZZZZZZZZZZZZZZ");
        }
        selecionado = -1, reset = 0;

        system("cls");
        printf("selecione os parametros para a busca:\n0 - Achar por código IBGE\n1 - nome\n2 - latitude\n3 - longitude\n4 - codigo uf\n5 - ddd\n6 - pesquisar\nQualquer outra entrada para sair\n");

        scanf("%d", &selecionado);

        switch (selecionado)
        {
        case 0:
            system("cls");
            printf("digite o código IBGE: ");
            scanf("%s", ibge);
            tempMunicipio = (municipio*)hash_busca(h, ibge);
            system("cls");
            printMunicipio(tempMunicipio);
            free(tempMunicipio);
            tempMunicipio = NULL;
            printf("Aperte enter para continuar\n");
            fflush(stdin);
            getchar();
            break;
        case 1:
            system("cls");
            printf("digite o nome a pesquisar, digite min para o valor minimo:\n");
            scanf(" %[^\n]s", nome1);

            if(!strcmp(nome1, "min")) strcpy(nome1, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

            printf("digite o nome a pesquisar, digite max para o valor máximo ou digite o mesmo nome para buscar somente pelo nome 1:\n");
            scanf(" %[^\n]s", nome2);

            if(!strcmp(nome2, "max")) strcpy(nome2, "ZZZZZZZZZZZZZZZZZZZ");
            break;
        case 2:
            system("cls");
            printf("digite a latitude a pesquisar, digite min para o valor minimo:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "min")) lat1 = -__FLT_MAX__;
            else lat1 = atof(tempstr);

            printf("digite a latitude a pesquisar, digite max para o valor máximo ou deixe em branco para buscar somente pela latitude 1:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "max")) lat2 = __FLT_MAX__;
            else lat2 = atof(tempstr);
            

            break;
        case 3:
            system("cls");
            printf("digite a longitude a pesquisar, digite min para o valor minimo:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "min")) long1 = -__FLT_MAX__;
            else long1 = atof(tempstr);

            printf("digite a longitude a pesquisar, digite max para o valor máximo ou deixe em branco para buscar somente pela longitude 1:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "max")) long2 = __FLT_MAX__;
            else long2 = atof(tempstr);

            break;
        case 4:
            system("cls");
            printf("digite o codigo uf a pesquisar, digite min para o valor minimo:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "min")) uf1 = INT32_MIN;
            else uf1 = atoi(tempstr);

            printf("digite o codigo uf a pesquisar, digite max para o valor máximo ou deixe em branco para buscar somente pelo codigo uf 1:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "max")) uf2 = INT32_MAX;
            else uf2 = atoi(tempstr);

            break;
        case 5:
            system("cls");
            printf("digite o ddd a pesquisar, digite min para o valor minimo:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "min")) ddd1 = INT32_MIN;
            else ddd1 = atoi(tempstr);

            printf("digite o ddd a pesquisar, digite max para o valor máximo ou deixe em branco para buscar somente pelo ddd 1:\n");
            scanf(" %s", tempstr);

            if(!strcmp(tempstr, "max")) ddd2 = INT32_MAX;
            else ddd2 = atoi(tempstr);

            break;
        case 6:
            reset = 1;
            if(strcmp(nome1, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") || strcmp(nome2, "ZZZZZZZZZZZZZZZZZZZ"))
            {
                avl_busca(nomes, getkeystr(nome1),getkeystr(nome2), &temp);
                comparaLista(&resultado, &temp);
            }
            if(lat1 != -__FLT_MAX__ && lat2 != __FLT_MAX__)
            {
                avl_busca(latitudes, getkeyfloat(lat1), getkeyfloat(lat2), &temp);
                comparaLista(&resultado, &temp);
            }
            if(long1 != -__FLT_MAX__ && long2 != __FLT_MAX__)
            {
                avl_busca(longitudes, getkeyfloat(long1), getkeyfloat(long2), &temp);
                comparaLista(&resultado, &temp);
            }
            if(uf1 != INT32_MIN && uf2 != INT32_MAX)
            {
                avl_busca(ufs, uf1, uf2, &temp);
                comparaLista(&resultado, &temp);
            }
            if(ddd1 != INT32_MIN && ddd2 != INT32_MAX)
            {
                avl_busca(ddds, ddd1, ddd2, &temp);
                comparaLista(&resultado, &temp);
            }

            if(resultado == NULL) avl_busca(ddds, INT32_MIN, INT32_MAX, &resultado);

            temp = resultado;
            while(temp != NULL)
            {
                printMunicipio(temp->municipio);
                temp = temp->prox;
            }
            freeList(&temp);
            freeList(&resultado);
           

            printf("Aperte enter para continuar\n");
            fflush(stdin);
            getchar();
            break;
        default:
            exit(0);
            break;
        }
        
    }

}