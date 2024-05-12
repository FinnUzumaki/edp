#ifndef _KDTREE_H_
#define _KDTREE_H_


struct kdtree;
struct kdres;

struct kdtree *kd_create(int k);

void kd_free(struct kdtree *tree);

void kd_clear(struct kdtree *tree);

void kd_data_destructor(struct kdtree *tree, void (*destr)(void*));

int kd_insert(struct kdtree *tree, const double *pos, void *data);
int kd_insertf(struct kdtree *tree, const float *pos, void *data);
int kd_insert3(struct kdtree *tree, double x, double y, double z, void *data);
int kd_insert3f(struct kdtree *tree, float x, float y, float z, void *data);

struct kdres *kd_nearest_n(struct kdtree *tree, const double *pos, int num);
struct kdres *kd_nearest_nf(struct kdtree *tree, const float *pos, int num);
struct kdres *kd_nearest_n3(struct kdtree *tree, double x, double y, double z, int num);
struct kdres *kd_nearest_n3f(struct kdtree *tree, float x, float y, float z, int num);

void kd_res_free(struct kdres *set);

int kd_res_size(struct kdres *set);

void kd_res_rewind(struct kdres *set);

int kd_res_end(struct kdres *set);

int kd_res_next(struct kdres *set);

void *kd_res_item(struct kdres *set, double *pos);
void *kd_res_itemf(struct kdres *set, float *pos);
void *kd_res_item3(struct kdres *set, double *x, double *y, double *z);
void *kd_res_item3f(struct kdres *set, float *x, float *y, float *z);

/* equivalent to kd_res_item(set, 0) */
void *kd_res_item_data(struct kdres *set);

#endif	/* _KDTREE_H_ */
