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
programa_t* programa;

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
  char* asmVal;
}


%define parse.error verbose
%locations

%start init
%token MULTIPLICACAO DIVISAO MAIS MENOS IGUAL L_PAREN R_PAREN PONTO_E_VIRGULA PONTO DOIS_PONTOS VIRGULA DIFERENCA MAIOR MAIOR_IGUAL MENOR MENOR_IGUAL WALRUS L_COLCH R_COLCH PONTO_PONTO
%token KW_PROGRAM KW_BEGIN KW_END KW_VAR KW_CHAR KW_BOOLEAN KW_INTEGER KW_REAL KW_STRING    KW_AND KW_OR KW_NOT KW_IF KW_THEN KW_ELSE KW_WHILE KW_DO KW_REPEAT KW_UNTIL KW_FOR KW_TO KW_DOWNTO KW_ARRAY KW_OF KW_PROCEDURE KW_FUNCTION KW_EXIT
%token <rval> REAL
%token <ival> INTEGER
%token <identVal> IDENTIFICADOR
%token <asmVal> ASM
%type <node> exp
%type <node> statement
%type <node> atribuicao_statement
%type <node> if_statement
%type <node> while_statment
%type <node> repeat_statment
%type <node> for_statment
%type <lista> statements
%type <tipo> var_tipos_primitivos
%type <lista> identificador_lista
%type <node> var_lista
%type <lista> var_listas
%type <lista> var_declaracao
%type <node> var_tipos_array
%type <lista> exp_lista
%type <node> chamada_subrotina
%type <node> exit
%type <node> subrotina_declaracao
%type <lista> subrotinas_declaracoes
%left KW_AND KW_OR KW_NOT
%left IGUAL DIFERENTE MAIOR MAIOR_IGUAL MENOR MENOR_IGUAL
%left MAIS MENOS
%left MULTIPLICACAO DIVISAO
%nonassoc UMINUS





%% 
init:	KW_PROGRAM IDENTIFICADOR PONTO_E_VIRGULA subrotinas_declaracoes var_declaracao KW_BEGIN statements KW_END PONTO { programa = criarNoPrograma($2, $4, $5, $7, yylloc.first_line); free($2); /*printAST(programa); printf("%s", $2);*/ }

subrotinas_declaracoes: { $$ = NULL; }
                        | subrotina_declaracao PONTO_E_VIRGULA subrotinas_declaracoes { $$ = addNoASTNaLista($1, $3); }
                        ;

subrotina_declaracao: KW_PROCEDURE IDENTIFICADOR L_PAREN var_listas R_PAREN PONTO_E_VIRGULA var_declaracao KW_BEGIN statements KW_END { $$ = criarNoProcedure($2, $4, $7, $9, yylloc.first_line); free($2); }
                    | KW_FUNCTION IDENTIFICADOR L_PAREN var_listas R_PAREN DOIS_PONTOS var_tipos_primitivos PONTO_E_VIRGULA var_declaracao KW_BEGIN statements KW_END { $$ = criarNoFunction($2, $4, $9, $11, $7, yylloc.first_line); free($2); }
                    ;

var_declaracao:	{ $$ = NULL; }
					| KW_VAR var_listas PONTO_E_VIRGULA { $$ = $2; /* printMapa(tabela_simbolos); */ }
					;

var_listas:	var_lista                              { $$ = addNoASTNaLista($1, NULL); }
			| var_listas PONTO_E_VIRGULA var_lista { $$ = addNoASTNaLista($3, $1); }
			;

var_lista:	identificador_lista DOIS_PONTOS var_tipos_primitivos { $$ = criarNoListaVar($1, $3, yylloc.first_line);/* $$ = $1; tabela_simbolos = adicionaListaVariaveisNaTabelaDeSimbolos($1, $3, tabela_simbolos);*/ }
			| identificador_lista DOIS_PONTOS var_tipos_array    { $$ = criarNoDeclaracaoArrayVar($1, $3, yylloc.first_line); }
			;

identificador_lista:  IDENTIFICADOR { $$ = addStringNaLista($1, NULL); free($1); }
                      | IDENTIFICADOR VIRGULA identificador_lista { $$ = addStringNaLista($1, $3); free($1); }
                      ;

var_tipos_primitivos:	KW_CHAR               { $$ = TIPO_PRIMITIVO_CHAR; }
						| KW_BOOLEAN          { $$ = TIPO_PRIMITIVO_BOOLEAN; }
						| KW_INTEGER          { $$ = TIPO_PRIMITIVO_INTEGER; }
						| KW_REAL             { $$ = TIPO_PRIMITIVO_REAL; }
						| KW_STRING           { $$ = TIPO_PRIMITIVO_STRING; }
						;

var_tipos_array:	KW_ARRAY L_COLCH INTEGER PONTO_PONTO INTEGER R_COLCH KW_OF var_tipos_primitivos    { $$ = criarNoTipoArray($3, $5, $8, yylloc.first_line); }
					;

