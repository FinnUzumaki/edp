#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "kdtree.h"

float distance(municipio* m1, municipio* m2)
{
	return pow((m1->coordenadas[0] - m2->coordenadas[0]), 2) + pow((m1->coordenadas[1] - m2->coordenadas[1]), 2);
}

kdtree* createKdtree(municipio* n)
{
	kdtree* temp = (kdtree*) malloc(sizeof(kdtree));
	temp->node = n;
	temp->left = NULL;
	temp->right = NULL;
}

void insertKdtree(kdtree** tree, municipio* node, int d)
{
	if((*tree) == NULL) (*tree) = createKdtree(node);
	else if((*tree)->node->coordenadas[d%2] > node->coordenadas[d%2])
	{
		insertKdtree(&(*tree)->left, node, d+1);
	}
	else
	{
		insertKdtree(&(*tree)->right, node, d+1);
	}
}

void _nearestnKdtree(kdtree* tree, municipio* node, municipio** array, int n)
{
	if(tree == NULL)
	{
		return;
	}

	for(int i = 0; i < n; i++)
	{
		if(tree->node == node) break;
		else if(array[i] == NULL)
		{
			array[i] = tree->node;
			break;
		}
		else
		{
			if(distance(node, tree->node) < distance(node, array[i]))
			{
				municipio *temp = tree->node, *aux;
				while(i < n-1)
				{
					aux = array[i];
					array[i] = temp;
					temp = aux;
					i++;
				}
				break;
			}
		}
	}

	_nearestnKdtree(tree->left, node, array, n);
	_nearestnKdtree(tree->right, node, array, n);
}

municipio** nearestnKdtree(kdtree* tree, municipio* node, int n)
{
	municipio** temp = (municipio**) malloc(sizeof(municipio*)*n);
	_nearestnKdtree(tree, node, temp, n);
	if(temp[0] == NULL) return NULL;
	return temp;
}