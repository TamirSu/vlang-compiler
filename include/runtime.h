#ifndef RUNTIME_H
#define RUNTIME_H

typedef struct {
    int n;
    int *data;
} vec;

vec vec_create(int n);
void vec_free(vec v);
vec vec_from_array(int n, int *arr);
vec vec_binop(vec a, vec b, char op);
vec vec_binop_scl(vec v, int s, char op);
vec vec_binop_scl_rev(int s, vec v, char op);
int vec_dot(vec a, vec b);
int vec_index(vec v, int i);
vec vec_gather(vec v, vec idx);
const char* vec_repr(vec v);

#endif // RUNTIME_H
