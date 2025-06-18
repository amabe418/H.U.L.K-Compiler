%error-verbose
%locations

%{
  /* Definir manualmente YYLTYPE */
  typedef struct YYLTYPE {
      int first_line;
      int first_column;
      int last_line;
      int last_column;
  } YYLTYPE;
  #define YYLTYPE_IS_DECLARED 1

  #include <cstdio>
  #include <cstdlib>
  #include "AST/ast.hpp"
  extern Program* rootAST;
  extern int yylex(void);
  extern FILE* yyin;
  extern int yylineno;

  /* 1) Función interna que maneja ubicación detallada */
  static void yyerror_loc(YYLTYPE *locp, const char *msg) {
      std::fprintf(stderr,
          "Error de parseo en  línea %d, columna %d: %s\n",
          locp->first_line,
          locp->first_column,
          msg);
      std::exit(EXIT_FAILURE);
  }

  /* 2) Firma que Bison invoca CON ubicación (2 args) */
  void yyerror(YYLTYPE *locp, const char *msg) {
      yyerror_loc(locp, msg);
  }

  /* 3) Firma que Bison invoca SIN ubicación (1 arg) */
  void yyerror(const char *msg) {
      /* `yylloc` es la variable global que Bison mantiene */
      extern YYLTYPE yylloc;
      yyerror_loc(&yylloc, msg);
  }
%}



%union {
  char* str;
  Expr* expr;
  Stmt* stmt;
  Program* prog;
  std::vector<StmtPtr>* stmts;
  std::vector<ExprPtr>* expr_list;
  std::vector<std::string>* str_list;
  std::pair<std::string, Expr*>* binding;
  std::vector<std::pair<std::string, Expr*>>* bindings;
  std::pair<
      std::vector<std::unique_ptr<AttributeDecl>>,
      std::vector<std::unique_ptr<MethodDecl>>
    >* mems;
  std::pair<AttributeDecl*, MethodDecl*>* member_pair; 
  AttributeDecl*           attribute_decl;             
  MethodDecl*              method_decl;               
}

%start input

%type  <stmt> stmt
%type  <expr> expr 
%type  <prog> input
%type  <prog> program
%type <stmts> stmt_list
%type <binding> binding
%type <str_list> ident_list
%type <bindings> binding_list 
%type <expr> if_expr elif_list
%type <expr_list> argument_list
%type <stmt> type_decl
%type <mems> member_list
%type <member_pair> member
%type <attribute_decl> attribute
%type <method_decl> method


%token LET IN 
%token WHILE FOR
%token <str> IDENT
%token IF ELSE ELIF
%token FUNCTION ARROW
%token ASSIGN ASSIGN_DESTRUCT
%token TYPE DOT SELF NEW INHERITS BASE
%token <expr> TRUE FALSE NUMBER STRING
%token PLUS MINUS MULT DIV MOD POW CONCAT
%token LE GE EQ NEQ LESS_THAN GREATER_THAN OR AND
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON

%left OR
%left AND
%left EQ NEQ
%left LESS_THAN GREATER_THAN LE GE
%left PLUS MINUS
%left MULT DIV MOD
%left CONCAT
%right POW
%left UMINUS

%%

input:
    program { rootAST = $1; }
  

program:
    /* vacío */      { $$ = new Program(); }
  | program stmt SEMICOLON
    {
      $1->stmts.emplace_back( StmtPtr($2) );
      $$ = $1;
    }
;    

binding_list:
    binding {
          $$ = new std::vector<std::pair<std::string, Expr*>>();
          $$->push_back(*$1);
          delete $1;
      }
    | binding_list COMMA binding {
          $1->push_back(*$3);
          delete $3;
          $$ = $1;
      }
;

binding:
      IDENT ASSIGN expr {
          $$ = new std::pair<std::string, Expr*>(std::string($1), $3);
          free($1);
      }
;

stmt:
    
   expr { $$ = new ExprStmt( ExprPtr($1) ); }

  | FUNCTION IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE {
          std::vector<std::string> args = std::move(*$4);
          delete $4;

          auto block = std::make_unique<Program>();
          block->stmts = std::move(*$7);
          delete $7;

          $$ = new FunctionDecl(std::string($2), std::move(args), std::move(block));
          free($2);
      }
  | FUNCTION IDENT LPAREN ident_list RPAREN ARROW expr  {
          std::vector<std::string> args = std::move(*$4);
          delete $4;

          $$ = new FunctionDecl(std::string($2), std::move(args), StmtPtr(new ExprStmt(ExprPtr($7))));
          free($2);
      }
  | type_decl  { $$ = $1; }    
;    



