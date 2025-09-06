#include <string.h>
#include "ast.h"
#include <stdlib.h>
#include <graphviz/gvc.h>

mapa_t* adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos) {
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

programa_t* criarNoPrograma(char* nome, mapa_t *tabela_simbolos, lista_t *filhos) {
	programa_t* programa = malloc(sizeof(programa_t));
	programa->nome = malloc(sizeof(char) * strlen(nome) + 1);
	strcpy(programa->nome, nome);
	programa->tabela_simbolos = tabela_simbolos;
	programa->filhos = filhos;

	return programa;
}

ast_node_t* criarNoReal(double valor) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_REAL;
	no->valor.dVal = valor;
	return no;
}

ast_node_t* criarNoInteger(int valor) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_INTEGER;
	no->valor.iVal = valor;
	return no;
}

ast_node_t* criarNoSoma(ast_node_t* lhs, ast_node_t* rhs) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_SOMA;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(lhs, no->filhos);
	return no;
}

ast_node_t* criarNoSubtracao(ast_node_t* lhs, ast_node_t* rhs) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_SUBTRACAO;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(lhs, no->filhos);
	return no;
}

ast_node_t* criarNoMultiplicacao(ast_node_t* lhs, ast_node_t* rhs) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_MULTIPLICACAO;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(lhs, no->filhos);
	return no;
}

ast_node_t* criarNoDivisao(ast_node_t* lhs, ast_node_t* rhs) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_DIVISAO;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(lhs, no->filhos);
	return no;
}

ast_node_t* criarNoNegativar(ast_node_t* op) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_NEGATIVACAO;
	no->filhos = addNoASTNaLista(op, NULL);
	return no;
}

ast_node_t* criarNoBinario(ast_node_t* lhs, ast_node_t* rhs, tipo_ast_node_t tipo) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = tipo;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(lhs, no->filhos);
	return no;
}

ast_node_t* criarNoUnario(ast_node_t* op, tipo_ast_node_t tipo) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = tipo;
	no->filhos = addNoASTNaLista(op, NULL);
	return no;
}

void printNoAST(ast_node_t* noAST, GVC_t *gvc, Agraph_t *g, Agnode_t *p) {
	if (noAST == NULL) {
		return;
	}
	char descricaoNoAST[50];
	switch (noAST->tipo) {
	case TAN_PROGRAMA:
		strcpy(descricaoNoAST, "PROGRAMA");
		break;
	case TAN_REAL:
		sprintf(descricaoNoAST, "REAL: %f", noAST->valor.dVal);
		break;
	case TAN_INTEGER:
		sprintf(descricaoNoAST, "INTEGER: %d", noAST->valor.iVal);
		break;
	case TAN_SOMA:
		strcpy(descricaoNoAST, "SOMA");
		break;
	case TAN_SUBTRACAO:
		strcpy(descricaoNoAST, "SUBTRACAO");
		break;
	case TAN_MULTIPLICACAO:
		strcpy(descricaoNoAST, "MULTIPLICACAO");
		break;
	case TAN_DIVISAO:
		strcpy(descricaoNoAST, "DIVISAO");
		break;
	case TAN_NEGATIVACAO:
		strcpy(descricaoNoAST, "NEGATIVO");
		break;
	case TAN_IGUAL:
		strcpy(descricaoNoAST, "IGUAL");
		break;
	case TAN_DIFERENTE:
		strcpy(descricaoNoAST, "DIFERENTE");
		break;
	case TAN_MAIOR:
		strcpy(descricaoNoAST, "MAIOR");
		break;
	case TAN_MAIOR_IGUAL:
		strcpy(descricaoNoAST, "MAIOR OU IGUAL");
		break;
	case TAN_MENOR:
		strcpy(descricaoNoAST, "MENOR");
		break;
	case TAN_MENOR_IGUAL:
		strcpy(descricaoNoAST, "MENOR OU IGUAL");
		break;
	case TAN_AND:
		strcpy(descricaoNoAST, "AND");
		break;
	case TAN_OR:
		strcpy(descricaoNoAST, "OR");
		break;
	case TAN_NOT:
		strcpy(descricaoNoAST, "NOT");
		break;
	}
	Agnode_t *f = agnode(g, descricaoNoAST, 1);
	if (p != NULL) {
		(void)agedge(g, p, f, 0, 1);
	}

	lista_t* filhos = noAST->filhos;
	while (filhos != NULL) {
		ast_node_t* valor = filhos->valor.astNode;
		printNoAST(valor, gvc, g, f);
		filhos = caudaDaLista(filhos);
	}
}

void printAST(programa_t* programa) {
	GVC_t *gvc = gvContext();
	Agraph_t *g = agopen("g", Agdirected, 0);
	lista_t* filhos = programa->filhos;

	char programString[255];
	sprintf(programString, "Programa: %s", programa->nome);
	Agnode_t *p = agnode(g, programString, 1);

	while (filhos != NULL) {
		ast_node_t* valor = filhos->valor.astNode;
		printNoAST(valor, gvc, g, p);
		filhos = caudaDaLista(filhos);
	}

#ifdef NO_LAYOUT_OR_RENDERING
  // Just write the graph without layout
  agwrite(g, stdout);
#else
  // Use the directed graph layout engine
  gvLayout(gvc, g, "dot");

  // Output in .dot format
  gvRender(gvc, g, "png", stdout);

  gvFreeLayout(gvc, g);
#endif

  agclose(g);
}
