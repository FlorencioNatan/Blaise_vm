#include <string.h>
#include "ast.h"
#include <stdlib.h>
#include <graphviz/gvc.h>

bool verificarTiposDosStatements(mapa_t *tabela_simbolos, lista_t *statements, char *nomeSubrotina);

mapa_t* adicionaListaVariaveisPrimitivasNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos, int *posicaoMemoria, int linha) {
	while (variaveis != NULL) {
		if (variaveis->tipo != TC_STRING) {
			variaveis = caudaDaLista(variaveis);
			return NULL;
		}

		variavel_t* variavel = malloc(sizeof(variavel_t));
		variavel->eArray = false;
		variavel->inicioArray = 0;
		variavel->fimArray = 0;
		variavel->nome = malloc(sizeof(char) * strlen(variaveis->valor.strVal) + 1);
		strcpy(variavel->nome, variaveis->valor.strVal);
		variavel->tipo = tipo;

		if (tabela_simbolos != NULL && contemChaveNoMapa(variavel->nome, tabela_simbolos)) {
			printf("** Linha %d: A variável \"%s\" já foi declarada anteriormente.\n", linha, variavel->nome);
			variaveis = caudaDaLista(variaveis);
			return NULL;
		}

		switch (tipo) {
			case TIPO_PRIMITIVO_INTEGER:
				variavel->valor.iVal = 0;
				variavel->comprimentoNaMemoria = COMPRIMENTO_INTEGER_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_STRING:
				variavel->valor.strVal = malloc(sizeof(char));
				variavel->valor.strVal = "";
				variavel->comprimentoNaMemoria = COMPRIMENTO_STRING_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_CHAR:
				variavel->valor.chVal = ' ';
				variavel->comprimentoNaMemoria = COMPRIMENTO_CHAR_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_BOOLEAN:
				variavel->valor.boolVal = true;
				variavel->comprimentoNaMemoria = COMPRIMENTO_BOOLEAN_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_REAL:
				variavel->valor.realVal = 0.0;
				variavel->comprimentoNaMemoria = COMPRIMENTO_REAL_NA_MEMORIA;
				break;
		}

		variavel->posicaoNaMemoria = *posicaoMemoria;
		*posicaoMemoria += variavel->comprimentoNaMemoria;

		tabela_simbolos = addVariavelNoMapa(variavel->nome, variavel, tabela_simbolos);
		variaveis = caudaDaLista(variaveis);
	}

	return tabela_simbolos;
}

mapa_t* adicionaListaVariaveisArrayNaTabelaDeSimbolos(ast_node_t* declaracao, mapa_t *tabela_simbolos, int *posicaoMemoria) {
	lista_t *variaveis = declaracao->filhos->valor.lista;
	lista_t *tipo_l = caudaDaLista(declaracao->filhos);
	lista_t *inicioArray_l = caudaDaLista(tipo_l);
	lista_t *fimArray_l = caudaDaLista(inicioArray_l);

	int linha = declaracao->linha;
	int tipo = tipo_l->valor.iVal;
	int inicioArray = inicioArray_l->valor.iVal;
	int fimArray = fimArray_l->valor.iVal;

	while (variaveis != NULL) {
		if (variaveis->tipo != TC_STRING) {
			variaveis = caudaDaLista(variaveis);
			return NULL;
		}

		variavel_t* variavel = malloc(sizeof(variavel_t));
		variavel->eArray = true;
		variavel->inicioArray = inicioArray;
		variavel->fimArray = fimArray;
		variavel->nome = malloc(sizeof(char) * strlen(variaveis->valor.strVal) + 1);
		strcpy(variavel->nome, variaveis->valor.strVal);
		variavel->tipo = tipo;

		if (inicioArray >= fimArray) {
			printf("** Linha %d: Tamanho do array \"%s\" é inválido. A posição inicial do array precisa ser menor do que a posição final\n", linha, variavel->nome);
			return NULL;
		}

		if (tabela_simbolos != NULL && contemChaveNoMapa(variavel->nome, tabela_simbolos)) {
			printf("** Linha %d: A variável \"%s\" já foi declarada anteriormente.\n", linha, variavel->nome);
			variaveis = caudaDaLista(variaveis);
			return NULL;
		}

		switch (tipo) {
			case TIPO_PRIMITIVO_INTEGER:
				variavel->valor.iVal = 0;
				variavel->comprimentoNaMemoria = COMPRIMENTO_INTEGER_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_STRING:
				variavel->valor.strVal = malloc(sizeof(char));
				variavel->valor.strVal = "";
				variavel->comprimentoNaMemoria = COMPRIMENTO_STRING_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_CHAR:
				variavel->valor.chVal = ' ';
				variavel->comprimentoNaMemoria = COMPRIMENTO_CHAR_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_BOOLEAN:
				variavel->valor.boolVal = true;
				variavel->comprimentoNaMemoria = COMPRIMENTO_BOOLEAN_NA_MEMORIA;
				break;
			case TIPO_PRIMITIVO_REAL:
				variavel->valor.realVal = 0.0;
				variavel->comprimentoNaMemoria = COMPRIMENTO_REAL_NA_MEMORIA;
				break;
		}

		variavel->posicaoNaMemoria = *posicaoMemoria;
		*posicaoMemoria += variavel->comprimentoNaMemoria * (fimArray - inicioArray + 1);

		tabela_simbolos = addVariavelNoMapa(variavel->nome, variavel, tabela_simbolos);
		variaveis = caudaDaLista(variaveis);
	}

	return tabela_simbolos;
}

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

	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_PROCEDURE;
	no->tipo_dados = TIPO_PRIMITIVO_NAO_PREENCHIDO;
	no->valor.proVal = procedure;
	no->linha = linha;
	return no;
}

