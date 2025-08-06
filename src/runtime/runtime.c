#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runtime.h"

vec vec_create(int n){
    vec v; v.n=n; v.data=calloc(n,sizeof(int)); return v;
}

void vec_free(vec v){ free(v.data); }

vec vec_from_array(int n, int *arr){
    vec v = vec_create(n);
    memcpy(v.data, arr, n*sizeof(int));
    return v;
}


vec vec_binop(vec a, vec b, char op){
    vec r = vec_create(a.n);
    for(int i=0;i<a.n;i++){
        switch(op){
            case '+': r.data[i]=a.data[i]+b.data[i]; break;
            case '-': r.data[i]=a.data[i]-b.data[i]; break;
            case '*': r.data[i]=a.data[i]*b.data[i]; break;
            case '/': r.data[i]=a.data[i]/b.data[i]; break;
        }
    }
    return r;
}

vec vec_binop_scl(vec v, int s, char op){
    vec r = vec_create(v.n);
    for(int i=0;i<v.n;i++){
        switch(op){
            case '+': r.data[i]=v.data[i]+s; break;
            case '-': r.data[i]=v.data[i]-s; break;
            case '*': r.data[i]=v.data[i]*s; break;
            case '/': r.data[i]=v.data[i]/s; break;
        }
    }
    return r;
}

vec vec_binop_scl_rev(int s, vec v, char op){
    vec r = vec_create(v.n);
    for(int i=0;i<v.n;i++){
        switch(op){
            case '+': r.data[i]=s+v.data[i]; break;
            case '-': r.data[i]=s-v.data[i]; break;
            case '*': r.data[i]=s*v.data[i]; break;
            case '/': r.data[i]=s/v.data[i]; break;
        }
    }
    return r;
}

int vec_dot(vec a, vec b){
    int sum=0; for(int i=0;i<a.n;i++) sum+=a.data[i]*b.data[i]; return sum;
}

int vec_index(vec v, int i){
    if(i<0 || i>=v.n)
        return 0;
    return v.data[i];
}

vec vec_gather(vec v, vec idx){
    vec r = vec_create(idx.n);
    for(int i=0;i<idx.n;i++) r.data[i]=vec_index(v,idx.data[i]);
    return r;
}

const char* vec_repr(vec v){
    static char buf[1024];
    char *p=buf; *p++='[';
    for(int i=0;i<v.n;i++){
        if(i) *p++=',';
        p+=sprintf(p,"%d",v.data[i]);
    }
    *p++=']'; *p='\0';
    return buf;
}
