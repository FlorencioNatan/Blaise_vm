#ifndef AST_H
#define AST_H

#include "lista.h"
#include "mapa.h"

#define TIPO_PRIMITIVO_INTEGER 0
#define TIPO_PRIMITIVO_STRING 1
#define TIPO_PRIMITIVO_CHAR 2
#define TIPO_PRIMITIVO_BOOLEAN 3
#define TIPO_PRIMITIVO_REAL 4
#define TIPO_PRIMITIVO_NAO_PREENCHIDO -1

#define COMPRIMENTO_INTEGER_NA_MEMORIA 4
#define COMPRIMENTO_STRING_NA_MEMORIA 32
#define COMPRIMENTO_CHAR_NA_MEMORIA 1
#define COMPRIMENTO_BOOLEAN_NA_MEMORIA 1
#define COMPRIMENTO_REAL_NA_MEMORIA 8

// Memória usada para o compilador armazenar seus dados auxiliares
// É como se fossem 1024 registradores de um byte livres para o compilador fazer o que quiser
#define INICIO_MEMORIA_AUXILIAR 0
#define FIM_MEMORIA_AUXILIAR 1023
// Memória disponível para o programa
#define INICIO_MEMORIA_DISPONIVEL 1024
#define FIM_MEMORIA_DISPONIVEL 4998974
// Memória usada para se comunicar com as extensões da Máquina virtual
#define INICIO_MEMORIA_EXTENSAO 4998975
#define FIM_MEMORIA_EXTENSAO 4999999

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
	TAN_ASM,
	TAN_DECLARACAO_VAR,
} tipo_ast_node_t;

typedef struct programa {
    char* nome;
    mapa_t *tabela_simbolos;
    lista_t *variaveis;
    lista_t *filhos;
    int linha;
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
    int comprimentoNaMemoria;
    int posicaoNaMemoria;
    valor_primitivo valor;
} variavel_t;

typedef struct node {
	tipo_ast_node_t tipo;
	int tipo_dados;
	int linha;
	valores_ast_node valor;
	lista_t *filhos;
} ast_node_t;


mapa_t* adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos, int linha);
programa_t* criarNoPrograma(char* nome, lista_t *variaveis, lista_t *filhos, int linha);
ast_node_t* criarNoReal(double valor, int linha);
ast_node_t* criarNoInteger(int valor, int linha);
ast_node_t* criarNoBinario(ast_node_t* lhs, ast_node_t* rhs, tipo_ast_node_t tipo, int linha);
ast_node_t* criarNoUnario(ast_node_t* op, tipo_ast_node_t tipo, int linha);
ast_node_t* criarNoVariavel(char* nomeVariavel, int linha);
ast_node_t* criarNoAtribuicao(char* lhs, ast_node_t* rhs, int linha);
ast_node_t* criarNoIf(ast_node_t* condicao, lista_t* then_stmt, lista_t* else_stmt, int linha);
ast_node_t* criarNoWhile(ast_node_t* condicao, lista_t* codigo, int linha);
ast_node_t* criarNoRepeat(ast_node_t* condicao, lista_t* codigo, int linha);
ast_node_t* criarNoForTo(ast_node_t* inicializacao, ast_node_t* ate, lista_t* codigo, int linha);
ast_node_t* criarNoForDownTo(ast_node_t* inicializacao, ast_node_t* ate, lista_t* codigo, int linha);
ast_node_t* criarNoASM(char* asmStr, int linha);
ast_node_t* criarNoDeclaracaoVar(lista_t* vars, int tipo, int linha);
void criarTabelaDeSimbolos(programa_t *programa);
void verificarTiposDoPrograma(programa_t *programa);

void printAST(programa_t* programa);

#endif /* AST_H */
