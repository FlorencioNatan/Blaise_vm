#ifndef AST_H
#define AST_H

#include "lista.h"
#include "mapa.h"

typedef struct programa {
    char* nome;
    mapa_t *tabela_simbolos;
} programa_t;


mapa_t *adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos);

#endif /* AST_H */
