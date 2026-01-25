#ifndef VERIFICACAO_DE_TIPOS_H
#define VERIFICACAO_DE_TIPOS_H

#include "ast.h"
#include "lista.h"
#include "mapa.h"
#include <stdbool.h>

#define COMPRIMENTO_INTEGER_NA_MEMORIA 4
#define COMPRIMENTO_STRING_NA_MEMORIA 1
#define COMPRIMENTO_CHAR_NA_MEMORIA 1
#define COMPRIMENTO_BOOLEAN_NA_MEMORIA 1
#define COMPRIMENTO_REAL_NA_MEMORIA 8

typedef union {
	int iVal;
	char* strVal;
	char chVal;
	int boolVal;
	double realVal;
} valor_primitivo;

typedef struct variavel {
    char* nome;
    int tipo;
    bool eArray;
    int comprimentoNaMemoria;
    int posicaoNaMemoria;
    int inicioArray;
    int fimArray;
    valor_primitivo valor;
} variavel_t;

mapa_t* adicionaListaVariaveisPrimitivasNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos, int *posicaoMemoria, bool positivo, int linha);
mapa_t* adicionaListaVariaveisArrayNaTabelaDeSimbolos(ast_node_t* declaracao, mapa_t *tabela_simbolos, int *posicaoMemoria, bool positivo);
bool criarTabelaDeSimbolos(programa_t *programa);
bool verificarTiposDoPrograma(programa_t *programa);
variavel_t* buscaVariavel(char *chave, mapa_t *mapa, int linha);

#endif /* VERIFICACAO_DE_TIPOS_H */
