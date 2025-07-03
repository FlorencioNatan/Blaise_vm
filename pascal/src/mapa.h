#ifndef MAPA_H
#define MAPA_H

#include "tipos_colecao.h"

#define COR_PRETO 0
#define COR_VERMELHO 1

typedef struct mapa mapa_t;
typedef struct no no_t;

struct mapa {
	no_t* raiz;
	no_t* nil;
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
void removerChaveDoMapa(char *chave, mapa_t *mapa);
void printMapa(mapa_t *mapa);


#endif /* MAPA_H */
