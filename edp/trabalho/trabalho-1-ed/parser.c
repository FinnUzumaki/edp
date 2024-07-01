#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "cJSON.h"

const char* readFile(const char* path)
{
    FILE* file = fopen(path,"r");
    assert(file != NULL);

    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    rewind(file);

    char* content = calloc(size + 1, 1);

    fread(content,1,size,file);

    fclose(file);

    return content;
}

municipio* makeArray(const char* str)
{
    cJSON* temp = cJSON_Parse(str);
    
    cJSON *item;
    cJSON *codigo_ibge;
    cJSON *nome;
    cJSON *latitude;
    cJSON *longitude;
    cJSON *capital;
    cJSON *codigo_uf;
    cJSON *siafi_id;
    cJSON *ddd;
    cJSON *fuso_horario;

    municipio *arr = (municipio*) malloc(sizeof(municipio)* 5570);

    for(int i = 0; i < 5570; i++)
    {
        item =  cJSON_GetArrayItem(temp, i);
        codigo_ibge = cJSON_GetObjectItem(item, "codigo_ibge");
        nome = cJSON_GetObjectItem(item, "nome");
        latitude = cJSON_GetObjectItem(item, "latitude");
        longitude = cJSON_GetObjectItem(item, "longitude");
        capital = cJSON_GetObjectItem(item, "capital");
        codigo_uf = cJSON_GetObjectItem(item, "codigo_uf");
        siafi_id = cJSON_GetObjectItem(item, "siafi_id");
        ddd = cJSON_GetObjectItem(item, "ddd");
        fuso_horario = cJSON_GetObjectItem(item, "fuso_horario");

        arr[i].codigo_ibge = codigo_ibge->valueint;
        arr[i].nome = nome->valuestring;
        arr[i].coordenadas[0] = latitude->valuedouble;
        arr[i].coordenadas[1] = longitude->valuedouble;
        arr[i].capital = capital->valueint;
        arr[i].codigo_uf = codigo_uf->valueint;
        arr[i].siafi_id = siafi_id->valueint;
        arr[i].ddd = ddd->valueint;
        arr[i].fuso_horario = fuso_horario->valuestring;
    }

    return arr;
}