#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

int codegen_program(Program *prog, const char *src_path);

#endif // CODEGEN_H