ast_node_t* criarNoFunction(char* nome, lista_t *parametros, lista_t *variaveis, lista_t *filhos, int tipo_retorno, int linha) {
	function_t* function = malloc(sizeof(programa_t));
	function->nome = malloc(sizeof(char) * strlen(nome) + 1);
	strcpy(function->nome, nome);

	function->parametros = parametros;
	function->variaveis = variaveis;
	function->filhos = filhos;
	function->tabela_simbolos = NULL;
	function->tipo_retorno = tipo_retorno;

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

mapa_t * adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(mapa_t *tabela_simbolos, lista_t *declaracoes) {
	int posicaoMemoria = 0;
	while (declaracoes != NULL) {
		ast_node_t* declaracao = declaracoes->valor.astNode;
		if (declaracao->valor.iVal != TIPO_PRIMITIVO_ARRAY) {
			tabela_simbolos = adicionaListaVariaveisPrimitivasNaTabelaDeSimbolos(
				declaracao->filhos->valor.lista,
				declaracao->valor.iVal,
				tabela_simbolos,
				&posicaoMemoria,
				declaracao->linha
			);
		} else {
			tabela_simbolos = adicionaListaVariaveisArrayNaTabelaDeSimbolos(
				declaracao,
				tabela_simbolos,
				&posicaoMemoria
			);
		}
		if (tabela_simbolos == NULL) {
			return false;
		}
		declaracoes = caudaDaLista(declaracoes);
	}
	return tabela_simbolos;
}

bool adicionarDeclaracoesSubrotinasNaTabelaDeSimbolos(programa_t *programa) {
	lista_t *subrotinas = programa->subrotinas;
	while (subrotinas != NULL) {
		ast_node_t* declaracao = subrotinas->valor.astNode;

		if (declaracao->tipo == TAN_PROCEDURE) {
			procedure_t *procedure = declaracao->valor.proVal;
			programa->tabela_simbolos = addProcedureNoMapa(procedure->nome, procedure, programa->tabela_simbolos);
		} else {
			function_t *function = declaracao->valor.funVal;
			programa->tabela_simbolos = addFunctionNoMapa(function->nome, function, programa->tabela_simbolos);
		}

		if (programa->tabela_simbolos == NULL) {
			return false;
		}
		subrotinas = caudaDaLista(subrotinas);
	}
	return true;
}

lista_t* formatarParametrosSubrotinas(lista_t* parametros) {
	lista_t* parametros_processados = NULL;
	while (parametros != NULL) {
		ast_node_t* declaracao = parametros->valor.astNode;
		lista_t* variaveis_parametro = declaracao->filhos->valor.lista;
		int tipoDados = declaracao->valor.iVal;
		while (variaveis_parametro != NULL) {
			ast_node_t *variavel = criarNoVariavel(variaveis_parametro->valor.strVal, -1);
			variavel->tipo_dados = tipoDados;
			parametros_processados = addNoASTNaLista(variavel, parametros_processados);
			variaveis_parametro = caudaDaLista(variaveis_parametro);
		}
		parametros = caudaDaLista(parametros);
	}
	return parametros_processados;
}

bool adicionarDeclaracoesDeVariaveisDasSubrotinas(programa_t *programa) {
	lista_t *subrotinas = programa->subrotinas;
	while (subrotinas != NULL) {
		ast_node_t* declaracao = subrotinas->valor.astNode;
		mapa_t* tabela_simbolos = NULL;
		lista_t* variaveis = NULL;

		if (declaracao->tipo == TAN_PROCEDURE) {
			procedure_t *procedure = declaracao->valor.proVal;
			tabela_simbolos = procedure->tabela_simbolos;
			variaveis = procedure->parametros;
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis);
			procedure->parametros = formatarParametrosSubrotinas(procedure->parametros);
			variaveis = procedure->variaveis;
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis);
			procedure->tabela_simbolos = tabela_simbolos;
			if (procedure->tabela_simbolos != NULL) {
				procedure->tabela_simbolos->mapa_pai = programa->tabela_simbolos;
			}
		} else {
			function_t *function = declaracao->valor.funVal;
			tabela_simbolos = function->tabela_simbolos;
			variaveis = function->parametros;
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis);
			function->parametros = formatarParametrosSubrotinas(function->parametros);
			variaveis = function->variaveis;
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis);
			function->tabela_simbolos = tabela_simbolos;
			if (function->tabela_simbolos != NULL) {
				function->tabela_simbolos->mapa_pai = programa->tabela_simbolos;
			}
		}

		if (tabela_simbolos == NULL && variaveis != NULL) {
			return false;
		}
		subrotinas = caudaDaLista(subrotinas);
	}

	return true;
}

bool criarTabelaDeSimbolos(programa_t *programa) {
	if (programa == NULL) {
		return false;
	}

	programa->tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(programa->tabela_simbolos, programa->variaveis);
	if (programa->tabela_simbolos == NULL && programa->variaveis != NULL) {
		return false;
	}

	if (!adicionarDeclaracoesSubrotinasNaTabelaDeSimbolos(programa)) {
		return false;
	}

	if (!adicionarDeclaracoesDeVariaveisDasSubrotinas(programa)) {
		return false;
	}

	return true;
}

char *retornaNomeDoTipo(int tipo) {
	switch (tipo){
	case TIPO_PRIMITIVO_INTEGER:
		return "Integer";
	case TIPO_PRIMITIVO_STRING:
		return "String";
	case TIPO_PRIMITIVO_CHAR:
		return "Char";
	case TIPO_PRIMITIVO_BOOLEAN:
		return "Boolean";
	case TIPO_PRIMITIVO_REAL:
		return "Real";
	case TIPO_PRIMITIVO_NAO_PREENCHIDO:
		return "Tipo não detectado";
	default:
		return "Tipo inválido";
	}
}

