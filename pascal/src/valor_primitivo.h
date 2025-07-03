#ifndef valor_primitivo_H
#define valor_primitivo_H

#define TIPO_PRIMITIVO_INTEGER 0
#define TIPO_PRIMITIVO_STRING 1
#define TIPO_PRIMITIVO_CHAR 2
#define TIPO_PRIMITIVO_BOOLEAN 3
#define TIPO_PRIMITIVO_REAL 4

typedef union {
	int iVal;
	char* strVal;
	char chVal;
	int boolVal;
	double realVal;
} valor_primitivo;

#endif /* valor_primitivo_H */
