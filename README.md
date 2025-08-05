This project implements a toy compiler for Vlang that transpiles source files to ANSI C.
The pipeline includes a Flex lexer, a Bison parser producing an AST, semantic checks
(type/size validation for scl/vec, @ and : rules), and a C code generator with a tiny runtime
for vector helpers and printing. The repo also contains examples and end-to-end tests.