void imprimirMensagemDeErroDeTipo(int linha, int tipoUm, int tipoDois) {
	printf("** Linha %d: Tipos incompatíveis. Esperado: %s, encontrado: %s.\n", linha, retornaNomeDoTipo(tipoUm), retornaNomeDoTipo(tipoDois));
}

variavel_t* buscaVariavel(char *chave, mapa_t *mapa, int linha) {
	variavel_t* variavel;
	variavel = buscarVariavelNoMapa(chave, mapa);
	if (variavel == NULL) {
		variavel = buscarVariavelNoMapa(chave, mapa->mapa_pai);
	}

	if (variavel == NULL) {
		printf("** Linha %d: Variável \"%s\" não declarada.\n", linha, chave);
		return NULL;
	}

	return variavel;
}

bool verificarTipoDaExpressao(mapa_t *tabela_simbolos, ast_node_t* expressao) {
	ast_node_t *lhsNode = NULL;
	ast_node_t *rhsNode = NULL;
	variavel_t* variavel;
	switch (expressao->tipo) {
	case TAN_VARIAVEL:
		variavel = buscaVariavel(expressao->valor.strVal, tabela_simbolos, expressao->linha);
		if (variavel != NULL) {
			expressao->tipo_dados = variavel->tipo;
		} else {
			return false;
		}
		break;
	case TAN_ACESSO_ARRAY:
		variavel = buscaVariavel(expressao->valor.strVal, tabela_simbolos, expressao->linha);
		if (variavel != NULL) {
			expressao->tipo_dados = variavel->tipo;
		} else {
			return false;
		}
		break;
	case TAN_IGUAL:
	case TAN_DIFERENTE:
	case TAN_MAIOR:
	case TAN_MAIOR_IGUAL:
	case TAN_MENOR:
	case TAN_MENOR_IGUAL:
		lhsNode = expressao->filhos->valor.astNode;
		rhsNode = caudaDaLista(expressao->filhos)->valor.astNode;
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, lhsNode)) {
				return false;
			}
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, rhsNode)) {
				return false;
			}
		}
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_REAL && rhsNode ->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
			expressao->tipo_dados = TIPO_PRIMITIVO_BOOLEAN;
			return true;
		}
		if (lhsNode->tipo_dados != rhsNode->tipo_dados) {
			imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, rhsNode->tipo_dados);
			return false;
		}
		expressao->tipo_dados = TIPO_PRIMITIVO_BOOLEAN;
		break;
	case TAN_AND:
	case TAN_OR:
		lhsNode = expressao->filhos->valor.astNode;
		rhsNode = caudaDaLista(expressao->filhos)->valor.astNode;
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, lhsNode)) {
				return false;
			}
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, rhsNode)) {
				return false;
			}
		}
		if (lhsNode->tipo_dados != rhsNode->tipo_dados || lhsNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
			imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, rhsNode->tipo_dados);
			return false;
		}
		expressao->tipo_dados = TIPO_PRIMITIVO_BOOLEAN;
		break;
	case TAN_NOT:
		lhsNode = expressao->filhos->valor.astNode;
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, lhsNode)) {
				return false;
			}
		}
		if (lhsNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
			imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
			return false;
		}
		expressao->tipo_dados = TIPO_PRIMITIVO_BOOLEAN;
		break;
	case TAN_SOMA:
	case TAN_SUBTRACAO:
	case TAN_MULTIPLICACAO:
	case TAN_DIVISAO:
		lhsNode = expressao->filhos->valor.astNode;
		rhsNode = caudaDaLista(expressao->filhos)->valor.astNode;
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, lhsNode)) {
				return false;
			}
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, rhsNode)) {
				return false;
			}
		}
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_REAL && rhsNode ->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
			expressao->tipo_dados = TIPO_PRIMITIVO_REAL;
			return true;
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_REAL && lhsNode ->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
			expressao->tipo_dados = TIPO_PRIMITIVO_REAL;
			return true;
		}
		if (
			lhsNode->tipo_dados != rhsNode->tipo_dados ||
			(lhsNode->tipo_dados != TIPO_PRIMITIVO_INTEGER &&
			lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL
			)
		) {
			imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, rhsNode->tipo_dados);
			return false;
		}
		expressao->tipo_dados = lhsNode->tipo_dados;
		break;
	case TAN_NEGATIVACAO:
		lhsNode = expressao->filhos->valor.astNode;
		if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			if (!verificarTipoDaExpressao(tabela_simbolos, lhsNode)) {
				return false;
			}
		}
		if (lhsNode->tipo_dados != TIPO_PRIMITIVO_INTEGER && lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
			imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
			return false;
		}
		expressao->tipo_dados = lhsNode->tipo_dados;
		break;
		break;
	default: break;
	}

	return true;
}

bool verificarTipoAtribuicao(mapa_t *tabela_simbolos, ast_node_t* atribuicao) {
	lista_t *lhs = atribuicao->filhos;
	lista_t *rhs = caudaDaLista(atribuicao->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		variavel_t* variavel = buscaVariavel(lhsNode->valor.strVal, tabela_simbolos, lhsNode->linha);
		if (variavel == NULL) {
			return false;
		}
		lhsNode->tipo_dados = variavel->tipo;
	}

	if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoDaExpressao(tabela_simbolos, rhsNode);
	}

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_REAL && rhsNode ->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		atribuicao->tipo_dados = lhsNode ->tipo_dados;
		return false;
	}

	if (lhsNode->tipo_dados != rhsNode->tipo_dados) {
		imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, rhsNode->tipo_dados);
		return false;
	}
	atribuicao->tipo_dados = lhsNode ->tipo_dados;
	return true;
}

