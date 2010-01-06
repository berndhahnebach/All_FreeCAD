/* Parser for the FreeCAD selection Units language */
/* (c) 2010 Juergen Riegel  LGPL                    */


/* Represents the many different ways we can access our data */
%union {
    double       number;
    std::string *string;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER 
%token <token> TFROM TSELECT TCOUNT TSLICE
%token <number> TNUMBER

%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above.
 */
%type <number> exp

%start input

%%

    input:    /* empty */
             | input line
     ;
     
     line:     '\n'
             | exp '\n'  { printf ("\t%.10g\n", $1); }
     ;
     
     exp:      TNUMBER            { $$ = $1;         }
             | exp '+' exp        { $$ = $1 + $3;    }
             | exp '-' exp        { $$ = $1 - $3;    }
             | exp '*' exp        { $$ = $1 * $3;    }
             | exp '/' exp        { $$ = $1 / $3;    }
             | '-' exp  %prec NEG { $$ = -$2;        }
             | exp '^' exp        { $$ = pow ($1, $3); }
             | '(' exp ')'        { $$ = $2;         }
     ;


%%
