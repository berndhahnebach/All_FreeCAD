%{
/* Lexer for the FreeCAD  Units language   */
/* (c) 2010 Juergen Riegel  LGPL           */

#include <string>

#include "stdio.h"


/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* change the name of the scanner class. results in "SelectionFilterFlexLexer" */
%option prefix="UnitsApi"

/* the manual says "somewhat more optimized" */
%option batch

/* no support for include files is planned */
%option noyywrap nounput 

DIGIT    [0-9]
ID       [a-z][a-z0-9]*


%% /*** Filter language Part ***/

 
[ \t]                   ;
[\n]+                   ;

"mm"                    yylval = 1.0;    return UNIT; // Milimeter
"m"                     yylval = 1000.0; return UNIT; // Meter
"cm"                    yylval = 100.0;  return UNIT; // Centimeter



{DIGIT}+"."{DIGIT}*    {yylval = atof( yytext ); return NUM;}
{DIGIT}+               {yylval = atof( yytext ); return NUM;}
                     

 