bool verificarTipoIf(mapa_t *tabela_simbolos, ast_node_t* atribuicao, char *nomeSubrotina) {
	lista_t *condicao = atribuicao->filhos;
	lista_t *then_stmts = caudaDaLista(atribuicao->filhos);
	lista_t *else_stmts = caudaDaLista(then_stmts);

	ast_node_t *condicaoNode = condicao->valor.astNode;
	bool sucesso = true;

	if (condicaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, condicaoNode);
	}

	if (condicaoNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
		imprimirMensagemDeErroDeTipo(condicaoNode->linha, condicaoNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
		return false;
	}

	if (then_stmts != NULL) {
		sucesso = sucesso & verificarTiposDosStatements(tabela_simbolos, then_stmts->valor.lista, nomeSubrotina);
	}

	if (else_stmts != NULL) {
		sucesso = sucesso & verificarTiposDosStatements(tabela_simbolos, else_stmts->valor.lista, nomeSubrotina);
	}
	return sucesso;
}

bool verificarTipoWhile(mapa_t *tabela_simbolos, ast_node_t* atribuicao, char *nomeSubrotina) {
	lista_t *condicao = atribuicao->filhos;
	lista_t *codigo = caudaDaLista(atribuicao->filhos);

	ast_node_t *condicaoNode = condicao->valor.astNode;
	bool sucesso = true;

	if (condicaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, condicaoNode);
	}

	if (condicaoNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
		imprimirMensagemDeErroDeTipo(condicaoNode->linha, condicaoNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
		return false;
	}

	if (codigo != NULL) {
		sucesso = sucesso & verificarTiposDosStatements(tabela_simbolos, codigo->valor.lista, nomeSubrotina);
	}
	return sucesso;
}

bool verificarTipoRepeat(mapa_t *tabela_simbolos, ast_node_t* atribuicao, char *nomeSubrotina) {
	lista_t *condicao = atribuicao->filhos;
	lista_t *codigo = caudaDaLista(atribuicao->filhos);

	ast_node_t *condicaoNode = condicao->valor.astNode;
	bool sucesso = true;

	if (condicaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, condicaoNode);
	}

	if (codigo != NULL) {
		sucesso = sucesso & verificarTiposDosStatements(tabela_simbolos, codigo->valor.lista, nomeSubrotina);
	}

	if (condicaoNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
		imprimirMensagemDeErroDeTipo(condicaoNode->linha, condicaoNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
		return false;
	}
	return sucesso;
}

bool verificarTipoFor(mapa_t *tabela_simbolos, ast_node_t* atribuicao, char *nomeSubrotina) {
	lista_t *inicializacao = atribuicao->filhos;
	lista_t *ate = caudaDaLista(inicializacao);
	lista_t *codigo = caudaDaLista(ate);

	ast_node_t *inicializacaoNode = inicializacao->valor.astNode;
	ast_node_t *ateNode = ate->valor.astNode;
	bool sucesso = true;

	if (inicializacaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		sucesso = sucesso & verificarTipoAtribuicao(tabela_simbolos, inicializacaoNode);
	}

	if (inicializacaoNode->tipo_dados != TIPO_PRIMITIVO_INTEGER && inicializacaoNode->tipo_dados != TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		imprimirMensagemDeErroDeTipo(inicializacaoNode->linha, inicializacaoNode->tipo_dados, TIPO_PRIMITIVO_INTEGER);
		return false;
	}


	if (ateNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, ateNode);
	}

	if (ateNode->tipo_dados != TIPO_PRIMITIVO_INTEGER) {
		imprimirMensagemDeErroDeTipo(ateNode->linha, ateNode->tipo_dados, TIPO_PRIMITIVO_INTEGER);
		return false;
	}


	if (codigo != NULL) {
		sucesso = sucesso & verificarTiposDosStatements(tabela_simbolos, codigo->valor.lista, nomeSubrotina);
	}
	return sucesso;
}

bool verificarTipoExit(mapa_t *tabela_simbolos, ast_node_t* exit, char *nomeSubrotina) {
	lista_t *exp = exit->filhos;

	ast_node_t *expNode = exp->valor.astNode;
	bool sucesso = true;

	if (expNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, expNode);
	}

	function_t *fun = buscarFunctionNoMapa(nomeSubrotina, tabela_simbolos);

	if (fun == NULL) {
		printf("** Linha %d: A função exit, só pode ser chamada a partir de uma função.\n", expNode->linha);
		return false;
	}

	if (expNode->tipo_dados != fun->tipo_retorno) {
		imprimirMensagemDeErroDeTipo(expNode->linha, expNode->tipo_dados, fun->tipo_retorno);
		return false;
	}
	return sucesso;
}

bool verificarTipoChamadaFuncao(mapa_t *tabela_simbolos, ast_node_t* chamada, function_t *fun) {
	int quantidadeParamentrosChamada = comprimentoDaLista(chamada->filhos);
	int quantidadeParamentrosFunction = comprimentoDaLista(fun->parametros);

	if (quantidadeParamentrosChamada != quantidadeParamentrosFunction) {
		printf("** Linha %d: A quantidade de parametros informada é diferente do que a função \"%s\" espera.\n", chamada->linha, fun->nome);
		return false;
	}

	bool sucesso = true;
	lista_t *paramChamada = chamada->filhos;
	lista_t *paramFunction = fun->parametros;
	int contadorParametro = 1;
	while (paramChamada != NULL) {
		ast_node_t *pcNode = paramChamada->valor.astNode;
		ast_node_t *pfNode = paramFunction->valor.astNode;
		if (pcNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, pcNode);
		}

		if (pcNode->tipo_dados != pfNode->tipo_dados) {
			printf(
				"** Linha %d: Tipos incompatíveis no parâmetro %d. Esperado: %s, encontrado: %s.\n",
				chamada->linha,
				contadorParametro,
				retornaNomeDoTipo(pfNode->tipo_dados),
				retornaNomeDoTipo(pcNode->tipo_dados)
			);
			return false;
		}

		paramChamada = caudaDaLista(paramChamada);
		paramFunction = caudaDaLista(paramFunction);
		contadorParametro++;
	}

	return sucesso;
}

