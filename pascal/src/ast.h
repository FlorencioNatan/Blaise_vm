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
	TAN_IGUAL,
	TAN_DIFERENTE,
	TAN_MAIOR,
	TAN_MAIOR_IGUAL,
	TAN_MENOR,
	TAN_MENOR_IGUAL,
	TAN_AND,
	TAN_OR,
	TAN_NOT,
	TAN_VARIAVEL,
	TAN_ATRIBUICAO,
	TAN_IF,
	TAN_WHILE,
	TAN_REPEAT,
	TAN_FORTO,
	TAN_FORDOWNTO,
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
ast_node_t* criarNoBinario(ast_node_t* lhs, ast_node_t* rhs, tipo_ast_node_t tipo);
ast_node_t* criarNoUnario(ast_node_t* op, tipo_ast_node_t tipo);
ast_node_t* criarNoVariavel(char* nomeVariavel);
ast_node_t* criarNoAtribuicao(char* lhs, ast_node_t* rhs);
ast_node_t* criarNoIf(ast_node_t* condicao, lista_t* then_stmt, lista_t* else_stmt);
ast_node_t* criarNoWhile(ast_node_t* condicao, lista_t* codigo);
ast_node_t* criarNoRepeat(ast_node_t* condicao, lista_t* codigo);
ast_node_t* criarNoForTo(ast_node_t* inicializacao, ast_node_t* ate, lista_t* codigo);
ast_node_t* criarNoForDownTo(ast_node_t* inicializacao, ast_node_t* ate, lista_t* codigo);

void printAST(programa_t* programa);

#endif /* AST_H */