statement: { $$ = NULL; }
            | atribuicao_statement { $$ = $1; }
            | if_statement { $$ = $1; }
            | while_statment { $$ = $1; }
            | repeat_statment { $$ = $1; }
            | for_statment { $$ = $1; }
            | ASM { $$ = criarNoASM($1, yylloc.first_line); }
            | chamada_subrotina { $$ = $1; }
            | exit { $$ = $1; }
            ;

statements: statement { $$ = addNoASTNaLista($1, NULL); }
            | statement PONTO_E_VIRGULA statements { $$ = addNoASTNaLista($1, $3); }
            ;

atribuicao_statement: IDENTIFICADOR WALRUS exp { $$ = criarNoAtribuicao($1, $3, yylloc.first_line); free($1); };

if_statement: KW_IF exp KW_THEN KW_BEGIN statements KW_END { $$ = criarNoIf($2, $5, NULL, yylloc.first_line); }
			  | KW_IF exp KW_THEN KW_BEGIN statements KW_END KW_ELSE KW_BEGIN statements KW_END { $$ = criarNoIf($2, $5, $9, yylloc.first_line); }
			  ;

while_statment: KW_WHILE exp KW_DO KW_BEGIN statements KW_END { $$ = criarNoWhile($2, $5, yylloc.first_line); };

repeat_statment: KW_REPEAT statements KW_UNTIL exp { $$ = criarNoRepeat($4, $2, yylloc.first_line); };

for_statment: KW_FOR atribuicao_statement KW_TO exp KW_DO KW_BEGIN statements KW_END { $$ = criarNoForTo($2, $4, $7, yylloc.first_line); }
			  | KW_FOR atribuicao_statement KW_DOWNTO exp KW_DO KW_BEGIN statements KW_END { $$ = criarNoForDownTo($2, $4, $7, yylloc.first_line); }
			  ;

chamada_subrotina: IDENTIFICADOR L_PAREN exp_lista R_PAREN       { $$ = criarNoChamadaSubrotina($1, $3, yylloc.first_line); free($1); };

exit: KW_EXIT L_PAREN exp R_PAREN       { $$ = criarNoExit($3, yylloc.first_line); };

exp_lista: exp                     { $$ = addNoASTNaLista($1, NULL); }
           | exp VIRGULA exp_lista { $$ = addNoASTNaLista($1, $3); }
           ;

exp:		REAL                  { $$ = criarNoReal($1, yylloc.first_line); }
			| INTEGER             { $$ = criarNoInteger($1, yylloc.first_line); }
			| IDENTIFICADOR       { $$ = criarNoVariavel($1, yylloc.first_line); free($1); }
			| IDENTIFICADOR L_COLCH exp R_COLCH       { $$ = criarNoAcessoArray($1, $3, yylloc.first_line); free($1); }
			| chamada_subrotina   {$$ = $1;}
			| exp MAIS exp        { $$ = criarNoBinario($1, $3, TAN_SOMA, yylloc.first_line); }
			| exp MENOS exp       { $$ = criarNoBinario($1, $3, TAN_SUBTRACAO, yylloc.first_line); }
			| exp MULTIPLICACAO exp        { $$ = criarNoBinario($1, $3, TAN_MULTIPLICACAO, yylloc.first_line); }
			| exp DIVISAO exp         { if ($3==0) yyerror("divide by zero"); else $$ = criarNoBinario($1, $3, TAN_DIVISAO, yylloc.first_line); }
			| MENOS exp %prec UMINUS { $$ = criarNoUnario($2, TAN_NEGATIVACAO, yylloc.first_line); }

			| exp IGUAL exp       { $$ = criarNoBinario($1, $3, TAN_IGUAL, yylloc.first_line); }
			| exp DIFERENTE exp       { $$ = criarNoBinario($1, $3, TAN_DIFERENTE, yylloc.first_line); }
			| exp MAIOR exp       { $$ = criarNoBinario($1, $3, TAN_MAIOR, yylloc.first_line); }
			| exp MAIOR_IGUAL exp       { $$ = criarNoBinario($1, $3, TAN_MAIOR_IGUAL, yylloc.first_line); }
			| exp MENOR exp       { $$ = criarNoBinario($1, $3, TAN_MENOR, yylloc.first_line); }
			| exp MENOR_IGUAL exp       { $$ = criarNoBinario($1, $3, TAN_MENOR_IGUAL, yylloc.first_line); }

			| exp KW_AND exp       { $$ = criarNoBinario($1, $3, TAN_AND, yylloc.first_line); }
			| exp KW_OR exp       { $$ = criarNoBinario($1, $3, TAN_OR, yylloc.first_line); }
			| KW_NOT exp       { $$ = criarNoUnario($2, TAN_NOT, yylloc.first_line); }
			| L_PAREN exp R_PAREN { $$ = $2; }
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
   bool sucesso = true;
   sucesso = criarTabelaDeSimbolos(programa);
   if (sucesso) {
       sucesso = verificarTiposDoPrograma(programa);
   }
   if (sucesso) {
       char* assembly = gerarAssembly(programa);
       printf("Assembly do programa:\n\n\n\n%s", assembly);
       free(assembly);
   }
   return 0;
}

void yyerror(const char *msg) {
   printf("** Linha %d: %s\n", yylloc.first_line, msg);
}