stmt_list:
      /* vacío */ { $$ = new std::vector<StmtPtr>(); }
    | stmt_list stmt {
        $1->emplace_back(StmtPtr($2));
        $$ = $1;
    }
;

ident_list:
      /* vacío */ { $$ = new std::vector<std::string>(); }
    | IDENT { $$ = new std::vector<std::string>(); $$->push_back($1); free($1); }
    | ident_list COMMA IDENT { $1->push_back($3); free($3); $$ = $1; }
;

expr:
      NUMBER            { $$ = $1; }
    | STRING            { $$ = $1; }
    | TRUE              { $$ = $1; }
    | FALSE             { $$ = $1; }

    | LBRACE stmt_list RBRACE {
          $$ = new ExprBlock(std::move(*$2));
          delete $2;
      }

    | IDENT LPAREN argument_list RPAREN {
          $$ = new CallExpr(std::string($1), std::move(*$3));
          delete $3;
          free($1);
      }

    | IDENT {
          $$ = new VariableExpr(std::string($1));
          free($1);
      }

    | MINUS expr %prec UMINUS {
          $$ = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr($2));
      }

    | expr POW expr {
          $$ = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr($1), ExprPtr($3));
      }

    | expr MULT expr {
          $$ = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr($1), ExprPtr($3));
      }

    | expr DIV expr {
          $$ = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr($1), ExprPtr($3));
      }

    | expr MOD expr %prec MULT {
          $$ = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr($1), ExprPtr($3));
      }


    | expr PLUS expr {
          $$ = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr($1), ExprPtr($3));
      }

    | expr MINUS expr {
          $$ = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr($1), ExprPtr($3));
      }

    | expr LESS_THAN expr {
          $$ = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr($1), ExprPtr($3));
      }

    | expr GREATER_THAN expr {
          $$ = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr($1), ExprPtr($3));
      }

    | expr LE expr {
          $$ = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr($1), ExprPtr($3));
      }

    | expr GE expr {
          $$ = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr($1), ExprPtr($3));
      }

    | expr EQ expr {
          $$ = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr($1), ExprPtr($3));
      }

    | expr NEQ expr {
          $$ = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr($1), ExprPtr($3));
      }

    | expr AND expr {
          $$ = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr($1), ExprPtr($3));
      }

    | expr OR expr {
          $$ = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr($1), ExprPtr($3));
      }

    | expr CONCAT expr {
        // Creamos un BinaryExpr con OP_CONCAT
        $$ = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr($1), ExprPtr($3));
    } 

    | expr CONCAT CONCAT expr {
        auto space = new StringExpr(" ");
        $$ = new BinaryExpr(BinaryExpr::OP_CONCAT,
                ExprPtr(new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr($1), ExprPtr(space))),
                ExprPtr($4));
    }
 

    | LPAREN expr RPAREN {
          $$ = $2;
      }

    | LET binding_list IN expr {
          Expr* result = $4;
          auto& list = *$2;

          for (auto it = list.rbegin(); it != list.rend(); ++it) {
              result = new LetExpr(it->first, ExprPtr(it->second), std::make_unique<ExprStmt>(ExprPtr(result)));
          }

          delete $2;
          $$ = result;      
        }

    | IDENT ASSIGN_DESTRUCT expr {
          $$ = new AssignExpr(std::string($1), ExprPtr($3));
          free($1);
      }
    | WHILE LPAREN expr RPAREN expr {
      $$ = new WhileExpr(ExprPtr($3), ExprPtr($5));
    }

    | if_expr  
    | FOR LPAREN IDENT IN expr RPAREN expr {
        auto argsNext = std::vector<ExprPtr>();
        argsNext.push_back(std::make_unique<VariableExpr>("__iter"));
        ExprPtr callNext = std::make_unique<CallExpr>("next", std::move(argsNext));

        auto argsCurrent = std::vector<ExprPtr>();
        argsCurrent.push_back(std::make_unique<VariableExpr>("__iter"));
        ExprPtr callCurrent = std::make_unique<CallExpr>("current", std::move(argsCurrent));

        ExprPtr bodyFor = ExprPtr($7);

        Expr* innerLetRaw = new LetExpr(
            std::string($3),                      // nombre de la variable (x)
            std::move(callCurrent),               // initializer = current(__iter)
            StmtPtr(new ExprStmt(std::move(bodyFor))) // cuerpo = ExprStmt(bodyFor)
        );

        Expr* whileRaw = new WhileExpr(
            std::move(callNext),
            ExprPtr(innerLetRaw)
        );

        auto argsIter = std::vector<ExprPtr>();
        argsIter.push_back( ExprPtr($5) );
        ExprPtr callIter = std::make_unique<CallExpr>("iter", std::move(argsIter));

        Expr* outerLetRaw = new LetExpr(
            "__iter",
            std::move(callIter),
            StmtPtr(new ExprStmt( ExprPtr(whileRaw) ))
        );

        free($3);  // liberar el IDENT
        $$ = outerLetRaw;
    }

  | NEW IDENT LPAREN argument_list RPAREN {
        $$ = new NewExpr(std::string($2), std::move(*$4));
        delete $4; free($2);
    }
    /* sin argumentos */
  | NEW IDENT {
        $$ = new NewExpr(std::string($2), {});
        free($2);
    }
  | expr DOT IDENT {
        $$ = new GetAttrExpr(ExprPtr($1), $3);
        free($3);
    }
  | expr DOT IDENT ASSIGN_DESTRUCT expr {
        $$ = new SetAttrExpr(ExprPtr($1), $3, ExprPtr($5));
        free($3);
    }
  | expr DOT IDENT LPAREN argument_list RPAREN {
        $$ = new MethodCallExpr(ExprPtr($1), $3, std::move(*$5));
        delete $5;
        free($3);
    }
  | SELF {
        $$ = new SelfExpr();
    }  
  | BASE LPAREN RPAREN {
        $$ = new BaseCallExpr({});
    }  

