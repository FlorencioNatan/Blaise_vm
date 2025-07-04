#ifndef AST_H
#define AST_H

#include "lista.h"
#include "mapa.h"

#define TIPO_PRIMITIVO_INTEGER 0
#define TIPO_PRIMITIVO_STRING 1
#define TIPO_PRIMITIVO_CHAR 2
#define TIPO_PRIMITIVO_BOOLEAN 3
#define TIPO_PRIMITIVO_REAL 4

typedef union {
	int iVal;
	char* strVal;
	char chVal;
	int boolVal;
	double realVal;
} valor_primitivo;

typedef enum {
	TAN_PROGRAMA
} tipo_ast_node_t;

typedef struct programa {
    char* nome;
    mapa_t *tabela_simbolos;
} programa_t;

typedef union {
	char* strVal;
	int iVal;
	programa_t *varVal;
} valores_ast_node;

typedef struct variavel {
    char* nome;
    int tipo;
    valor_primitivo valor;
} variavel_t;

typedef struct node {
	tipo_ast_node_t tipo;
	valores_ast_node *valor;
	lista_t *filhos;
} ast_node_t;


mapa_t *adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos);

#endif /* AST_H */
