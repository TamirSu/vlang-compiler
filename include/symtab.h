#ifndef SYMTAB_H
#define SYMTAB_H

#include "types.h"

typedef struct Sym {
    char *name;
    Type type;
    struct Sym *next;
} Sym;

typedef struct Scope {
    Sym *symbols;
    struct Scope *next;
} Scope;

typedef struct {
    Scope *scopes;
} SymTab;

SymTab *symtab_create(void);
void symtab_push(SymTab *st);
void symtab_pop(SymTab *st);
int symtab_define(SymTab *st, const char *name, Type type);
int symtab_lookup(SymTab *st, const char *name, Type *out);
void symtab_free(SymTab *st);

#endif // SYMTAB_H
