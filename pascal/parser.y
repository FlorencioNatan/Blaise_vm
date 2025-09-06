/* calculator. */
%{
 #include <stdio.h>
 #include <stdlib.h>
 #include "lexer.h"
 #include "../lista.h"
 #include "../mapa.h"
 #include "../ast.h"
 void yyerror(const char *msg);

mapa_t *tabela_simbolos;

%}

%union{
  double rval;
  int ival;
  char* identVal;
  programa_t* prog; // define the pointer type for custom data structure
  lista_t* lista;
  mapa_t* mapa;
  int tipo;
  ast_node_t* node;
}


%define parse.error verbose
%locations

%start init
%token MULT DIV PLUS MINUS EQUAL L_PAREN R_PAREN SEMICOLON PERIOD COLON COMMA               DIFF GREATER_THAN GREATER_EQUAL LESSER_THAN LESSER_EQUAL
%token KW_PROGRAM KW_BEGIN KW_END KW_VAR KW_CHAR KW_BOOLEAN KW_INTEGER KW_REAL KW_STRING    KW_AND KW_OR KW_NOT
%token <rval> REAL
%token <ival> INTEGER
%token <identVal> IDENTIFIER
%type <node> exp
%type <node> logic_exp
%type <lista> statement
%type <tipo> var_tipos
%type <lista> var_lista
%type <mapa> var_declaracao
%left KW_AND KW_OR KW_NOT
%left EQUAL DIFF GREATER_THAN GREATER_EQUAL LESSER_THAN LESSER_EQUAL
%left PLUS MINUS
%left MULT DIV
%nonassoc UMINUS





%% 
init:	KW_PROGRAM IDENTIFIER SEMICOLON var_declaracao KW_BEGIN statement KW_END PERIOD { programa_t* programa = criarNoPrograma($2, $4, $6); printAST(programa); /*printf("%s", $2);*/ }

var_declaracao:	{/* */}
					| KW_VAR var_linhas_declaracao { $$ = tabela_simbolos; /* printMapa(tabela_simbolos); */ }

var_linhas_declaracao:	var_linha_declaracao SEMICOLON              {  }
						| var_linhas_declaracao var_linha_declaracao SEMICOLON {  }

var_linha_declaracao:	var_lista COLON var_tipos { /* $$ = $1; */ tabela_simbolos = adicionaListaVariaveisNaTabelaDeSimbolos($1, $3, tabela_simbolos); }

var_lista:	IDENTIFIER { $$ = addStringNaLista($1, NULL); }
		| IDENTIFIER COMMA var_lista { $$ = addStringNaLista($1, $3); }

var_tipos:	KW_CHAR               { $$ = TIPO_PRIMITIVO_CHAR; }
			| KW_BOOLEAN          { $$ = TIPO_PRIMITIVO_BOOLEAN; }
			| KW_INTEGER          { $$ = TIPO_PRIMITIVO_INTEGER; }
			| KW_REAL             { $$ = TIPO_PRIMITIVO_REAL; }
			| KW_STRING           { $$ = TIPO_PRIMITIVO_STRING; }

statement: {  }
            | exp SEMICOLON { $$ = addNoASTNaLista($1, NULL); };

exp:		REAL                  { $$ = criarNoReal($1); }
			| INTEGER             { $$ = criarNoInteger($1); }
			| exp PLUS exp        { $$ = criarNoBinario($1, $3, TAN_SOMA); }
			| exp MINUS exp       { $$ = criarNoBinario($1, $3, TAN_SUBTRACAO); }
			| exp MULT exp        { $$ = criarNoBinario($1, $3, TAN_MULTIPLICACAO); }
			| exp DIV exp         { if ($3==0) yyerror("divide by zero"); else $$ = criarNoBinario($1, $3, TAN_DIVISAO); }
			| MINUS exp %prec UMINUS { $$ = criarNoUnario($2, TAN_NEGATIVACAO); }
			| logic_exp
			| L_PAREN exp R_PAREN { $$ = $2; }
			;

logic_exp: 	exp EQUAL exp       { $$ = criarNoBinario($1, $3, TAN_IGUAL); }
			| exp DIFF exp       { $$ = criarNoBinario($1, $3, TAN_DIFERENTE); }
			| exp GREATER_THAN exp       { $$ = criarNoBinario($1, $3, TAN_MAIOR); }
			| exp GREATER_EQUAL exp       { $$ = criarNoBinario($1, $3, TAN_MAIOR_IGUAL); }
			| exp LESSER_THAN exp       { $$ = criarNoBinario($1, $3, TAN_MENOR); }
			| exp LESSER_EQUAL exp       { $$ = criarNoBinario($1, $3, TAN_MENOR_IGUAL); }

			| exp KW_AND exp       { $$ = criarNoBinario($1, $3, TAN_AND); }
			| exp KW_OR exp       { $$ = criarNoBinario($1, $3, TAN_OR); }
			| KW_NOT exp       { $$ = criarNoUnario($2, TAN_NOT); }
			;


%%

int main(int argc, char **argv) {
   if (argc > 1) {
      yyin = fopen(argv[1], "r");
      if (yyin == NULL){
         printf("syntax: %s filename\n", argv[0]);
      }//end if
   }//end if
   yyparse(); // Calls yylex() for tokens.
   return 0;
}

void yyerror(const char *msg) {
   printf("** Line %d: %s\n", yylloc.first_line, msg);
}

