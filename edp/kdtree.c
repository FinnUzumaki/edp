#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "kdtree.h"
#include <malloc.h>

struct kdhyperrect {
	int dim;
	double *min, *max;              /* minimum/maximum coords */
};

struct kdnode {
	double *pos;
	int dir;
	void *data;

	struct kdnode *left, *right;	/* negative/positive side */
};

struct res_node {
	struct kdnode *item;
	double dist_sq;
	struct res_node *next;
};

struct kdtree {
	int dim;
	struct kdnode *root;
	struct kdhyperrect *rect;
	void (*destr)(void*);
};

struct kdres {
	struct kdtree *tree;
	struct res_node *rlist, *riter;
	int size;
};

#define SQ(x)			((x) * (x))


static void clear_rec(struct kdnode *node, void (*destr)(void*));
static int insert_rec(struct kdnode **node, const double *pos, void *data, int dir, int dim);
static int rlist_insert(struct res_node *list, struct kdnode *item, double dist_sq);
static void clear_results(struct kdres *set);

static struct kdhyperrect* hyperrect_create(int dim, const double *min, const double *max);
static void hyperrect_free(struct kdhyperrect *rect);
static struct kdhyperrect* hyperrect_duplicate(const struct kdhyperrect *rect);
static void hyperrect_extend(struct kdhyperrect *rect, const double *pos);
static double hyperrect_dist_sq(struct kdhyperrect *rect, const double *pos);

#define alloc_resnode()		malloc(sizeof(struct res_node))
#define free_resnode(n)		free(n)

struct kdtree *kd_create(int k)
{
	struct kdtree *tree;

	if(!(tree = malloc(sizeof *tree))) {
		return 0;
	}

	tree->dim = k;
	tree->root = 0;
	tree->destr = 0;
	tree->rect = 0;

	return tree;
}

void kd_free(struct kdtree *tree)
{
	if(tree) {
		kd_clear(tree);
		free(tree);
	}
}

static void clear_rec(struct kdnode *node, void (*destr)(void*))
{
	if(!node) return;

	clear_rec(node->left, destr);
	clear_rec(node->right, destr);
	
	if(destr) {
		destr(node->data);
	}
	free(node->pos);
	free(node);
}

void kd_clear(struct kdtree *tree)
{
	clear_rec(tree->root, tree->destr);
	tree->root = 0;

	if (tree->rect) {
		hyperrect_free(tree->rect);
		tree->rect = 0;
	}
}

void kd_data_destructor(struct kdtree *tree, void (*destr)(void*))
{
	tree->destr = destr;
}


static int insert_rec(struct kdnode **nptr, const double *pos, void *data, int dir, int dim)
{
	int new_dir;
	struct kdnode *node;

	if(!*nptr) {
		if(!(node = malloc(sizeof *node))) {
			return -1;
		}
		if(!(node->pos = malloc(dim * sizeof *node->pos))) {
			free(node);
			return -1;
		}
		memcpy(node->pos, pos, dim * sizeof *node->pos);
		node->data = data;
		node->dir = dir;
		node->left = node->right = 0;
		*nptr = node;
		return 0;
	}

	node = *nptr;
	new_dir = (node->dir + 1) % dim;
	if(pos[node->dir] < node->pos[node->dir]) {
		return insert_rec(&(*nptr)->left, pos, data, new_dir, dim);
	}
	return insert_rec(&(*nptr)->right, pos, data, new_dir, dim);
}

int kd_insert(struct kdtree *tree, const double *pos, void *data)
{
	if (insert_rec(&tree->root, pos, data, 0, tree->dim)) {
		return -1;
	}

	if (tree->rect == 0) {
		tree->rect = hyperrect_create(tree->dim, pos, pos);
	} else {
		hyperrect_extend(tree->rect, pos);
	}

	return 0;
}

int kd_insertf(struct kdtree *tree, const float *pos, void *data)
{
	static double sbuf[16];
	double *bptr, *buf = 0;
	int res, dim = tree->dim;

	if(dim > 16) {
#ifndef NO_ALLOCA
		if(dim <= 256)
			bptr = buf = alloca(dim * sizeof *bptr);
		else
#endif
			if(!(bptr = buf = malloc(dim * sizeof *bptr))) {
				return -1;
			}
	} else {
		bptr = buf = sbuf;
	}

	while(dim-- > 0) {
		*bptr++ = *pos++;
	}

	res = kd_insert(tree, buf, data);
	if(tree->dim > 256)
		free(buf);
	return res;
}

