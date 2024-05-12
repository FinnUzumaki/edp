#ifndef __MUNICIPIO__
#define __MUNICIPIO__

typedef struct {
    int codigo_ibge;
    char* nome;
    float latitude;
    float longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char* fuso_horario;
}municipio;

#endif