#ifndef TESTE_HASH
#define TESTE_HASH

typedef struct{
    char nome[30];
    char  cpf[11];
}taluno;

char * get_key(void * reg);
void * aloca_aluno(char * nome, char * cpf);

void test_insere();
void test_search();
void test_remove();

#endif