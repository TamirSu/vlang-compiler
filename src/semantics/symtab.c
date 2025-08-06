#include <stdlib.h>
#include <string.h>
#include "symtab.h"

static char* dupstr(const char* s){ char* d=malloc(strlen(s)+1); strcpy(d,s); return d; }

SymTab *symtab_create(void){
    SymTab *st = malloc(sizeof(SymTab));
    st->scopes = NULL;
    return st;
}

void symtab_push(SymTab *st){
    Scope *sc = malloc(sizeof(Scope));
    sc->symbols = NULL;
    sc->next = st->scopes;
    st->scopes = sc;
}

void symtab_pop(SymTab *st){
    if(!st->scopes) return;
    Scope *sc = st->scopes;
    Sym *sym = sc->symbols;
    while(sym){ Sym *tmp = sym->next; free(sym->name); free(sym); sym = tmp; }
    st->scopes = sc->next;
    free(sc);
}

static Sym *find_in_scope(Scope *sc, const char *name){
    for(Sym *s=sc->symbols; s; s=s->next){
        if(strcmp(s->name,name)==0) return s;
    }
    return NULL;
}

int symtab_define(SymTab *st, const char *name, Type type){
    if(!st->scopes) symtab_push(st);
    if(find_in_scope(st->scopes,name)) return -1;
    Sym *s = malloc(sizeof(Sym));
    s->name = dupstr(name);
    s->type = type;
    s->next = st->scopes->symbols;
    st->scopes->symbols = s;
    return 0;
}

int symtab_lookup(SymTab *st, const char *name, Type *out){
    for(Scope *sc=st->scopes; sc; sc=sc->next){
        Sym *s = find_in_scope(sc,name);
        if(s){ if(out) *out = s->type; return 0; }
    }
    return -1;
}

void symtab_free(SymTab *st){
    while(st->scopes) symtab_pop(st);
    free(st);
}
