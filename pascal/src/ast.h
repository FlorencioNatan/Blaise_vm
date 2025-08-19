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
	TAN_PROGRAMA,
	TAN_REAL,
	TAN_INTEGER,
	TAN_SOMA,
	TAN_SUBTRACAO,
	TAN_MULTIPLICACAO,
	TAN_DIVISAO,
	TAN_NEGATIVACAO,
} tipo_ast_node_t;

typedef struct programa {
    char* nome;
    mapa_t *tabela_simbolos;
    lista_t *filhos;
} programa_t;

typedef union {
	char* strVal;
	int iVal;
	double dVal;
	programa_t *varVal;
} valores_ast_node;

typedef struct variavel {
    char* nome;
    int tipo;
    valor_primitivo valor;
} variavel_t;

typedef struct node {
	tipo_ast_node_t tipo;
	valores_ast_node valor;
	lista_t *filhos;
} ast_node_t;


mapa_t* adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos);
programa_t* criarNoPrograma(char* nome, mapa_t *tabela_simbolos, lista_t *filhos);
ast_node_t* criarNoReal(double valor);
ast_node_t* criarNoInteger(int valor);
ast_node_t* criarNoSoma(ast_node_t* lhs, ast_node_t* rhs);
ast_node_t* criarNoSubtracao(ast_node_t* lhs, ast_node_t* rhs);
ast_node_t* criarNoMultiplicacao(ast_node_t* lhs, ast_node_t* rhs);
ast_node_t* criarNoDivisao(ast_node_t* lhs, ast_node_t* rhs);
ast_node_t* criarNoNegativar(ast_node_t* op);
void printAST(programa_t* programa);

#endif /* AST_H */
