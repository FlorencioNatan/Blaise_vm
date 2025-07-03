#ifndef variavel_H
#define variavel_H

#include "valor_primitivo.h"


typedef struct {
    char* nome;
    int tipo;
    valor_primitivo valor;
} variavel_t;

#endif /* variavel_H */