static int find_nearest(struct kdnode *node, const double *pos, double range, struct res_node *list, int ordered, int dim)
{
	double dist_sq, dx;
	int i, ret, added_res = 0;

	if(!node) return 0;

	dist_sq = 0;
	for(i=0; i<dim; i++) {
		dist_sq += SQ(node->pos[i] - pos[i]);
	}
	if(dist_sq <= SQ(range)) {
		if(rlist_insert(list, node, ordered ? dist_sq : -1.0) == -1) {
			return -1;
		}
		added_res = 1;
	}

	dx = pos[node->dir] - node->pos[node->dir];

	ret = find_nearest(dx <= 0.0 ? node->left : node->right, pos, range, list, ordered, dim);
	if(ret >= 0 && fabs(dx) < range) {
		added_res += ret;
		ret = find_nearest(dx <= 0.0 ? node->right : node->left, pos, range, list, ordered, dim);
	}
	if(ret == -1) {
		return -1;
	}
	added_res += ret;

	return added_res;
}

static struct res_node *rlist_pop_back(struct res_node *list)
{
	struct res_node *previous = 0;
	while (list->next) {
		previous = list;
		list = list->next;
	}
	if (previous) {
		previous->next = 0;
	}
	free_resnode(list);
	return previous;
}

static int find_nearest_n(struct kdnode *node, const double *pos, int num, int *size, double *dist_max, struct res_node *list, int dim)
{
	double dist_sq, dx;
	int i, ret;

	if(!node) return 0;

	dist_sq = 0;
	for(i=0; i<dim; i++) {
		dist_sq += SQ(node->pos[i] - pos[i]);
	}

	if(dist_sq < *dist_max) {
		if(*size < num) {
			++(*size);
		} else {
			struct res_node *back = rlist_pop_back(list);
			if (back == 0)
				return -1;
			*dist_max = back->dist_sq > dist_sq ? back->dist_sq : dist_sq;
		}
		if (rlist_insert(list, node, dist_sq) == -1) {
			return -1;
		}
	}

	/* find signed distance from the splitting plane */
	dx = pos[node->dir] - node->pos[node->dir];

	ret = find_nearest_n(dx <= 0.0 ? node->left : node->right, pos, num, size, dist_max, list, dim);
	if(ret >= 0 && SQ(dx) < *dist_max) {
		ret = find_nearest_n(dx <= 0.0 ? node->right : node->left, pos, num, size, dist_max, list, dim);
	}
	return ret;
}

struct kdres *kd_nearest_n(struct kdtree *kd, const double *pos, int num)
{
	int ret, size = 0;
	struct kdres *rset;
	double dist_max = DBL_MAX;

	if(!(rset = malloc(sizeof *rset))) {
		return 0;
	}
	if(!(rset->rlist = alloc_resnode())) {
		free(rset);
		return 0;
	}
	rset->rlist->next = 0;
	rset->tree = kd;

	ret = find_nearest_n(kd->root, pos, num, &size, &dist_max, rset->rlist, kd->dim);
	if(ret == -1) {
		kd_res_free(rset);
		return 0;
	}
	rset->size = size;
	kd_res_rewind(rset);
	return rset;
}

struct kdres *kd_nearest_nf(struct kdtree *tree, const float *pos, int num)
{
	static double sbuf[16];
	double *bptr, *buf = 0;
	int dim = tree->dim;
	struct kdres *res;

	if (dim > 16) {
#ifndef NO_ALLOCA
		if (dim <= 256)
			bptr = buf = alloca(dim * sizeof *bptr);
		else
#endif
		    if (!(bptr = buf = malloc(dim * sizeof *bptr))) {
			return 0;
		}
	} else {
		bptr = buf = sbuf;
	}

	while (dim-- > 0) {
		*bptr++ = *pos++;
	}

	res = kd_nearest_n(tree, buf, num);
	if (tree->dim > 256)
		free(buf);
	return res;
}

struct kdres *kd_nearest_n3(struct kdtree *tree, double x, double y, double z, int num)
{
	double pos[3];
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	return kd_nearest_n(tree, pos, num);
}

struct kdres *kd_nearest_n3f(struct kdtree *tree, float x, float y, float z, int num)
{
	double pos[3];
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	return kd_nearest_n(tree, pos, num);
}

