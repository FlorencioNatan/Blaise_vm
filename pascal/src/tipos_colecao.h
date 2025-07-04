#ifndef TIPOS_COLECAO_H
#define TIPOS_COLECAO_H

typedef struct node ast_node_t;
typedef struct variavel variavel_t;

typedef union {
	char* strVal;
	int iVal;
	variavel_t *varVal;
	ast_node_t *astNode;
} valores_colecao;

typedef enum {
	TC_INT,
	TC_STRING,
	TC_VARIAVEL,
	TC_AST_NODE
} tipos_colecao;

#endif /* TIPOS_COLECAO_H */
