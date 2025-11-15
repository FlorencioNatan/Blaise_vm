#ifndef MAPA_H
#define MAPA_H

#include "tipos_colecao.h"
#include <stdbool.h>

#define COR_PRETO 0
#define COR_VERMELHO 1

typedef struct mapa mapa_t;
typedef struct no no_t;

struct mapa {
	no_t* raiz;
	no_t* nil;
	mapa_t *mapa_pai;
};

struct no {
	no_t* esquerda;
	no_t* direita;
	no_t* pai;
	char cor;
	tipos_colecao tipo;
	char* chave;
	valores_colecao valor;
};

mapa_t* addStringNoMapa(char *chave, char *valor, mapa_t *mapa);
mapa_t* addIntNoMapa(char *chave, int valor, mapa_t *mapa);
mapa_t* addVariavelNoMapa(char *chave, variavel_t *valor, mapa_t *mapa);
mapa_t* addProcedureNoMapa(char *chave, procedure_t *valor, mapa_t *mapa);
mapa_t* addFunctionNoMapa(char *chave, function_t *valor, mapa_t *mapa);
variavel_t* buscarVariavelNoMapa(char *chave, mapa_t *mapa);
function_t* buscarFunctionNoMapa(char *chave, mapa_t *mapa);
procedure_t* buscarProcedureNoMapa(char *chave, mapa_t *mapa);
bool contemChaveNoMapa(char *chave, mapa_t *mapa);

void removerChaveDoMapa(char *chave, mapa_t *mapa);
void printMapa(mapa_t *mapa);


#endif /* MAPA_H */
