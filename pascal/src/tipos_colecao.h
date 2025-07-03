#ifndef TIPOS_COLECAO_H
#define TIPOS_COLECAO_H
#include "variavel.h"

typedef union {
	char* strVal;
	int iVal;
	variavel_t *varVal;
} valores_colecao;

typedef enum {
	TC_INT,
	TC_STRING,
	TC_VARIAVEL
} tipos_colecao;

#endif /* TIPOS_COLECAO_H */