void kd_res_free(struct kdres *rset)
{
	clear_results(rset);
	free_resnode(rset->rlist);
	free(rset);
}

int kd_res_size(struct kdres *set)
{
	return (set->size);
}

void kd_res_rewind(struct kdres *rset)
{
	rset->riter = rset->rlist->next;
}

int kd_res_end(struct kdres *rset)
{
	return rset->riter == 0;
}

int kd_res_next(struct kdres *rset)
{
	rset->riter = rset->riter->next;
	return rset->riter != 0;
}

void *kd_res_item(struct kdres *rset, double *pos)
{
	if(rset->riter) {
		if(pos) {
			memcpy(pos, rset->riter->item->pos, rset->tree->dim * sizeof *pos);
		}
		return rset->riter->item->data;
	}
	return 0;
}

void *kd_res_itemf(struct kdres *rset, float *pos)
{
	if(rset->riter) {
		if(pos) {
			int i;
			for(i=0; i<rset->tree->dim; i++) {
				pos[i] = rset->riter->item->pos[i];
			}
		}
		return rset->riter->item->data;
	}
	return 0;
}

void *kd_res_item3(struct kdres *rset, double *x, double *y, double *z)
{
	if(rset->riter) {
		if(x) *x = rset->riter->item->pos[0];
		if(y) *y = rset->riter->item->pos[1];
		if(z) *z = rset->riter->item->pos[2];
		return rset->riter->item->data;
	}
	return 0;
}

void *kd_res_item3f(struct kdres *rset, float *x, float *y, float *z)
{
	if(rset->riter) {
		if(x) *x = rset->riter->item->pos[0];
		if(y) *y = rset->riter->item->pos[1];
		if(z) *z = rset->riter->item->pos[2];
		return rset->riter->item->data;
	}
	return 0;
}

void *kd_res_item_data(struct kdres *set)
{
	return kd_res_item(set, 0);
}

/* ---- hyperrectangle helpers ---- */
static struct kdhyperrect* hyperrect_create(int dim, const double *min, const double *max)
{
	size_t size = dim * sizeof(double);
	struct kdhyperrect* rect = 0;

	if (!(rect = malloc(sizeof(struct kdhyperrect)))) {
		return 0;
	}

	rect->dim = dim;
	if (!(rect->min = malloc(size))) {
		free(rect);
		return 0;
	}
	if (!(rect->max = malloc(size))) {
		free(rect->min);
		free(rect);
		return 0;
	}
	memcpy(rect->min, min, size);
	memcpy(rect->max, max, size);

	return rect;
}

static void hyperrect_free(struct kdhyperrect *rect)
{
	free(rect->min);
	free(rect->max);
	free(rect);
}

static struct kdhyperrect* hyperrect_duplicate(const struct kdhyperrect *rect)
{
	return hyperrect_create(rect->dim, rect->min, rect->max);
}

static void hyperrect_extend(struct kdhyperrect *rect, const double *pos)
{
	int i;

	for (i=0; i < rect->dim; i++) {
		if (pos[i] < rect->min[i]) {
			rect->min[i] = pos[i];
		}
		if (pos[i] > rect->max[i]) {
			rect->max[i] = pos[i];
		}
	}
}

static double hyperrect_dist_sq(struct kdhyperrect *rect, const double *pos)
{
	int i;
	double result = 0;

	for (i=0; i < rect->dim; i++) {
		if (pos[i] < rect->min[i]) {
			result += SQ(rect->min[i] - pos[i]);
		} else if (pos[i] > rect->max[i]) {
			result += SQ(rect->max[i] - pos[i]);
		}
	}

	return result;
}


/* inserts the item. if dist_sq is >= 0, then do an ordered insert */
/* TODO make the ordering code use heapsort */
static int rlist_insert(struct res_node *list, struct kdnode *item, double dist_sq)
{
	struct res_node *rnode;

	if(!(rnode = alloc_resnode())) {
		return -1;
	}
	rnode->item = item;
	rnode->dist_sq = dist_sq;

	if(dist_sq >= 0.0) {
		while(list->next && list->next->dist_sq < dist_sq) {
			list = list->next;
		}
	}
	rnode->next = list->next;
	list->next = rnode;
	return 0;
}

static void clear_results(struct kdres *rset)
{
	struct res_node *tmp, *node = rset->rlist->next;

	while(node) {
		tmp = node;
		node = node->next;
		free_resnode(tmp);
	}

	rset->rlist->next = 0;
}