;

if_expr:
    IF LPAREN expr RPAREN expr elif_list {
        $$ = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr($6));
    }
;

elif_list:
    ELSE expr {
        $$ = $2;
    }
    | ELIF LPAREN expr RPAREN expr elif_list {
        $$ = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr($6));
    }
;

argument_list:
      /* vacío */ { $$ = new std::vector<ExprPtr>(); }
    | expr {
          $$ = new std::vector<ExprPtr>();
          $$->emplace_back(ExprPtr($1));
      }
    | argument_list COMMA expr {
          $1->emplace_back(ExprPtr($3));
          $$ = $1;
      }
; 

type_decl:
    TYPE IDENT LPAREN ident_list RPAREN INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE {
        $$ = new TypeDecl(
            std::string($2),
            std::move(*$4),
            std::move($12->first),
            std::move($12->second),
            std::string($7),
            std::move(*$9)
        );
        delete $4; delete $9; delete $12;
        free($2); free($7);
    }
  | TYPE IDENT INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE {
        $$ = new TypeDecl(
            std::string($2),
            {},
            std::move($9->first),
            std::move($9->second),
            std::string($4),
            std::move(*$6)
        );
        delete $6; delete $9;
        free($2); free($4);
    }
| TYPE IDENT INHERITS IDENT LBRACE member_list RBRACE {
        $$ = new TypeDecl(
            std::string($2),
            {}, // sin parámetros propios
            std::move($6->first),
            std::move($6->second),
            std::string($4), // tipo base
            {} // sin baseArgs explícitos: herencia implícita
        );
        delete $6;
        free($2); free($4);
    }
  | TYPE IDENT LPAREN ident_list RPAREN LBRACE member_list RBRACE {
        $$ = new TypeDecl(
            std::string($2),
            std::move(*$4),
            std::move($7->first),
            std::move($7->second)
        );
        delete $4; delete $7;
        free($2);
    }
  | TYPE IDENT LBRACE member_list RBRACE {
        $$ = new TypeDecl(
            std::string($2),
            {},
            std::move($4->first),
            std::move($4->second)
        );
        delete $4; free($2);
    }
;

member_list:
    /* empty */ { 
        auto* pair = new std::pair<
            std::vector<std::unique_ptr<AttributeDecl>>,
            std::vector<std::unique_ptr<MethodDecl>>
        >();
        $$ = pair;
    }
    | member_list member {
        if ($2->first) {
            $1->first.push_back(std::unique_ptr<AttributeDecl>($2->first));
        } else {
            $1->second.push_back(std::unique_ptr<MethodDecl>($2->second));
        }
        delete $2;
        $$ = $1;
    }
;

member:
    attribute SEMICOLON {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->first = $1;
        $$ = pair;
    }
    | method SEMICOLON {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->second = $1;
        $$ = pair;
    }
;

attribute:
    IDENT ASSIGN expr {
        $$ = new AttributeDecl($1, ExprPtr($3));
        free($1);
    }
;

method:
    IDENT LPAREN ident_list RPAREN ARROW expr {
        std::vector<std::string> args = $3 ? std::move(*$3) : std::vector<std::string>();
        delete $3;
        $$ = new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($6))));
        free($1);
    }
    | IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> args = $3 ? std::move(*$3) : std::vector<std::string>();
        delete $3;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$6);
        delete $6;
        $$ = new MethodDecl($1, std::move(args), StmtPtr(std::move(block)));
        free($1);
    }
;


%%