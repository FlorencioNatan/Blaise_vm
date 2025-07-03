#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lista.h"

lista_t* addStringNaLista(char *str, lista_t *lista) {
	lista_t *novoNo = malloc(sizeof(lista_t));
	novoNo->tipo = TC_STRING;
	novoNo->valor.strVal = malloc(sizeof(char) * (strlen(str) + 1));
	strcpy(novoNo->valor.strVal, str);
	novoNo->proximo = lista;
	return novoNo;
}

lista_t* addIntNaLista(int val, lista_t *lista) {
	lista_t *novoNo = malloc(sizeof(lista_t));
	novoNo->tipo = TC_INT;
	novoNo->valor.iVal = val;
	novoNo->proximo = lista;
	return novoNo;
}

void printLista(lista_t *lista) {
	while (lista != NULL) {
		if (lista->tipo == TC_STRING) {
			printf("%s -> ", lista->valor.strVal);
		} else if (lista->tipo == TC_STRING) {
			printf("%d -> ", lista->valor.iVal);
		}
		lista = lista->proximo;
	}
	printf("null\n");
}

lista_t* cabecaDaLista(lista_t *lista) {
	return lista;
}

lista_t* caudaDaLista(lista_t *lista) {
	return lista->proximo;
}

bool temCaudaNaLista(lista_t *lista) {
	if (lista->proximo != NULL) {
		return true;
	}
	return false;
}
