#include <string.h>
#include "ast.h"
#include <stdlib.h>
#include <graphviz/gvc.h>

bool verificarTiposDosStatements(programa_t *programa, lista_t *statements);

mapa_t* adicionaListaVariaveisNaTabelaDeSimbolos(lista_t *variaveis, int tipo, mapa_t *tabela_simbolos, int *posicaoMemoria, int linha) {
	while (variaveis != NULL) {
		if (variaveis->tipo != TC_STRING) {
			variaveis = caudaDaLista(variaveis);
			return NULL;
		}

		variavel_t* variavel = malloc(sizeof(variavel_t));
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

programa_t* criarNoPrograma(char* nome, lista_t *variaveis, lista_t *filhos, int linha) {
	programa_t* programa = malloc(sizeof(programa_t));
	programa->nome = malloc(sizeof(char) * strlen(nome) + 1);
	strcpy(programa->nome, nome);
	programa->variaveis = variaveis;
	programa->filhos = filhos;
	programa->linha = linha;

	return programa;
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

ast_node_t* criarNoDeclaracaoVar(lista_t* vars, int tipo, int linha) {
	ast_node_t* no = malloc(sizeof(ast_node_t));
	no->tipo = TAN_DECLARACAO_VAR;
	no->filhos = addListaNaLista(vars, NULL);
	no->valor.iVal = tipo;
	no->linha = linha;
	return no;
}

bool criarTabelaDeSimbolos(programa_t *programa) {
	if (programa == NULL) {
		return false;
	}

	lista_t *declaracoes = programa->variaveis;
	int posicaoMemoria = 0;
	while (declaracoes != NULL) {
		ast_node_t* declaracao = declaracoes->valor.astNode;
		programa->tabela_simbolos = adicionaListaVariaveisNaTabelaDeSimbolos(
			declaracao->filhos->valor.lista,
			declaracao->valor.iVal,
			programa->tabela_simbolos,
			&posicaoMemoria,
			declaracao->linha
		);
		if (programa->tabela_simbolos == NULL) {
			return false;
		}
		declaracoes = caudaDaLista(declaracoes);
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
	printf("** Linha %d: Tipos incompativeis. Esperado: %s, encontrado: %s.\n", linha, retornaNomeDoTipo(tipoUm), retornaNomeDoTipo(tipoDois));
}

variavel_t* buscaVariavel(char *chave, mapa_t *mapa, int linha) {
	variavel_t* variavel;
	variavel = buscarVariavelNoMapa(chave, mapa);

	if (variavel == NULL) {
		printf("** Linha %d: Variável \"%s\" não declarada.\n", linha, chave);
		return NULL;
	}

	return variavel;
}

bool verificarTipoDaExpressao(programa_t *programa, ast_node_t* expressao) {
	ast_node_t *lhsNode = NULL;
	ast_node_t *rhsNode = NULL;
	variavel_t* variavel;
	switch (expressao->tipo) {
	case TAN_VARIAVEL:
		variavel = buscaVariavel(expressao->valor.strVal, programa->tabela_simbolos, expressao->linha);
		if (variavel != NULL) {
			expressao->tipo_dados = variavel->tipo;
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
			verificarTipoDaExpressao(programa, lhsNode);
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			verificarTipoDaExpressao(programa, rhsNode);
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
			verificarTipoDaExpressao(programa, lhsNode);
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			verificarTipoDaExpressao(programa, rhsNode);
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
			verificarTipoDaExpressao(programa, lhsNode);
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
			verificarTipoDaExpressao(programa, lhsNode);
		}
		if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
			verificarTipoDaExpressao(programa, rhsNode);
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
			verificarTipoDaExpressao(programa, lhsNode);
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

bool verificarTipoAtribuicao(programa_t *programa, ast_node_t* atribuicao) {
	lista_t *lhs = atribuicao->filhos;
	lista_t *rhs = caudaDaLista(atribuicao->filhos);
	ast_node_t *lhsNode = lhs->valor.astNode;
	ast_node_t *rhsNode = rhs->valor.astNode;

	if (lhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		variavel_t* variavel = buscaVariavel(lhsNode->valor.strVal, programa->tabela_simbolos, lhsNode->linha);
		if (variavel == NULL) {
			return false;
		}
		lhsNode->tipo_dados = variavel->tipo;
	}

	if (rhsNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoDaExpressao(programa, rhsNode);
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

bool verificarTipoIf(programa_t *programa, ast_node_t* atribuicao) {
	lista_t *condicao = atribuicao->filhos;
	lista_t *then_stmts = caudaDaLista(atribuicao->filhos);
	lista_t *else_stmts = caudaDaLista(then_stmts);

	ast_node_t *condicaoNode = condicao->valor.astNode;

	if (condicaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoDaExpressao(programa, condicaoNode);
	}

	if (condicaoNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
		imprimirMensagemDeErroDeTipo(condicaoNode->linha, condicaoNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
		return false;
	}

	if (then_stmts != NULL) {
		verificarTiposDosStatements(programa, then_stmts->valor.lista);
	}

	if (else_stmts != NULL) {
		verificarTiposDosStatements(programa, else_stmts->valor.lista);
	}
	return true;
}

bool verificarTipoWhile(programa_t *programa, ast_node_t* atribuicao) {
	lista_t *condicao = atribuicao->filhos;
	lista_t *codigo = caudaDaLista(atribuicao->filhos);

	ast_node_t *condicaoNode = condicao->valor.astNode;

	if (condicaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoDaExpressao(programa, condicaoNode);
	}

	if (condicaoNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
		imprimirMensagemDeErroDeTipo(condicaoNode->linha, condicaoNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
		return false;
	}

	if (codigo != NULL) {
		verificarTiposDosStatements(programa, codigo->valor.lista);
	}
	return true;
}

bool verificarTipoRepeat(programa_t *programa, ast_node_t* atribuicao) {
	lista_t *condicao = atribuicao->filhos;
	lista_t *codigo = caudaDaLista(atribuicao->filhos);

	ast_node_t *condicaoNode = condicao->valor.astNode;

	if (condicaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoDaExpressao(programa, condicaoNode);
	}

	if (codigo != NULL) {
		verificarTiposDosStatements(programa, codigo->valor.lista);
	}

	if (condicaoNode->tipo_dados != TIPO_PRIMITIVO_BOOLEAN) {
		imprimirMensagemDeErroDeTipo(condicaoNode->linha, condicaoNode->tipo_dados, TIPO_PRIMITIVO_BOOLEAN);
		return false;
	}
	return true;
}

bool verificarTipoFor(programa_t *programa, ast_node_t* atribuicao) {
	lista_t *inicializacao = atribuicao->filhos;
	lista_t *ate = caudaDaLista(inicializacao);
	lista_t *codigo = caudaDaLista(ate);

	ast_node_t *inicializacaoNode = inicializacao->valor.astNode;
	ast_node_t *ateNode = ate->valor.astNode;

	if (inicializacaoNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoAtribuicao(programa, inicializacaoNode);
	}

	if (inicializacaoNode->tipo_dados != TIPO_PRIMITIVO_INTEGER && inicializacaoNode->tipo_dados != TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		imprimirMensagemDeErroDeTipo(inicializacaoNode->linha, inicializacaoNode->tipo_dados, TIPO_PRIMITIVO_INTEGER);
		return false;
	}


	if (ateNode->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		verificarTipoDaExpressao(programa, ateNode);
	}

	if (ateNode->tipo_dados != TIPO_PRIMITIVO_INTEGER) {
		imprimirMensagemDeErroDeTipo(ateNode->linha, ateNode->tipo_dados, TIPO_PRIMITIVO_INTEGER);
		return false;
	}


	if (codigo != NULL) {
		verificarTiposDosStatements(programa, codigo->valor.lista);
	}
	return true;
}

bool verificarTiposDosStatements(programa_t *programa, lista_t *statements) {
	bool sucesso = true;
	while (statements != NULL) {
		ast_node_t* statement = statements->valor.astNode;
		if (statement == NULL) {
			return true;
		}

		switch (statement->tipo) {
		case TAN_ATRIBUICAO:
			sucesso = verificarTipoAtribuicao(programa, statement);
			break;
		case TAN_IF:
			sucesso = verificarTipoIf(programa, statement);
			break;
		case TAN_WHILE:
			sucesso = verificarTipoWhile(programa, statement);
			break;
		case TAN_REPEAT:
			sucesso = verificarTipoRepeat(programa, statement);
			break;
		case TAN_FORTO:
		case TAN_FORDOWNTO:
			sucesso = verificarTipoFor(programa, statement);
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
	return verificarTiposDosStatements(programa, programa->filhos);
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
	case TAN_DECLARACAO_VAR:
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
	char *buffer = malloc(sizeof(char) * tamanhoAssembly * 2);

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
	*posicaoAssembly += strlen(noAST->valor.strVal);
	free(buffer);
}

void gerarAssemblyNoAst(
	programa_t* programa,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
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
	case TAN_DECLARACAO_VAR:
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
	int comprimentoAssembly = 1024;
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

	printf("Assembly do programa:\n\n\n\n%s", assembly);
	return assembly;
}
