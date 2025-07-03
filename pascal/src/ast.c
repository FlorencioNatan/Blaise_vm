#include <string.h>
#include "ast.h"
#include "stdlib.h"
#include "variavel.h"

mapa_t *adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos) {
	while (variaveis != NULL) {
		if (variaveis->tipo != TC_STRING) {
			continue;
		}

		variavel_t* variavel = malloc(sizeof(variavel_t));
		variavel->nome = malloc(sizeof(char) * strlen(variaveis->valor.strVal) + 1);
		strcpy(variavel->nome, variaveis->valor.strVal);
		variavel->tipo = tipo;

		switch (tipo) {
			case TIPO_PRIMITIVO_INTEGER:
				variavel->valor.iVal = 0;
				break;
			case TIPO_PRIMITIVO_STRING:
				variavel->valor.strVal = malloc(sizeof(char));
				variavel->valor.strVal = "";
				break;
			case TIPO_PRIMITIVO_CHAR:
				variavel->valor.chVal = ' ';
				break;
			case TIPO_PRIMITIVO_BOOLEAN:
				variavel->valor.boolVal = true;
				break;
			case TIPO_PRIMITIVO_REAL:
				variavel->valor.realVal = 0.0;
				break;
		}

		tabela_simbolos = addVariavelNoMapa(variavel->nome, variavel, tabela_simbolos);
		variaveis = caudaDaLista(variaveis);
	}

	return tabela_simbolos;
}

