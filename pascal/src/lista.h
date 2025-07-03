#ifndef LISTA_H
#define LISTA_H

#include "stdbool.h"
#include "tipos_colecao.h"

typedef struct lista lista_t;


struct lista {
	lista_t* proximo;
	tipos_colecao tipo;
	valores_colecao valor;
};


lista_t* addStringNaLista(char *str, lista_t *lista);
lista_t* addIntNaLista(int val, lista_t *lista);
lista_t* cabecaDaLista(lista_t *lista);
lista_t* caudaDaLista(lista_t *lista);
bool temCaudaDaLista(lista_t *lista);

void printLista(lista_t *lista);


#endif /* LISTA_H */

