#ifndef TYPES_H
#define TYPES_H

typedef enum {
    TYPE_SCL,
    TYPE_VEC
} TypeKind;

typedef struct {
    TypeKind kind;
    int size; // valid if kind == TYPE_VEC
} Type;

#endif // TYPES_H
