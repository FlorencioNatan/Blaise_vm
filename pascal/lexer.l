%{
#include "../lista.h"
#include "../ast.h"
#include "parser.h"
#include <string.h>
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
char* copiarString(char* fonte);
%}

%option noyywrap


DIGIT    [0-9]
LETTER    [a-zA-Z]
HEX_DIGIT    [0-9a-fA-F]
   
%%

(?i:program)            {return KW_PROGRAM;}
(?i:begin)              {return KW_BEGIN;}
(?i:end)                {return KW_END;}
(?i:var)                {return KW_VAR;}
(?i:char)               {return KW_CHAR;}
(?i:boolean)            {return KW_BOOLEAN;}
(?i:integer)            {return KW_INTEGER;}
(?i:real)               {return KW_REAL;}
(?i:string)             {return KW_STRING;}
"-"                     {return MINUS;}
"+"                     {return PLUS;}
"*"                     {return MULT;}
"/"                     {return DIV;}
"="                     {return EQUAL;}
"("                     {return L_PAREN;}
")"                     {return R_PAREN;}
";"                     {return SEMICOLON;}
":"                     {return COLON;}
"."                     {return PERIOD;}
","                     {return COMMA;}


[ \t\n]+         {/* ignore spaces */}

(\.{DIGIT}+)|({DIGIT}+\.{DIGIT}+)       {yylval.rval = atof(yytext); return REAL;}
({DIGIT}+)                              {yylval.ival = atoi(yytext); return INTEGER;}
({LETTER}+({DIGIT}|{LETTER})*)          {yylval.identVal = copiarString(yytext); return IDENTIFIER;}


.              {printf("Error at line %d: unrecognized symbol \"%s\"\n", yylloc.first_line, yytext); exit(0);}

%%

char* copiarString(char* fonte) {
	char* novaString = malloc((strlen(fonte)+1) * sizeof(char));
	strcpy(novaString, fonte);
	return novaString;
}
