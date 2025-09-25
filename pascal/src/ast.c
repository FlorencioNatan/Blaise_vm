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