bool verificarTipoChamadaProcedure(mapa_t *tabela_simbolos, ast_node_t* chamada, procedure_t *pro) {
	int quantidadeParamentrosChamada = comprimentoDaLista(chamada->filhos);
	int quantidadeParamentrosFunction = comprimentoDaLista(pro->parametros);

	if (quantidadeParamentrosChamada != quantidadeParamentrosFunction) {
		printf("** Linha %d: A quantidade de parametros informada é diferente do que a função \"%s\" espera.\n", chamada->linha, pro->nome);
		return false;
	}

	bool sucesso = true;
	lista_t *paramChamada = chamada->filhos;
	lista_t *paramProcedure = pro->parametros;
	int contadorParametro = 1;
	while (paramChamada != NULL) {
		ast_node_t *pcNode = paramChamada->valor.astNode;
		ast_node_t *ppNode = paramProcedure->valor.astNode;
		if (pcNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			sucesso = sucesso & verificarTipoDaExpressao(tabela_simbolos, pcNode);
		}

		if (pcNode->tipo_dados != ppNode->tipo_dados) {
			printf(
				"** Linha %d: Tipos incompatíveis no parâmetro %d. Esperado: %s, encontrado: %s.\n",
				chamada->linha,
				contadorParametro,
				retornaNomeDoTipo(ppNode->tipo_dados),
				retornaNomeDoTipo(pcNode->tipo_dados)
			);
			return false;
		}

		paramChamada = caudaDaLista(paramChamada);
		paramProcedure = caudaDaLista(paramProcedure);
		contadorParametro++;
	}

	return sucesso;
}

bool verificarTipoChamadaSubrotina(mapa_t *tabela_simbolos, ast_node_t* chamada) {
	function_t *fun = buscarFunctionNoMapa(chamada->valor.strVal, tabela_simbolos);
	if (fun != NULL) {
		return verificarTipoChamadaFuncao(tabela_simbolos, chamada, fun);
	}

	procedure_t *pro = buscarProcedureNoMapa(chamada->valor.strVal, tabela_simbolos);
	if (pro != NULL) {
		return verificarTipoChamadaProcedure(tabela_simbolos, chamada, pro);
	}

	printf("** Linha %d: A subrotina \"%s\" não exite.\n", chamada->linha, chamada->valor.strVal);
	return false;
}

bool verificarTiposDosStatements(mapa_t *tabela_simbolos, lista_t *statements, char *nomeSubrotina) {
	bool sucesso = true;
	while (statements != NULL) {
		ast_node_t* statement = statements->valor.astNode;
		if (statement == NULL) {
			return true;
		}

		switch (statement->tipo) {
		case TAN_ATRIBUICAO:
			sucesso = verificarTipoAtribuicao(tabela_simbolos, statement);
			break;
		case TAN_IF:
			sucesso = verificarTipoIf(tabela_simbolos, statement, nomeSubrotina);
			break;
		case TAN_WHILE:
			sucesso = verificarTipoWhile(tabela_simbolos, statement, nomeSubrotina);
			break;
		case TAN_REPEAT:
			sucesso = verificarTipoRepeat(tabela_simbolos, statement, nomeSubrotina);
			break;
		case TAN_FORTO:
		case TAN_FORDOWNTO:
			sucesso = verificarTipoFor(tabela_simbolos, statement, nomeSubrotina);
			break;
		case TAN_EXIT:
			sucesso = verificarTipoExit(tabela_simbolos, statement, nomeSubrotina);
			break;
		case TAN_CHAMADA_SUBROTINA:
			sucesso = verificarTipoChamadaSubrotina(tabela_simbolos, statement);
			break;
		default:
			break;
		}
		if (sucesso == false) {
			return false;
		}
		statements = caudaDaLista(statements);
	}
	return true;
}

bool verificarTiposDoPrograma(programa_t *programa) {
	if (!verificarTiposDosStatements(programa->tabela_simbolos, programa->filhos, NULL)) {
		return false;
	}

	lista_t *subrotinas = programa->subrotinas;
	while (subrotinas != NULL) {
		ast_node_t* declaracao = subrotinas->valor.astNode;
		mapa_t *tabela_simbolos;
		char *nome;
		lista_t *codigo_subrotina;
		if (declaracao->tipo == TAN_PROCEDURE) {
			nome = declaracao->valor.proVal->nome;
			tabela_simbolos = declaracao->valor.proVal->tabela_simbolos;
			codigo_subrotina = declaracao->valor.proVal->filhos;
		} else {
			nome = declaracao->valor.funVal->nome;
			tabela_simbolos = declaracao->valor.funVal->tabela_simbolos;
			codigo_subrotina = declaracao->valor.proVal->filhos;
		}
		if (!verificarTiposDosStatements(tabela_simbolos, codigo_subrotina, nome)) {
			return false;
		}
		subrotinas = caudaDaLista(subrotinas);
	}

	return true;
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



typedef struct contadores {
	int If;
	int While;
	int Repeat;
	int For;
	int comparacao;
	int logico;
} contadores_t;

void gerarAssemblyNoAst(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
);

void gerarAssemblyListaNoAst(
	programa_t* programa,
	lista_t* listaNo,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
);

void gerarAssemblyAtribuicao(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	variavel_t* variavel = buscaVariavel(lhsNode->valor.strVal, programa->tabela_simbolos, lhsNode->linha);

	char buffer[256] = "";

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n", variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	switch (variavel->tipo) {
	case TIPO_PRIMITIVO_INTEGER:
		strcpy(buffer, "    sh\n");
		break;
	case TIPO_PRIMITIVO_STRING:
		strcpy(buffer, "    sb\n");
		break;
	case TIPO_PRIMITIVO_CHAR:
		strcpy(buffer, "    sb\n");
		break;
	case TIPO_PRIMITIVO_BOOLEAN:
		strcpy(buffer, "    sb\n");
		break;
	case TIPO_PRIMITIVO_REAL:
		strcpy(buffer, "    sw\n");
		break;
	}
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

void gerarAssemblyObterValorDaVariavel(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly
) {
	variavel_t* variavel = buscaVariavel(noAST->valor.strVal, programa->tabela_simbolos, noAST->linha);

	char buffer[256] = "";
	sprintf(buffer, "    push %d\n", variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	switch (variavel->tipo) {
	case TIPO_PRIMITIVO_INTEGER:
		strcpy(buffer, "    lh\n");
		break;
	case TIPO_PRIMITIVO_STRING:
		strcpy(buffer, "    lb\n");
		break;
	case TIPO_PRIMITIVO_CHAR:
		strcpy(buffer, "    lb\n");
		break;
	case TIPO_PRIMITIVO_BOOLEAN:
		strcpy(buffer, "    lb\n");
		break;
	case TIPO_PRIMITIVO_REAL:
		strcpy(buffer, "    lw\n");
		break;
	}
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

void gerarAssemblySoma(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    add\n");
		*posicaoAssembly += strlen("    add\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    addf\n");
		*posicaoAssembly += strlen("    addf\n");
	}
}

void gerarAssemblySubtracao(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}
}

void gerarAssemblyMultiplicacao(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    mul\n");
		*posicaoAssembly += strlen("    mul\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    mulf\n");
		*posicaoAssembly += strlen("    mulf\n");
	}
}

