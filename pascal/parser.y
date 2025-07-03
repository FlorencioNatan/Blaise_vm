/* calculator. */
%{
 #include <stdio.h>
 #include <stdlib.h>
 #include "lexer.h"
 #include "../lista.h"
 #include "../mapa.h"
 #include "../ast.h"
 #include "../variavel.h"
 void yyerror(const char *msg);

mapa_t *tabela_simbolos;

%}

%union{
  double dval;
  int ival;
  char* identVal;
  programa_t* prog; // define the pointer type for custom data structure
  lista_t* lista;
  int tipo;
}


%define parse.error verbose
%locations

%start init
%token MULT DIV PLUS MINUS EQUAL L_PAREN R_PAREN SEMICOLON PERIOD COLON COMMA
%token KW_PROGRAM KW_BEGIN KW_END KW_VAR KW_CHAR KW_BOOLEAN KW_INTEGER KW_REAL KW_STRING
%token <dval> REAL
%token <ival> INTEGER
%token <identVal> IDENTIFIER
%type <tipo> var_tipos
%type <lista> var_lista
%left PLUS MINUS
%left MULT DIV
%nonassoc UMINUS


%% 
init:	KW_PROGRAM IDENTIFIER SEMICOLON var_declaracao KW_BEGIN KW_END PERIOD { /*printf("%s", $2);*/ }

var_declaracao:	{/* */}
					| KW_VAR var_linhas_declaracao { /* $$ = $2; */ printMapa(tabela_simbolos); }

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

/*
%type <dval> exp

exp:		REAL                  { $$ = $1; }
			| INTEGER             { $$ = $1; }
			| exp PLUS exp        { $$ = $1 + $3; }
			| exp MINUS exp       { $$ = $1 - $3; }
			| exp MULT exp        { $$ = $1 * $3; }
			| exp DIV exp         { if ($3==0) yyerror("divide by zero"); else $$ = $1 / $3; }
			| MINUS exp %prec UMINUS { $$ = -$2; }
			| L_PAREN exp R_PAREN { $$ = $2; }
			;
*/

