#include <string.h>
#include "geracao_de_codigo.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct contadores {
	int If;
	int While;
	int Repeat;
	int For;
	int comparacao;
	int logico;
} contadores_t;

void gerarAssemblyNoAst(
	mapa_t* tabela_simbolos,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
);

void gerarAssemblyListaNoAst(
	mapa_t* tabela_simbolos,
	lista_t* listaNo,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
);

void gerarAssemblyAtribuicao(
	mapa_t* tabela_simbolos,
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

	variavel_t* variavel = buscaVariavel(lhsNode->valor.strVal, tabela_simbolos, lhsNode->linha);

	char buffer[256] = "";

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    push 8\n    lw\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	switch (variavel->tipo) {
	case TIPO_PRIMITIVO_INTEGER:
		sprintf(buffer, "    push %d\n    sub\n    sh\n", variavel->posicaoNaMemoria + 4);
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
		sprintf(buffer, "    push %d\n    sub\n    sw\n", variavel->posicaoNaMemoria + 8);
		break;
	}
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

void gerarAssemblyObterValorDaVariavel(
	mapa_t* tabela_simbolos,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly
) {
	variavel_t* variavel = buscaVariavel(noAST->valor.strVal, tabela_simbolos, noAST->linha);

	char buffer[256] = "";
	sprintf(buffer, "    push 8\n    lw\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	switch (variavel->tipo) {
	case TIPO_PRIMITIVO_INTEGER:
		sprintf(buffer, "    push %d\n    sub\n    lh\n", variavel->posicaoNaMemoria + 4);
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
		sprintf(buffer, "    push %d\n    sub\n    lw\n", variavel->posicaoNaMemoria + 8);
		break;
	}
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

void gerarAssemblySoma(
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
		strcpy(&assembly[*posicaoAssembly], "    add\n");
		*posicaoAssembly += strlen("    add\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    addf\n");
		*posicaoAssembly += strlen("    addf\n");
	}
}

void gerarAssemblySubtracao(
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}
}

void gerarAssemblyMultiplicacao(
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
		strcpy(&assembly[*posicaoAssembly], "    mul\n");
		*posicaoAssembly += strlen("    mul\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    mulf\n");
		*posicaoAssembly += strlen("    mulf\n");
	}
}

void gerarAssemblyDivisao(
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
		strcpy(&assembly[*posicaoAssembly], "    div\n");
		*posicaoAssembly += strlen("    div\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    divf\n");
		*posicaoAssembly += strlen("    divf\n");
	}
}

