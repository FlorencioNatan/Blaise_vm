#ifndef TIPOS_COLECAO_H
#define TIPOS_COLECAO_H

typedef struct node ast_node_t;
typedef struct variavel variavel_t;
typedef struct lista lista_t;

typedef union {
	char* strVal;
	int iVal;
	variavel_t *varVal;
	ast_node_t *astNode;
	lista_t *lista;
} valores_colecao;

typedef enum {
	TC_INT,
	TC_STRING,
	TC_VARIAVEL,
	TC_AST_NODE,
	TC_LISTA,
} tipos_colecao;

#endif /* TIPOS_COLECAO_H */
