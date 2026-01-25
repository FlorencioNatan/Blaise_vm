#include <string.h>
#include "verificacao_de_tipos.h"
#include <stdlib.h>
#include <stdio.h>

bool verificarTiposDosStatements(mapa_t *tabela_simbolos, lista_t *statements, char *nomeSubrotina);
bool verificarTipoDaExpressao(mapa_t *tabela_simbolos, ast_node_t* expressao);

mapa_t* adicionaListaVariaveisPrimitivasNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos, int *posicaoMemoria, bool positivo, int linha) {
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
				variavel->comprimentoNaMemoria = COMPRIMENTO_STRING_NA_MEMORIA * 255;
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

		if (positivo) {
			variavel->posicaoNaMemoria = *posicaoMemoria;
			*posicaoMemoria += variavel->comprimentoNaMemoria;
		} else {
			*posicaoMemoria -= variavel->comprimentoNaMemoria;
			variavel->posicaoNaMemoria = *posicaoMemoria;
		}

		tabela_simbolos = addVariavelNoMapa(variavel->nome, variavel, tabela_simbolos);
		variaveis = caudaDaLista(variaveis);
	}

	return tabela_simbolos;
}

mapa_t* adicionaListaVariaveisArrayNaTabelaDeSimbolos(ast_node_t* declaracao, mapa_t *tabela_simbolos, int *posicaoMemoria, bool positivo) {
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
				variavel->comprimentoNaMemoria = COMPRIMENTO_STRING_NA_MEMORIA * 255;
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


		if (positivo) {
			variavel->posicaoNaMemoria = *posicaoMemoria;
			*posicaoMemoria += variavel->comprimentoNaMemoria * (fimArray - inicioArray + 1);
		} else {
			*posicaoMemoria -= variavel->comprimentoNaMemoria * (fimArray - inicioArray + 1);
			variavel->posicaoNaMemoria = *posicaoMemoria;
		}

		tabela_simbolos = addVariavelNoMapa(variavel->nome, variavel, tabela_simbolos);
		variaveis = caudaDaLista(variaveis);
	}

	return tabela_simbolos;
}

mapa_t * adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(mapa_t *tabela_simbolos, lista_t *declaracoes, bool positivo, int *tamanho_memoria_usado) {
	int posicaoMemoria = positivo ? 0 : -8;
	while (declaracoes != NULL) {
		ast_node_t* declaracao = declaracoes->valor.astNode;
		if (declaracao->valor.iVal != TIPO_PRIMITIVO_ARRAY) {
			tabela_simbolos = adicionaListaVariaveisPrimitivasNaTabelaDeSimbolos(
				declaracao->filhos->valor.lista,
				declaracao->valor.iVal,
				tabela_simbolos,
				&posicaoMemoria,
				positivo,
				declaracao->linha
			);
		} else {
			tabela_simbolos = adicionaListaVariaveisArrayNaTabelaDeSimbolos(
				declaracao,
				tabela_simbolos,
				&posicaoMemoria,
				positivo
			);
		}
		if (tabela_simbolos == NULL) {
			return false;
		}
		declaracoes = caudaDaLista(declaracoes);
	}
	if (tamanho_memoria_usado != NULL) {
		*tamanho_memoria_usado = posicaoMemoria;
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
			variaveis = inverterLista(procedure->parametros);
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis, false, &procedure->comprimento_parametros_stack);
			procedure->comprimento_parametros_stack = abs(procedure->comprimento_parametros_stack);
			procedure->parametros = formatarParametrosSubrotinas(procedure->parametros);
			variaveis = procedure->variaveis;
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis, true, &procedure->comprimento_memoria_stack);
			procedure->tabela_simbolos = tabela_simbolos;
			if (procedure->tabela_simbolos != NULL) {
				procedure->tabela_simbolos->mapa_pai = programa->tabela_simbolos;
			}
		} else {
			function_t *function = declaracao->valor.funVal;
			tabela_simbolos = function->tabela_simbolos;
			variaveis = inverterLista(function->parametros);
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis, false, &function->comprimento_parametros_stack);
			function->comprimento_parametros_stack = abs(function->comprimento_parametros_stack);
			function->parametros = formatarParametrosSubrotinas(function->parametros);
			variaveis = function->variaveis;
			tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(tabela_simbolos, variaveis, true, &function->comprimento_memoria_stack);
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

	programa->tabela_simbolos = adicionarDeclaracoesVariaveisNaTabelaDeSimbolos(programa->tabela_simbolos, programa->variaveis, true, &programa->comprimento_memoria_stack);
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

	chamada->tipo_dados = fun->tipo_retorno;
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
			imprimirMensagemDeErroDeTipo(lhsNode->linha, lhsNode->tipo_dados, TIPO_PRIMITIVO_INTEGER);
			return false;
		}
		expressao->tipo_dados = lhsNode->tipo_dados;
		break;
	case TAN_CHAMADA_SUBROTINA:
		return verificarTipoChamadaSubrotina(tabela_simbolos, expressao);
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
