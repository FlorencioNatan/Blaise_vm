%{
#include "../lista.h"
#include "../ast.h"
#include "parser.h"
#include <string.h>
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
char* copiarString(char* fonte);
char buff[1024];
char *s;
%}

%option noyywrap
%option yylineno
%x ASSEMBLY

DIGIT    [0-9]
LETTER    [a-zA-Z]
HEX_DIGIT    [0-9a-fA-F]
   
%%

(?i:program)            {return KW_PROGRAM;}
(?i:begin)              {return KW_BEGIN;}
<INITIAL>(?i:end)       {return KW_END;}
(?i:var)                {return KW_VAR;}
(?i:char)               {return KW_CHAR;}
(?i:boolean)            {return KW_BOOLEAN;}
(?i:integer)            {return KW_INTEGER;}
(?i:real)               {return KW_REAL;}
(?i:string)             {return KW_STRING;}

(?i:and)                {return KW_AND;}
(?i:or)                 {return KW_OR;}
(?i:not)                {return KW_NOT;}

(?i:if)                 {return KW_IF;}
(?i:then)               {return KW_THEN;}
(?i:else)               {return KW_ELSE;}
(?i:while)              {return KW_WHILE;}
(?i:do)                 {return KW_DO;}
(?i:repeat)             {return KW_REPEAT;}
(?i:until)              {return KW_UNTIL;}
(?i:for)                {return KW_FOR;}
(?i:to)                 {return KW_TO;}
(?i:downto)             {return KW_DOWNTO;}

"-"                     {return MENOS;}
"+"                     {return MAIS;}
"*"                     {return MULTIPLICACAO;}
"/"                     {return DIVISAO;}
"="                     {return IGUAL;}

"<>"                    {return DIFERENTE;}
">"                     {return MAIOR;}
">="                    {return MAIOR_IGUAL;}
"<"                     {return MENOR;}
"<="                    {return MENOR_IGUAL;}

"("                     {return L_PAREN;}
")"                     {return R_PAREN;}
";"                     {return PONTO_E_VIRGULA;}
":"                     {return DOIS_PONTOS;}
"."                     {return PONTO;}
","                     {return VIRGULA;}

":="                    {return WALRUS;}

<INITIAL>(?i:asm)       { BEGIN ASSEMBLY; s = buff; }
<ASSEMBLY>(?i:end)      { BEGIN INITIAL; *s = '\0'; yylval.asmVal = buff; return ASM; }
<ASSEMBLY>(.|"\n")             { *s++ = *yytext; }

[ \t\n]+         {/* ignore spaces */}

(\.{DIGIT}+)|({DIGIT}+\.{DIGIT}+)       {yylval.rval = atof(yytext); return REAL;}
({DIGIT}+)                              {yylval.ival = atoi(yytext); return INTEGER;}
({LETTER}+({DIGIT}|{LETTER})*)          {yylval.identVal = copiarString(yytext); return IDENTIFICADOR;}


.              {printf("Error at line %d: unrecognized symbol \"%s\"\n", yylloc.first_line, yytext); exit(0);}

%%


char* copiarString(char* fonte) {
	char* novaString = malloc((strlen(fonte)+1) * sizeof(char));
	strcpy(novaString, fonte);
	return novaString;
}