void gerarAssemblyNegativacao(
	mapa_t* tabela_simbolos,
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
	gerarAssemblyNoAst(tabela_simbolos, opNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (opNode->tipo_dados == TIPO_PRIMITIVO_INTEGER) {
		strcpy(&assembly[*posicaoAssembly], "    sub\n");
		*posicaoAssembly += strlen("    sub\n");
	} else {
		strcpy(&assembly[*posicaoAssembly], "    subf\n");
		*posicaoAssembly += strlen("    subf\n");
	}
}

void gerarAssemblyIgual(
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	if (lhsNode->tipo_dados != TIPO_PRIMITIVO_REAL && rhsNode->tipo_dados != TIPO_PRIMITIVO_REAL) {
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";
	sprintf(
		buffer,
		"    beqi falso_and_%d\n",
		contadores->logico
	);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	strcpy(buffer,"");

	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
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
	mapa_t* tabela_simbolos,
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

	gerarAssemblyNoAst(tabela_simbolos, rhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";
	sprintf(
		buffer,
		"    bnei true_or_%d\n",
		contadores->logico
	);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	strcpy(buffer,"");

	gerarAssemblyNoAst(tabela_simbolos, lhsNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
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
	mapa_t* tabela_simbolos,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t *lhs = noAST->filhos;
	ast_node_t *opNode = lhs->valor.astNode;

	gerarAssemblyNoAst(tabela_simbolos, opNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

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
	mapa_t* tabela_simbolos,
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
	gerarAssemblyNoAst(tabela_simbolos, condicaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    beqi else_if_%d\nthan_if_%d:\n", contadores->If, contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyListaNoAst(tabela_simbolos, then_stmtLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    jumpi fim_if_%d:\n", contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "");
	sprintf(buffer, "else_if_%d:\n", contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	if (else_stmtLista != NULL) {
		gerarAssemblyListaNoAst(tabela_simbolos, else_stmtLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	}
	strcpy(buffer, "");
	sprintf(buffer, "fim_if_%d:\n", contadores->If);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->If++;
}

void gerarAssemblyWhile(
	mapa_t* tabela_simbolos,
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
	gerarAssemblyNoAst(tabela_simbolos, condicaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    beqi fim_while_%d\n", contadores->While);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyListaNoAst(tabela_simbolos, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);


	strcpy(buffer, "");
	sprintf(buffer, "    jumpi inicio_while_%d\nfim_while_%d:\n", contadores->While, contadores->While);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->While++;
}

void gerarAssemblyRepeat(
	mapa_t* tabela_simbolos,
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

	gerarAssemblyListaNoAst(tabela_simbolos, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	gerarAssemblyNoAst(tabela_simbolos, condicaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	strcpy(buffer, "");
	sprintf(buffer, "    bnei fim_repeat_%d\n    jumpi inicio_repeat_%d\n\nfim_repeat_%d:\n", contadores->Repeat, contadores->Repeat, contadores->Repeat);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	contadores->Repeat++;
}

void gerarAssemblyForTo(
	mapa_t* tabela_simbolos,
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

	variavel_t* variavel = buscaVariavel(varNode->valor.strVal, tabela_simbolos, varNode->linha);

	lista_t *codigoLista = codigo->valor.lista;

	char buffer[256] = "";

	gerarAssemblyNoAst(tabela_simbolos, inicializacaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	sprintf(buffer, "inicio_for_%d:\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);


	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n    lh\n", variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyNoAst(tabela_simbolos, ateNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	strcpy(buffer, "");
	sprintf(buffer, "    sub\n    beqi fim_for_%d\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	gerarAssemblyListaNoAst(tabela_simbolos, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

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
	mapa_t* tabela_simbolos,
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

	variavel_t* variavel = buscaVariavel(varNode->valor.strVal, tabela_simbolos, varNode->linha);

	lista_t *codigoLista = codigo->valor.lista;

	char buffer[256] = "";

	gerarAssemblyNoAst(tabela_simbolos, inicializacaoNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	sprintf(buffer, "inicio_for_%d:\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);


	strcpy(buffer, "");
	sprintf(buffer, "    push %d\n    lh\n", variavel->posicaoNaMemoria);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
	gerarAssemblyNoAst(tabela_simbolos, ateNode, assembly, posicaoAssembly, comprimentoAssembly, contadores);
	strcpy(buffer, "");
	sprintf(buffer, "    sub\n    beqi fim_for_%d\n", contadores->For);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	gerarAssemblyListaNoAst(tabela_simbolos, codigoLista, assembly, posicaoAssembly, comprimentoAssembly, contadores);

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
	mapa_t *tabela_simbolos,
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

			variavel_t* variavel = buscaVariavel(varNome, tabela_simbolos, noAST->linha);
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
	mapa_t* tabela_simbolos,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	variavel_t* variavel = buscaVariavel(noAST->valor.strVal, tabela_simbolos, noAST->linha);

	ast_node_t *indice = noAST->filhos->valor.astNode;

	gerarAssemblyNoAst(tabela_simbolos, indice, assembly, posicaoAssembly, comprimentoAssembly, contadores);

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

void gerarAssemblyExit(
	mapa_t* tabela_simbolos,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	ast_node_t* exp = noAST->filhos->valor.astNode;
	gerarAssemblyNoAst(tabela_simbolos, exp, assembly, posicaoAssembly, comprimentoAssembly, contadores);

	char buffer[256] = "";

	strcpy(buffer, "\n# Carrega o PC de onde a funcao foi chamada\n    push 0x8\n    lw\n    lw\n\n# Retorna para função original Valor de retorno vai na pilha de operações\n    jump\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

int retornarComprimentoBasicoNaMemoria(int tipoDeDado) {
	switch (tipoDeDado) {
		case TIPO_PRIMITIVO_INTEGER:
			return COMPRIMENTO_INTEGER_NA_MEMORIA;
		case TIPO_PRIMITIVO_STRING:
			return COMPRIMENTO_STRING_NA_MEMORIA;
		case TIPO_PRIMITIVO_CHAR:
			return COMPRIMENTO_CHAR_NA_MEMORIA;
		case TIPO_PRIMITIVO_BOOLEAN:
			return COMPRIMENTO_BOOLEAN_NA_MEMORIA;
		case TIPO_PRIMITIVO_REAL:
			return COMPRIMENTO_REAL_NA_MEMORIA;
	}
	return COMPRIMENTO_INTEGER_NA_MEMORIA;
}

void gerarAssemblyChamadaSubrotina(
	mapa_t* tabela_simbolos,
	ast_node_t* noAST,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	char buffer[256] = "", prefixo[10] = "function";

	sprintf(buffer, "# Inicia a chamada à subrotina: %s\n", noAST->valor.strVal);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "# incrementa o stack pointer\n    push 0\n    lw\n    push 8\n    sub\n    push 0\n    sw\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "# Adiciona o frame pointer na call stack\n    push 8\n    lw\n    push 0\n    lw\n    sw\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	lista_t *filhosInvertidos = inverterLista(noAST->filhos);
	lista_t *filhos = filhosInvertidos;
	int contadorParametros = 1;
	while (filhos != NULL) {
		ast_node_t* parametro = filhos->valor.astNode;

		sprintf(buffer, "\n# incrementa o stack pointer\n    push 0\n    lw\n    push %d\n    sub\n    push 0\n    sw\n", retornarComprimentoBasicoNaMemoria(parametro->tipo_dados));
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);

		sprintf(buffer, "# Adiciona o parametro %d na call stack\n", contadorParametros);
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);

		gerarAssemblyNoAst(tabela_simbolos, parametro, assembly, posicaoAssembly, comprimentoAssembly, contadores);

		strcpy(buffer, "    push 0\n    lw\n");
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);

		switch (parametro->tipo_dados) {
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
		default:
			strcpy(buffer, "");
			break;
		}

		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);

		contadorParametros++;
		filhos = caudaDaLista(filhos);
	}
	freeLista(filhosInvertidos);

	if (noAST->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		strcpy(prefixo, "procedure");
	}

	strcpy(buffer, "\n# Coloca o frame pointer como stack pointer\n    push 0\n    lw\n    push 8\n    sw\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	sprintf(buffer, "\n    calli %s_%s\n", prefixo, noAST->valor.strVal);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	int comprimento_parametros_stack = 0;
	if (noAST->tipo_dados == TIPO_PRIMITIVO_NAO_PREENCHIDO) {
		procedure_t *pro = buscarProcedureNoMapa(noAST->valor.strVal, tabela_simbolos);
		comprimento_parametros_stack = pro ->comprimento_parametros_stack;
	} else {
		function_t *fun = buscarFunctionNoMapa(noAST->valor.strVal, tabela_simbolos);
		comprimento_parametros_stack = fun->comprimento_parametros_stack;
	}

	sprintf(buffer, "\n# Restaura o frame pointer\n    push 8\n    lw\n    push %d\n    add\n    lw\n    push 8\n    sw\n\n", comprimento_parametros_stack);
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "\n# Restaura o stack pointer\n    push 8\n    lw\n    push 4\n    sub\n    push 0\n    sw\n\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

void gerarAssemblyNoAst(
	mapa_t* tabela_simbolos,
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
	case TAN_CHAR:
		sprintf(buffer, "    push %d\n", noAST->valor.cVal);
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);
		break;
	case TAN_BOOLEAN:
		sprintf(buffer, "    push %d\n", noAST->valor.iVal);
		strcpy(&assembly[*posicaoAssembly], buffer);
		*posicaoAssembly += strlen(buffer);
		break;
	case TAN_SOMA:
		gerarAssemblySoma(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_SUBTRACAO:
		gerarAssemblySubtracao(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MULTIPLICACAO:
		gerarAssemblyMultiplicacao(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_DIVISAO:
		gerarAssemblyDivisao(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_NEGATIVACAO:
		gerarAssemblyNegativacao(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_IGUAL:
		gerarAssemblyIgual(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_DIFERENTE:
		gerarAssemblyDiferente(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MAIOR:
		gerarAssemblyMaior(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MAIOR_IGUAL:
		gerarAssemblyMaiorIgual(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MENOR:
		gerarAssemblyMenor(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_MENOR_IGUAL:
		gerarAssemblyMenorIgual(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_AND:
		gerarAssemblyAND(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_OR:
		gerarAssemblyOR(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_NOT:
		gerarAssemblyNOT(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_VARIAVEL:
		gerarAssemblyObterValorDaVariavel(tabela_simbolos, noAST, assembly, posicaoAssembly);
		break;
	case TAN_ATRIBUICAO:
		gerarAssemblyAtribuicao(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_IF:
		gerarAssemblyIf(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_WHILE:
		gerarAssemblyWhile(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_REPEAT:
		gerarAssemblyRepeat(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_FORTO:
		gerarAssemblyForTo(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_FORDOWNTO:
		gerarAssemblyForDownTo(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_ASM:
		gerarAssemblyAsm(tabela_simbolos, noAST, assembly, posicaoAssembly);
		break;
	case TAN_ACESSO_ARRAY:
		gerarAssemblyObterValorDoArray(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_EXIT:
		gerarAssemblyExit(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_CHAMADA_SUBROTINA:
		gerarAssemblyChamadaSubrotina(tabela_simbolos, noAST, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		break;
	case TAN_LISTA_VAR:
	case TAN_TIPO_ARRAY:
	case TAN_PROCEDURE:
	case TAN_FUNCTION:
		break;
	}
}

void gerarAssemblyListaNoAst(
	mapa_t* tabela_simbolos,
	lista_t* listaNo,
	char *assembly,
	int *posicaoAssembly,
	int *comprimentoAssembly,
	contadores_t *contadores
) {
	lista_t* filhos = listaNo;
	while (filhos != NULL) {
		ast_node_t* valor = filhos->valor.astNode;
		gerarAssemblyNoAst(tabela_simbolos, valor, assembly, posicaoAssembly, comprimentoAssembly, contadores);
		filhos = caudaDaLista(filhos);
	}
}

void salvarEnderecoRetornoNaStack(char *assembly, int *posicaoAssembly) {
	char buffer[256] = "";
	strcpy(buffer, "# incrementa o frame pointer\n    push 8\n    lw\n    push 8\n    sub\n    push 8\n    sw\n\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "# incrementa o stack pointer\n    push 0\n    lw\n    push 8\n    sub\n    push 0\n    sw\n\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);

	strcpy(buffer, "# salva endereco de retorno na stack\n    push 0x8\n    lw\n    sw\n\n");
	strcpy(&assembly[*posicaoAssembly], buffer);
	*posicaoAssembly += strlen(buffer);
}

char* gerarAssembly(programa_t *programa) {
	if (programa == NULL) {
		return NULL;
	}

	char *assembly;
	int comprimentoAssembly = 5000000;
	int posicaoAssembly = 0;
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

	char buffer[256] = "";
	sprintf(buffer, "program_%s:\n", programa->nome);
	strcpy(&assembly[posicaoAssembly], buffer);
	posicaoAssembly += strlen(buffer);
	gerarAssemblyListaNoAst(programa->tabela_simbolos, programa->filhos, assembly, &posicaoAssembly, &comprimentoAssembly, contadores);
	strcpy(&assembly[posicaoAssembly], "    halt\n");
	posicaoAssembly += strlen("    halt\n");

	lista_t *subrotinas = programa->subrotinas;
	while (subrotinas != NULL) {
		ast_node_t* noSubrotinas = subrotinas->valor.astNode;
		lista_t* filhosSubrotina = NULL;
		mapa_t* tabelaSimbolosSubRotinas = NULL;
		int comprimentoMemoria = 0;
		if (noSubrotinas->tipo == TAN_PROCEDURE) {
			procedure_t *pro = noSubrotinas->valor.proVal;
			filhosSubrotina = pro->filhos;
			tabelaSimbolosSubRotinas = pro->tabela_simbolos;
			sprintf(buffer, "\nprocedure_%s:\n", pro->nome);
			comprimentoMemoria = pro->comprimento_memoria_stack;
		} else {
			function_t *fun = noSubrotinas->valor.funVal;
			filhosSubrotina = fun->filhos;
			tabelaSimbolosSubRotinas = fun->tabela_simbolos;
			sprintf(buffer, "\nfunction_%s:\n", fun->nome);
			comprimentoMemoria = fun->comprimento_memoria_stack;
		}
		strcpy(&assembly[posicaoAssembly], buffer);
		posicaoAssembly += strlen(buffer);
		salvarEnderecoRetornoNaStack(assembly, &posicaoAssembly);

		sprintf(buffer, "# incrementa o stack pointer\n    push 0\n    lw\n    push %d\n    sub\n    push 0\n    sw\n\n", comprimentoMemoria);
		strcpy(&assembly[posicaoAssembly], buffer);
		posicaoAssembly += strlen(buffer);

		gerarAssemblyListaNoAst(tabelaSimbolosSubRotinas, filhosSubrotina, assembly, &posicaoAssembly, &comprimentoAssembly, contadores);
		strcpy(&assembly[posicaoAssembly], "    halt\n");
		posicaoAssembly += strlen("    halt\n");
		subrotinas = caudaDaLista(subrotinas);
	}

	int frame_pointer = FIM_MEMORIA_DISPONIVEL;
	int stack_pointer = frame_pointer - programa->comprimento_memoria_stack;
	sprintf(buffer, "\n.data\n0 word 1 %d # stack pointer\n8 word 1 %d # frame pointer\n", stack_pointer, frame_pointer);
	strcpy(&assembly[posicaoAssembly], buffer);

	free(contadores);
	return assembly;
}
