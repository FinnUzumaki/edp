#ifndef __KDTREE__
#define __KDTREE__

#include "municipio.h"

typedef struct tKdtree
{
    municipio* node;
    struct tKdtree* left;
    struct tKdtree* right;
    int d;
}kdtree;

kdtree* createKdtree(municipio* m);
void insertKdtree(kdtree** tree, municipio* node, int d);
municipio** nearestnKdtree(kdtree* tree, municipio* node, int n);

#endif