void gerarAssemblyDivisao(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    div\n");
		*posicaoAssembly += strlen("    div\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    divf\n");
		*posicaoAssembly += strlen("    divf\n");
	}
}

void gerarAssemblyNegativacao(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *op = noAST->filhos;
	ast_node_t *opNode = op->valor.astNode;

	strcpy(&assembly[*posicaoAssembly], "    push 0\n");
	*posicaoAssembly += strlen("    push 0\n");
	gerarAssemblyNoAst(programa, opNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (opNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}
}

void gerarAssemblyIgual(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}

	char buffer[256] = "";
	sprintf(
		buffer,
		"    beqi igual_%d\n    push %d\n    jumpi fim_igual_%d\nigual_%d:\n    push %d\nfim_igual_%d:\n",
		contadores->comparacao,
		CONST_BOOLEAN_FALSE,
		contadores->comparacao,
		contadores->comparacao,
		CONST_BOOLEAN_TRUE,
		contadores->comparacao
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyDiferente(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bnei diferente_%d\n    push %d\n    jumpi fim_diferente_%d\ndiferente_%d:\n    push %d\nfim_diferente_%d:\n",
		contadores->comparacao,
		CONST_BOOLEAN_FALSE,
		contadores->comparacao,
		contadores->comparacao,
		CONST_BOOLEAN_TRUE,
		contadores->comparacao
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyMaior(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bgtzi maior_%d\n    push %d\n    jumpi fim_maior_%d\nmaior_%d:\n    push %d\nfim_maior_%d:\n",
		contadores->comparacao,
		CONST_BOOLEAN_FALSE,
		contadores->comparacao,
		contadores->comparacao,
		CONST_BOOLEAN_TRUE,
		contadores->comparacao
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyMaiorIgual(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bgezi maior_igual_%d\n    push %d\n    jumpi fim_maior_igual_%d\nmaior_igual_%d:\n    push %d\nfim_maior_igual_%d:\n",
		contadores->comparacao,
		CONST_BOOLEAN_FALSE,
		contadores->comparacao,
		contadores->comparacao,
		CONST_BOOLEAN_TRUE,
		contadores->comparacao
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyMenor(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bltzi menor_%d\n    push %d\n    jumpi fim_menor_%d\nmenor_%d:\n    push %d\nfim_menor_%d:\n",
		contadores->comparacao,
		CONST_BOOLEAN_FALSE,
		contadores->comparacao,
		contadores->comparacao,
		CONST_BOOLEAN_TRUE,
		contadores->comparacao
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyMenorIgual(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER && rhsNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}

	char buffer[256] = "";
	sprintf(
		buffer,
		"    blezi menor_igual_%d\n    push %d\n    jumpi fim_menor_igual_%d\nmenor_igual_%d:\n    push %d\nfim_menor_igual_%d:\n",
		contadores->comparacao,
		CONST_BOOLEAN_FALSE,
		contadores->comparacao,
		contadores->comparacao,
		CONST_BOOLEAN_TRUE,
		contadores->comparacao
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyAND(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";
	sprintf(
		buffer,
		"    beqi falso_and_%d\n",
		contadores->logico
	);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	strcpy(buffer,"");

	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	sprintf(
		buffer,
		"    beqi falso_and_%d\n    push %d\n    jumpi fim_and_%d\nfalso_and_%d:\n    push %d\nfim_and_%d:\n",
		contadores->logico,
		CONST_BOOLEAN_TRUE,
		contadores->logico,
		contadores->logico,
		CONST_BOOLEAN_FALSE,
		contadores->logico
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyOR(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	lista_t *rhs = caudaDaLista(noAST->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	gerarAssemblyNoAst(programa, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bnei true_or_%d\n",
		contadores->logico
	);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	strcpy(buffer,"");

	gerarAssemblyNoAst(programa, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	sprintf(
		buffer,
		"    bnei true_or_%d\n    push %d\n    jumpi fim_or_%d\ntrue_or_%d:\n    push %d\nfim_or_%d:\n",
		contadores->logico,
		CONST_BOOLEAN_FALSE,
		contadores->logico,
		contadores->logico,
		CONST_BOOLEAN_TRUE,
		contadores->logico
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyNOT(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	ast_node_t *opNode = lhs->valor.astNode;

	gerarAssemblyNoAst(programa, opNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bnei false_not_%d\n    push %d\n    jumpi fim_not_%d\nfalse_not_%d:\n    push %d\nfim_not_%d:\n",
		contadores->logico,
		CONST_BOOLEAN_TRUE,
		contadores->logico,
		contadores->logico,
		CONST_BOOLEAN_FALSE,
		contadores->logico
	);

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->comparacao++;
}

void gerarAssemblyIf(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *condicao = noAST->filhos;
	lista_t *then_stmt = caudaDaLista(noAST->filhos);
	lista_t *else_stmt = caudaDaLista(then_stmt);
	ast_node_t *condicaoNode = condicao->valor.astNode;
	lista_t *then_stmtLista = then_stmt->valor.lista;
	lista_t *else_stmtLista = else_stmt != NULL ? else_stmt->valor.lista : NULL;

	char buffer[256] = "";

	sprintf(buffer, "# Começo do if %d\n", contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyNoAst(programa, condicaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    beqi else_if_%d\nthan_if_%d:\n", contadores->If, contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyListaNoAst(programa, then_stmtLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "else_if_%d:\n", contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	if (else_stmtLista != NULL) {
		gerarAssemblyListaNoAst(programa, else_stmtLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	}
	strcpy(buffer, "");
	sprintf(buffer, "fim_if_%d:\n", contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->If++;
}

void gerarAssemblyWhile(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *condicao = noAST->filhos;
	lista_t *codigo = caudaDaLista(noAST->filhos);

	ast_node_t *condicaoNode = condicao->valor.astNode;
	lista_t *codigoLista = codigo->valor.lista;

	char buffer[256] = "";

	sprintf(buffer, "inicio_while_%d:\n", contadores->While);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyNoAst(programa, condicaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    beqi fim_while_%d\n", contadores->While);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyListaNoAst(programa, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);


	strcpy(buffer, "");
	sprintf(buffer, "    jumpi inicio_while_%d\nfim_while_%d:\n", contadores->While, contadores->While);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->While++;
}

void gerarAssemblyRepeat(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *condicao = noAST->filhos;
	lista_t *codigo = caudaDaLista(noAST->filhos);

	ast_node_t *condicaoNode = condicao->valor.astNode;
	lista_t *codigoLista = codigo->valor.lista;

	char buffer[256] = "";

	sprintf(buffer, "inicio_repeat_%d:\n", contadores->Repeat);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	gerarAssemblyListaNoAst(programa, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	gerarAssemblyNoAst(programa, condicaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    bnei fim_repeat_%d\n    jumpi inicio_repeat_%d\n\nfim_repeat_%d:\n", contadores->Repeat, contadores->Repeat, contadores->Repeat);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->Repeat++;
}

void gerarAssemblyForTo(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *inicializacao = noAST->filhos;
	lista_t *ate = caudaDaLista(inicializacao);
	lista_t *codigo = caudaDaLista(ate);

	ast_node_t *inicializacaoNode = inicializacao->valor.astNode;
	ast_node_t *ateNode = ate->valor.astNode;
	ast_node_t *varNode = inicializacaoNode->filhos->valor.astNode;

	variavel_t* variavel = buscaVariavel(varNode->valor.strVal, programa->tabela_simbolos, varNode->linha);

	lista_t *codigoLista = codigo->valor.lista;

	char buffer[256] = "";

	gerarAssemblyNoAst(programa, inicializacaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	sprintf(buffer, "inicio_for_%d:\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);


	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n    lh\n", variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyNoAst(programa, ateNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	strcpy(buffer, "");
	sprintf(buffer, "    sub\n    beqi fim_for_%d\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	gerarAssemblyListaNoAst(programa, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n    lh\n    push 1\n    add\n    push %d\n    sh\n", variavel->posicaoNaMemoria, variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "");
	sprintf(buffer, "    jumpi inicio_for_%d\nfim_for_%d:\n", contadores->For, contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->For++;
}

void gerarAssemblyForDownTo(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *inicializacao = noAST->filhos;
	lista_t *ate = caudaDaLista(inicializacao);
	lista_t *codigo = caudaDaLista(ate);

	ast_node_t *inicializacaoNode = inicializacao->valor.astNode;
	ast_node_t *ateNode = ate->valor.astNode;
	ast_node_t *varNode = inicializacaoNode->filhos->valor.astNode;

	variavel_t* variavel = buscaVariavel(varNode->valor.strVal, programa->tabela_simbolos, varNode->linha);

	lista_t *codigoLista = codigo->valor.lista;

	char buffer[256] = "";

	gerarAssemblyNoAst(programa, inicializacaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	sprintf(buffer, "inicio_for_%d:\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);


	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n    lh\n", variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyNoAst(programa, ateNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	strcpy(buffer, "");
	sprintf(buffer, "    sub\n    beqi fim_for_%d\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	gerarAssemblyListaNoAst(programa, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n    lh\n    push 1\n    sub\n    push %d\n    sh\n", variavel->posicaoNaMemoria, variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "");
	sprintf(buffer, "    jumpi inicio_for_%d\nfim_for_%d:\n", contadores->For, contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->For++;
}

void gerarAssemblyAsm(
	programa_t *programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly
) {
	int tamanhoAssembly = strlen(noAST->valor.strVal);
	char *noASRStr = noAST->valor.strVal;
	char *buffer = calloc(tamanhoAssembly * 2, sizeof(char));

	for (int i = 0, bI = 0; i < tamanhoAssembly; i++, bI++) {
		if (noASRStr[i] == '@') {
			i++;
			char varNome[256] = "";
			int varI = 0;
			while (
				(noASRStr[i] >= 0x30 && noASRStr[i] <= 0x39) || // Digito numerico
				(noASRStr[i] >= 0x41 && noASRStr[i] <= 0x5A) || // Letra maiúscula
				(noASRStr[i] >= 0x61 && noASRStr[i] <= 0x7A)    // Letra minúscula
			) {
				varNome[varI++] = noASRStr[i++];
			}
			varNome[varI++] = '\0';
			i--;

			variavel_t* variavel = buscaVariavel(varNome, programa->tabela_simbolos, noAST->linha);
			if (variavel == NULL) {
				return;
			}

			strcpy(varNome, "");
			sprintf(varNome, "%d", variavel->posicaoNaMemoria);
			strcpy(&buffer[bI], varNome);
			bI += strlen(varNome) - 1;
			continue;
		}
		buffer[bI] = noASRStr[i];
	}

	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	free(buffer);
}

/**
 * Está faltando fazer as validações dos indices em tempo de execução
 */
void gerarAssemblyObterValorDoArray(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	variavel_t* variavel = buscaVariavel(noAST->valor.strVal, programa->tabela_simbolos, noAST->linha);

	ast_node_t *indice = noAST->filhos->valor.astNode;

	gerarAssemblyNoAst(programa, indice, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";
	sprintf(buffer, "    push %d\n    sub\n    push %d\n", variavel->inicioArray, variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	switch (variavel->tipo) {
	case TIPO_PRIMITIVO_INTEGER:
		strcpy(buffer, "    lh\n");
		break;
	case TIPO_PRIMITIVO_STRING:
		strcpy(buffer, "    lb\n");
		break;
	case TIPO_PRIMITIVO_CHAR:
		strcpy(buffer, "    lb\n");
		break;
	case TIPO_PRIMITIVO_BOOLEAN:
		strcpy(buffer, "    lb\n");
		break;
	case TIPO_PRIMITIVO_REAL:
		strcpy(buffer, "    lw\n");
		break;
	}
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

void gerarAssemblyNoAst(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {

	if (*comprimentoAssembly - *posicaoAssembly < 512) {
		*comprimentoAssembly = 2 * *comprimentoAssembly;
		char *novoAssembly = malloc(sizeof(char) * *comprimentoAssembly);
		strcpy(novoAssembly, assembly);
		free(assembly);
		assembly = novoAssembly;
	}

	char buffer[256] = "";
	if (noAST == NULL) {
		return;
	}

	switch (noAST->tipo) {
	case TAN_PROGRAMA:
		break;
	case TAN_REAL:
		sprintf(buffer, "    push %f\n", noAST->valor.dVal);
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);
		break;
	case TAN_INTEGER:
		sprintf(buffer, "    push %d\n", noAST->valor.iVal);
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);
		break;
	case TAN_SOMA:
		gerarAssemblySoma(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_SUBTRACAO:
		gerarAssemblySubtracao(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MULTIPLICACAO:
		gerarAssemblyMultiplicacao(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_DIVISAO:
		gerarAssemblyDivisao(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_NEGATIVACAO:
		gerarAssemblyNegativacao(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_IGUAL:
		gerarAssemblyIgual(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_DIFERENTE:
		gerarAssemblyDiferente(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MAIOR:
		gerarAssemblyMaior(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MAIOR_IGUAL:
		gerarAssemblyMaiorIgual(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MENOR:
		gerarAssemblyMenor(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MENOR_IGUAL:
		gerarAssemblyMenorIgual(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_AND:
		gerarAssemblyAND(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_OR:
		gerarAssemblyOR(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_NOT:
		gerarAssemblyNOT(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_VARIAVEL:
		gerarAssemblyObterValorDaVariavel(programa, noAST, assembly, posicaoAssembly);
		break;
	case TAN_ATRIBUICAO:
		gerarAssemblyAtribuicao(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_IF:
		gerarAssemblyIf(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_WHILE:
		gerarAssemblyWhile(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_REPEAT:
		gerarAssemblyRepeat(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_FORTO:
		gerarAssemblyForTo(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_FORDOWNTO:
		gerarAssemblyForDownTo(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_ASM:
		gerarAssemblyAsm(programa, noAST, assembly, posicaoAssembly);
		break;
	case TAN_ACESSO_ARRAY:
		gerarAssemblyObterValorDoArray(programa, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_LISTA_VAR:
	case TAN_TIPO_ARRAY:
		break;
	}
}

void gerarAssemblyListaNoAst(
	programa_t* programa,
	lista_t* listaNo,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t* filhos = listaNo;
	while (filhos != NULL) {
		ast_node_t* valor = filhos->valor.astNode;
		gerarAssemblyNoAst(programa, valor, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		filhos = caudaDaLista(filhos);
	}
}

char* gerarAssembly(programa_t *programa) {
	if (programa == NULL) {
		return NULL;
	}

	char *assembly;
	int comprimentoAssembly = 5000000;
	int posicaoAssembly = 0;
	lista_t* filhos = programa->filhos;
	contadores_t *contadores = malloc(sizeof(contadores_t));

	contadores->For = 0;
	contadores->If = 0;
	contadores->Repeat = 0;
	contadores->While = 0;
	contadores->comparacao = 0;
	contadores->logico = 0;

	assembly = malloc(sizeof(char) * comprimentoAssembly);
	sprintf(assembly, "# Programa: %s\n.code\n", programa->nome);
	posicaoAssembly += strlen(assembly);

	while (filhos != NULL) {
		ast_node_t* valor = filhos->valor.astNode;
		gerarAssemblyNoAst(programa, valor, assembly, &posicaoAssembly, &comprimentoAssembly, contadores);
		filhos = caudaDaLista(filhos);
	}
	strcpy(&assembly[posicaoAssembly], "    halt\n");
	posicaoAssembly += strlen("    halt\n");

	free(contadores);

	return assembly;
}
