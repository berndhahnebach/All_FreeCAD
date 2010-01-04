/* Parser for the FreeCAD selection filter language */
/* (c) 2010 Juergen Riegel  LGPL                    */


/* Represents the many different ways we can access our data */
%union {
    std::string *string;
    Node_Object *object;
    Node_Slice  *slice;
    Node_Block  *block;
    int          token;
    int          number;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER 
%token <token> TFROM TSELECT TCOUNT TSLICE
%token <number> TNUMBER

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above.
 */
%type <slice> slice count
%type <object> matchline
%type <string> namespc
%type <block>  matchlines block

%start filter

%%

slice : TNUMBER TSLICE TNUMBER    { $$ = new Node_Slice($1,$3) }
      | TNUMBER TSLICE            { $$ = new Node_Slice($1) }
      | TNUMBER                   { $$ = new Node_Slice($1,$1) }
      
count : TCOUNT slice              { $$ = $2 }
			
namespc : TFROM TIDENTIFIER       { $$ = $2 }

matchline  : namespc TSELECT TIDENTIFIER count { $$ = new Node_Object($1,$3,$4) }
           | namespc TSELECT TIDENTIFIER       { $$ = new Node_Object($1,$3,0) }
           | TSELECT TIDENTIFIER count         { $$ = new Node_Object(new std::string(),$2,$3) }
           | TSELECT TIDENTIFIER               { $$ = new Node_Object(new std::string(),$2,0 ) }

matchlines : matchline            { $$ = new Node_Block($1);  }
           | matchlines matchline { $$ = $1 ; $$->Objects.push_back($2); }
           
block : matchlines                { $$ = $1 }

filter:   block					  { TopBlock = $1 }
;


%%
