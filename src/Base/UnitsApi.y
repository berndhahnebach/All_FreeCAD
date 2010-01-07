/* Parser for the FreeCAD  Units language           */
/* (c) 2010 Juergen Riegel  LGPL                    */


/* Represents the many different ways we can access our data */
%{
       #define YYSTYPE double
       #include <math.h>
       #include <stdio.h>
 %}

     /* Bison declarations.  */
     %token NUM
     %token UNIT
     %left '-' '+'
     %left '*' '/'
     %left NEG     /* negation--unary minus */
     %right '^'    /* exponentiation */



%start input

%%

    input:     exp                { ScanResult = $1     ;    }
             | exp uexp           { ScanResult = $1 * $2;    }
             | uexp               { ScanResult = $1     ;    }
 ;     
     
     exp:      NUM                { $$ = $1;         }
             | exp '+' exp        { $$ = $1 + $3;    }
             | exp '-' exp        { $$ = $1 - $3;    }
             | exp '*' exp        { $$ = $1 * $3;    }
             | exp '/' exp        { $$ = $1 / $3;    }
             | '-' exp  %prec NEG { $$ = -$2;        }
             | exp '^' exp        { $$ = pow ($1, $3); }
             | '(' exp ')'        { $$ = $2;         }
             | exp uexp           { $$ = $1 * $2;    }
 ;            
     uexp:     UNIT               { $$ = $1;         }
             | uexp '*' uexp      { $$ = $1 * $3;    }
             | uexp '/' uexp      { $$ = $1 / $3;    }
             | uexp '^' exp       { $$ = pow ($1, $3); }
             | '(' uexp ')'       { $$ = $2;         }
 ;

%%
