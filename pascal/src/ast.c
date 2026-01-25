#include <string.h>
#include "ast.h"
#include <stdlib.h>
#include <graphviz/gvc.h>

programa_t* criarNoPrograma(char* nome, lista_t *subrotinas, lista_t *variaveis, lista_t *filhos, int linha) {
	programa_t* programa = malloc(sizeof(programa_t));
	programa->nome = malloc(sizeof(char) * strlen(nome) + 1);
	strcpy(programa->nome, nome);
	programa->subrotinas = subrotinas;
	programa->variaveis = variaveis;
	programa->filhos = filhos;
	programa->linha = linha;
	programa->tabela_simbolos = NULL;

	return programa;
}

ast_node_t* criarNoProcedure(char* nome, lista_t *parametros, lista_t *variaveis, lista_t *filhos, int linha) {
	procedure_t* procedure = malloc(sizeof(programa_t));
	procedure->nome = malloc(sizeof(char) * strlen(nome) + 1);
	strcpy(procedure->nome, nome);

	procedure->parametros = parametros;
	procedure->variaveis = variaveis;
	procedure->filhos = filhos;
	procedure->tabela_simbolos = NULL;
	procedure->comprimento_memoria_stack = 0;
	procedure->comprimento_parametros_stack = 0;

	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_PROCEDURE;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->valor.proVal = procedure;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoFunction(char* nome, lista_t *parametros, lista_t *variaveis, lista_t *filhos, int tipo_retorno, int linha) {
	function_t* function = malloc(sizeof(function_t));
	function->nome = malloc(sizeof(char) * strlen(nome) + 1);
	strcpy(function->nome, nome);

	function->parametros = parametros;
	function->variaveis = variaveis;
	function->filhos = filhos;
	function->tabela_simbolos = NULL;
	function->tipo_retorno = tipo_retorno;
	function->comprimento_memoria_stack = 0;
	function->comprimento_parametros_stack = 0;

	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_FUNCTION;
	no->tipo_dados = tipo_retorno;
	no->valor.funVal = function;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoReal(double valor, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_REAL;
	no->tipo_dados = TIPO_PRIMITIVO_REAL;
	no->valor.dVal = valor;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoInteger(int valor, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_INTEGER;
	no->tipo_dados = TIPO_PRIMITIVO_INTEGER;
	no->valor.iVal = valor;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoChar(char valor, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_CHAR;
	no->tipo_dados = TIPO_PRIMITIVO_CHAR;
	no->valor.cVal = valor;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoBoolean(int valor, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_BOOLEAN;
	no->tipo_dados = TIPO_PRIMITIVO_BOOLEAN;
	no->valor.iVal = valor;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoBinario(ast_node_t* lhs, ast_node_t* rhs, tipo_ast_node_t tipo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = tipo;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(lhs, no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoUnario(ast_node_t* op, tipo_ast_node_t tipo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = tipo;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->filhos = addNoASTNaLista(op, NULL);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoVariavel(char* nomeVariavel, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_VARIAVEL;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->valor.strVal = malloc(sizeof(char) * (strlen(nomeVariavel) + 1));
	strcpy(no->valor.strVal, nomeVariavel);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoAtribuicao(char* lhs, ast_node_t* rhs, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_ATRIBUICAO;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->filhos = addNoASTNaLista(rhs, NULL);
	no->filhos = addNoASTNaLista(criarNoVariavel(lhs, linha), no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoIf(ast_node_t* condicao, lista_t* then_stmt, lista_t* else_stmt, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_IF;
	if (else_stmt != NULL) {
		no->filhos = addListaNaLista(else_stmt, NULL);
		no->filhos = addListaNaLista(then_stmt, no->filhos);
	} else {
		no->filhos = addListaNaLista(then_stmt, NULL);
	}
	no->filhos = addNoASTNaLista(condicao, no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoWhile(ast_node_t* condicao, lista_t* codigo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_WHILE;
	no->filhos = addListaNaLista(codigo, NULL);
	no->filhos = addNoASTNaLista(condicao, no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoRepeat(ast_node_t* condicao, lista_t* codigo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_REPEAT;
	no->filhos = addListaNaLista(codigo, NULL);
	no->filhos = addNoASTNaLista(condicao, no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoForTo(ast_node_t* inicializacao, ast_node_t* ate, lista_t* codigo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_FORTO;
	no->filhos = addListaNaLista(codigo, NULL);
	no->filhos = addNoASTNaLista(ate, no->filhos);
	no->filhos = addNoASTNaLista(inicializacao, no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoForDownTo(ast_node_t* inicializacao, ast_node_t* ate, lista_t* codigo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_FORDOWNTO;
	no->filhos = addListaNaLista(codigo, NULL);
	no->filhos = addNoASTNaLista(ate, no->filhos);
	no->filhos = addNoASTNaLista(inicializacao, no->filhos);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoASM(char* asmStr, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_ASM;
	no->valor.strVal = malloc(sizeof(char) * (strlen(asmStr) + 1));
	strcpy(no->valor.strVal, asmStr);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoListaVar(lista_t* vars, int tipo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_LISTA_VAR;
	no->filhos = addListaNaLista(vars, NULL);
	no->valor.iVal = tipo;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoDeclaracaoArrayVar(lista_t* vars, ast_node_t *tipoArray, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_LISTA_VAR;

	int inicioArray = tipoArray->filhos->valor.iVal;
	int fimArray = caudaDaLista(tipoArray->filhos)->valor.iVal;

	no->filhos = addIntNaLista(fimArray, NULL);
	no->filhos = addIntNaLista(inicioArray, no->filhos);
	no->filhos = addIntNaLista(tipoArray->tipo_dados, no->filhos);
	no->filhos = addListaNaLista(vars, no->filhos);
	no->valor.iVal = TIPO_PRIMITIVO_ARRAY;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoAcessoArray(char* nomeVariavel, ast_node_t* indice, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_ACESSO_ARRAY;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->valor.strVal = malloc(sizeof(char) * (strlen(nomeVariavel) + 1));
	strcpy(no->valor.strVal, nomeVariavel);
	no->linha = linha;
	no->filhos = addNoASTNaLista(indice, NULL);
	return no;
}

ast_node_t* criarNoTipoArray(int inicioArray, int fimArray, int tipo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_TIPO_ARRAY;
	no->filhos = addIntNaLista(fimArray, NULL);
	no->filhos = addIntNaLista(inicioArray, no->filhos);
	no->valor.iVal = tipo;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoChamadaSubrotina(char* nomeVariavel, lista_t* parametros, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_CHAMADA_SUBROTINA;
	no->filhos = parametros;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->valor.strVal = malloc(sizeof(char) * (strlen(nomeVariavel) + 1));
	strcpy(no->valor.strVal, nomeVariavel);
	no->linha = linha;
	return no;
}

ast_node_t* criarNoExit(ast_node_t* exp, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_EXIT;
	no->filhos = addNoASTNaLista(exp, NULL);
	no->linha = linha;
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
	case TAN_CHAR:
		sprintf(descricaoNoAST, "CHAR: %d", noAST->valor.cVal);
		break;
	case TAN_BOOLEAN:
		sprintf(descricaoNoAST, "BOOLEAN: %s", noAST->valor.iVal ? "TRUE" : "FALSE");
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
	case TAN_VARIAVEL:
		sprintf(descricaoNoAST, "VAR: %s", noAST->valor.strVal);
		break;
	case TAN_ATRIBUICAO:
		strcpy(descricaoNoAST, "ATRIBUICAO");
		break;
	case TAN_IF:
		strcpy(descricaoNoAST, "IF");
		break;
	case TAN_WHILE:
		strcpy(descricaoNoAST, "WHILE");
		break;
	case TAN_REPEAT:
		strcpy(descricaoNoAST, "REPEAT");
		break;
	case TAN_FORTO:
		strcpy(descricaoNoAST, "FORTO");
		break;
	case TAN_FORDOWNTO:
		strcpy(descricaoNoAST, "FORDOWNTO");
		break;
	case TAN_ASM:
		sprintf(descricaoNoAST, "ASM: %s", noAST->valor.strVal);
		break;
	case TAN_ACESSO_ARRAY:
		sprintf(descricaoNoAST, "ARRAY: %s", noAST->valor.strVal);
		break;
	case TAN_LISTA_VAR:
	case TAN_TIPO_ARRAY:
	case TAN_EXIT:
		break;
	case TAN_PROCEDURE:
		sprintf(descricaoNoAST, "Procedure: %s", noAST->valor.proVal->nome);
		break;
	case TAN_FUNCTION:
		sprintf(descricaoNoAST, "Function: %s", noAST->valor.funVal->nome);
		break;
	case TAN_CHAMADA_SUBROTINA:
		sprintf(descricaoNoAST, "Chamada: %s", noAST->valor.strVal);
		break;
	}
	Agnode_t *f = agnode(g, descricaoNoAST, 1);
	if (p != NULL) {
		(void)agedge(g, p, f, 0, 1);
	}

	lista_t* filhos = noAST->filhos;
	while (filhos != NULL) {
		if (filhos->tipo == TC_LISTA) {
			Agnode_t *n = agnode(g, "Código(Statements)", 1);
			if (p != NULL) {
				(void)agedge(g, f, n, 0, 1);
			}
			filhos = caudaDaLista(filhos);
			continue;
		}
		if (filhos->tipo != TC_AST_NODE) {
			filhos = caudaDaLista(filhos);
			continue;
		}

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
