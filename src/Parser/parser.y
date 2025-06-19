%error-verbose
%locations
%debug

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
  #include "Types/type_info.hpp"
  extern Program* rootAST;
  extern int yylex(void);
  extern FILE* yyin;
  extern int yylineno;

  /* 1) Función interna que maneja ubicación detallada */
  static void yyerror_loc(YYLTYPE *locp, const char *msg) {
      // Asegurarnos de que la línea no sea mayor que el número real de líneas
      int line = locp->first_line;
      if (line < 1) line = 1;
      
      // Obtener el número de línea actual del lexer
      extern int yylineno;
      if (line > yylineno) {
          line = yylineno;
      }
      
      std::fprintf(stderr,
          "Error de parseo en línea %d, columna %d: %s\n",
          line,
          locp->first_column,
          msg);
      
      // Información adicional de debug
      extern char* yytext;
      std::fprintf(stderr, "Token actual: '%s'\n", yytext ? yytext : "NULL");
      
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

  #include <string>
  #include <vector>
  #include <memory>
  #include <iostream>
  #include <cstdlib>
  #include <cstring>

  void yyerror(const char* s);
  int yylex();

  extern char* yytext;

  // Inicialización por defecto para TypeInfo
  std::shared_ptr<TypeInfo> defaultTypeInfo() {
      return std::make_shared<TypeInfo>(TypeInfo::Void());
  }
%}



%union {
  Expr* expr;
  Stmt* stmt;
  Program* prog;
  std::vector<StmtPtr>* stmts;
  std::vector<ExprPtr>* expr_list;
  std::vector<std::string>* str_list;
  std::pair<std::string, Expr*>* binding;
  std::vector<std::pair<std::string, Expr*>>* bindings;
  std::vector<std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>>* bindings_with_types;
  std::pair<
      std::vector<std::unique_ptr<AttributeDecl>>,
      std::vector<std::unique_ptr<MethodDecl>>
    >* mems;
  std::pair<AttributeDecl*, MethodDecl*>* member_pair; 
    AttributeDecl* attribute_decl;
    MethodDecl* method_decl;
    std::shared_ptr<TypeInfo>* type_info;
    std::vector<std::shared_ptr<TypeInfo>>* type_info_list;
    std::pair<std::string, std::shared_ptr<TypeInfo>>* param_with_type;
    std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>* param_list;
    std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>* binding_with_type;
    char* str;
}

%start input

%type  <stmt> stmt
%type  <expr> expr 
%type  <prog> input
%type  <prog> program
%type <stmts> stmt_list
%type <str_list> ident_list
%type <bindings_with_types> binding_list
%type <expr> if_expr
%type <expr_list> argument_list
%type <stmt> type_decl
%type <mems> member_list
%type <member_pair> member
%type <attribute_decl> attribute
%type <method_decl> method
%type <type_info> type
%type <param_with_type> param
%type <param_list> param_list
%type <binding_with_type> binding


%token LET IN 
%token WHILE FOR
%token <str> IDENT
%token IF ELSE ELIF
%token FUNCTION ARROW
%token ASSIGN ASSIGN_DESTRUCT
%token TYPE DOT SELF NEW INHERITS BASE
%token <expr> TRUE FALSE NUMBER STRING
%token PLUS MINUS MULT DIV MOD POW CONCAT CONCAT_WS
%token LE GE EQ NEQ LESS_THAN GREATER_THAN OR AND
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON COLON
%token INT FLOAT BOOL VOID


%right ASSIGN_DESTRUCT
%right ASSIGN
%left OR
%left AND
%left EQ NEQ
%left CONCAT CONCAT_WS
%left LESS_THAN GREATER_THAN LE GE
%left PLUS MINUS
%left MULT DIV MOD
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
  | program expr SEMICOLON
    {
      $1->stmts.emplace_back( StmtPtr(new ExprStmt(ExprPtr($2))) );
      $$ = $1;
    }
;    

binding_list:
    binding {
          $$ = new std::vector<std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>>();
          $$->push_back(std::make_pair($1->first, std::make_pair($1->second.first, $1->second.second)));
          delete $1;
      }
    | binding_list COMMA binding {
          $1->push_back(std::make_pair($3->first, std::make_pair($3->second.first, $3->second.second)));
          delete $3;
          $$ = $1;
      }
;

binding:
      IDENT ASSIGN expr {
          $$ = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string($1), 
              std::make_pair($3, std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown))
          );
          free($1);
      }
      | IDENT COLON type ASSIGN expr {
          $$ = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string($1), 
              std::make_pair($5, *$3)
          );
          free($1);
          delete $3;
      }
;

stmt:
    expr  {
        (yyval.stmt) = (new ExprStmt(ExprPtr((yyvsp[(1) - (1)].expr))));
    }
    | FUNCTION IDENT LPAREN param_list RPAREN COLON type LBRACE stmt_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$9);
        delete $9;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::move(block), 
            std::move(param_types),
            *$7
        ));
        delete $4;
        delete $7;
        free($2);
    }
    | FUNCTION IDENT LPAREN param_list RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$7);
        delete $7;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::move(block), 
            std::move(param_types)
        ));
        delete $4;
        free($2);
    }
    | FUNCTION IDENT LPAREN param_list RPAREN ARROW expr {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr($7)), 
            std::move(param_types)
        ));
        delete $4;
        free($2);
    }
    | FUNCTION IDENT LPAREN param_list RPAREN COLON type ARROW expr {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr($9)), 
            std::move(param_types),
            *$7
        ));
        delete $4;
        delete $7;
        free($2);
    }
    | FUNCTION IDENT LPAREN RPAREN ARROW expr {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr($6)), 
            std::move(param_types)
        ));
        free($2);
    }
    | FUNCTION IDENT LPAREN RPAREN COLON type ARROW expr {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr($8)), 
            std::move(param_types),
            *$6
        ));
        delete $6;
        free($2);
    }
    | type_decl
;    



stmt_list:
    /* vacío */ { $$ = new std::vector<StmtPtr>(); }
  | stmt_list stmt SEMICOLON { 
      $1->push_back(StmtPtr($2)); 
      $$ = $1; 
    }
  | stmt_list expr SEMICOLON { 
      $1->push_back(StmtPtr(new ExprStmt(ExprPtr($2)))); 
      $$ = $1; 
    }
;

ident_list:
      /* vacío */ { $$ = new std::vector<std::string>(); }
    | IDENT { $$ = new std::vector<std::string>(); $$->push_back($1); free($1); }
    | ident_list COMMA IDENT { $1->push_back($3); free($3); $$ = $1; }
;

expr:
    NUMBER { std::cout << "Evaluated expression statement" << std::endl; $$ = $1; }
  | STRING { $$ = $1; }
  | TRUE { $$ = $1; }
  | FALSE { $$ = $1; }
  | IDENT { $$ = new VariableExpr($1); free($1); }
  | expr PLUS expr { $$ = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr($1), ExprPtr($3)); }
  | expr MINUS expr { $$ = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr($1), ExprPtr($3)); }
  | expr MULT expr { $$ = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr($1), ExprPtr($3)); }
  | expr DIV expr { $$ = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr($1), ExprPtr($3)); }
  | expr MOD expr { $$ = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr($1), ExprPtr($3)); }
  | expr POW expr { $$ = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr($1), ExprPtr($3)); }
  | expr CONCAT expr { $$ = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr($1), ExprPtr($3)); }
  | expr CONCAT_WS expr { $$ = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr($1), ExprPtr($3)); }
  | expr LESS_THAN expr { $$ = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr($1), ExprPtr($3)); }
  | expr GREATER_THAN expr { $$ = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr($1), ExprPtr($3)); }
  | expr LE expr { $$ = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr($1), ExprPtr($3)); }
  | expr GE expr { $$ = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr($1), ExprPtr($3)); }
  | expr EQ expr { $$ = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr($1), ExprPtr($3)); }
  | expr NEQ expr { $$ = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr($1), ExprPtr($3)); }
  | expr AND expr { $$ = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr($1), ExprPtr($3)); }
  | expr OR expr { $$ = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr($1), ExprPtr($3)); }
  | MINUS expr %prec UMINUS { $$ = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr($2)); }
  | IDENT LPAREN argument_list RPAREN { 
      std::vector<ExprPtr> args;
      for (auto& arg : *$3) {
          args.push_back(std::move(arg));
      }
      $$ = new CallExpr($1, std::move(args)); 
      free($1); 
      delete $3; 
  }
  | IDENT LPAREN RPAREN { $$ = new CallExpr($1, std::vector<ExprPtr>()); free($1); }
  | NEW IDENT LPAREN argument_list RPAREN {
      std::vector<ExprPtr> args;
      for (auto& arg : *$4) {
          args.push_back(std::move(arg));
      }
      $$ = new NewExpr($2, std::move(args));
      free($2);
      delete $4;
  }
  | NEW IDENT LPAREN RPAREN {
      $$ = new NewExpr($2, std::vector<ExprPtr>());
      free($2);
  }
  | expr DOT IDENT LPAREN argument_list RPAREN {
      std::vector<ExprPtr> args;
      for (auto& arg : *$5) {
          args.push_back(std::move(arg));
      }
      $$ = new MethodCallExpr(ExprPtr($1), $3, std::move(args));
      free($3);
      delete $5;
  }
  | expr DOT IDENT LPAREN RPAREN {
      $$ = new MethodCallExpr(ExprPtr($1), $3, std::vector<ExprPtr>());
      free($3);
  }
  | BASE LPAREN argument_list RPAREN {
      std::vector<ExprPtr> args;
      for (auto& arg : *$3) {
          args.push_back(std::move(arg));
      }
      $$ = new BaseCallExpr(std::move(args));
      delete $3;
  }
  | BASE LPAREN RPAREN {
      $$ = new BaseCallExpr(std::vector<ExprPtr>());
  }
  | LET binding_list IN expr { 
      // Desugar multiple bindings into nested LetExpr
      // Start from the last binding and work backwards
      Expr* result = $4;
      
      // Process bindings in reverse order to maintain correct nesting
      for (int i = $2->size() - 1; i >= 0; --i) {
          const auto& binding = (*$2)[i];
          std::string name = binding.first;
          Expr* init = binding.second.first;
          std::shared_ptr<TypeInfo> type = binding.second.second;
          
          if (i == $2->size() - 1) {
              // Last binding: body is the original expression
              result = new LetExpr(name, ExprPtr(init), 
                                  std::make_unique<ExprStmt>(ExprPtr(result)), type);
          } else {
              // Other bindings: body is the nested LetExpr
              result = new LetExpr(name, ExprPtr(init), 
                                  std::make_unique<ExprStmt>(ExprPtr(result)), type);
          }
      }
      
      $$ = result;
      delete $2; 
  }
  | LET IDENT COLON type ASSIGN expr IN expr {
      $$ = new LetExpr(std::string($2), ExprPtr($6), 
                      std::make_unique<ExprStmt>(ExprPtr($8)), *$4);
      delete $4;
      free($2);
  }
  | LET IDENT ASSIGN expr IN expr {
      $$ = new LetExpr(std::string($2), ExprPtr($4), 
                      std::make_unique<ExprStmt>(ExprPtr($6)));
      free($2);
  }
  | if_expr
  
  | WHILE expr LBRACE stmt_list RBRACE { 
      auto block = ExprPtr(new ExprBlock(std::move(*$4)));
      $$ = new WhileExpr(ExprPtr($2), std::move(block)); 
      delete $4;
  }
  | FOR LPAREN IDENT IN expr RPAREN expr {
      $$ = new ForExpr(std::string($3), ExprPtr($5), ExprPtr($7));
      free($3);
  }
  | FOR LPAREN IDENT IN expr RPAREN LBRACE stmt_list RBRACE {
      auto block = ExprPtr(new ExprBlock(std::move(*$8)));
      $$ = new ForExpr(std::string($3), ExprPtr($5), std::move(block));
      free($3);
      delete $8;
  }
  | LBRACE stmt_list RBRACE { 
      $$ = new ExprBlock(std::move(*$2)); 
      delete $2;
  }
  | LPAREN expr RPAREN { $$ = $2; }
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
  | IDENT ASSIGN_DESTRUCT expr {
        $$ = new AssignExpr(std::string($1), ExprPtr($3));
        free($1);
    }
;



if_expr:
    IF LPAREN expr RPAREN expr ELSE expr {
        $$ = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr($7));
    }
    | IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*$6)));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*$10)));
      $$ = new IfExpr(ExprPtr($3), std::move(ifBlock), std::move(elseBlock)); 
      delete $6;
      delete $10;
  }
  | IF LPAREN expr RPAREN LBRACE stmt_list RBRACE { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*$6)));
      $$ = new IfExpr(ExprPtr($3), std::move(ifBlock), nullptr); 
      delete $6;
  }
;

argument_list:
    /* empty */
    { (yyval.expr_list) = new std::vector<ExprPtr>(); }
    | expr
    {
        (yyval.expr_list) = new std::vector<ExprPtr>();
        (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[(1) - (1)].expr)));
      }
    | argument_list COMMA expr
    {
        (yyvsp[(1) - (3)].expr_list)->emplace_back(ExprPtr((yyvsp[(3) - (3)].expr)));
        (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list);
      }
; 

type_decl:
    TYPE IDENT LPAREN ident_list RPAREN INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            std::move(*$4),
            std::move($12->first),
            std::move($12->second),
            std::string($7),
            std::move(*$9)
        ));
        delete $4; delete $9; delete $12;
        free($2); free($7);
    }
  | TYPE IDENT INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            {},
            std::move($9->first),
            std::move($9->second),
            std::string($4),
            std::move(*$6)
        ));
        delete $6; delete $9;
        free($2); free($4);
    }
| TYPE IDENT INHERITS IDENT LBRACE member_list RBRACE {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            {}, // sin parámetros propios
            std::move($6->first),
            std::move($6->second),
            std::string($4), // tipo base
            {} // sin baseArgs explícitos: herencia implícita
        ));
        delete $6;
        free($2); free($4);
    }
  | TYPE IDENT LPAREN ident_list RPAREN LBRACE member_list RBRACE {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            std::move(*$4),
            std::move($7->first),
            std::move($7->second)
        ));
        delete $4; delete $7;
        free($2);
    }
  | TYPE IDENT LBRACE member_list RBRACE {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            {},
            std::move($4->first),
            std::move($4->second)
        ));
        delete $4; free($2);
    }
;

member_list:
    /* empty */ { 
        auto* pair = new std::pair<
            std::vector<std::unique_ptr<AttributeDecl>>,
            std::vector<std::unique_ptr<MethodDecl>>
        >();
        (yyval.mems) = pair;
    }
    | member_list member {
        if ($2->first) {
            $1->first.push_back(std::unique_ptr<AttributeDecl>($2->first));
        } else {
            $1->second.push_back(std::unique_ptr<MethodDecl>($2->second));
        }
        delete $2;
        (yyval.mems) = $1;
    }
;

member:
    attribute SEMICOLON {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->first = $1;
        (yyval.member_pair) = pair;
    }
    | method SEMICOLON {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->second = $1;
        (yyval.member_pair) = pair;
    }
;

attribute:
    IDENT ASSIGN expr {
        (yyval.attribute_decl) = static_cast<AttributeDecl*>(new AttributeDecl($1, ExprPtr($3)));
        free($1);
    }
;

method:
    IDENT LPAREN ident_list RPAREN ARROW expr {
        std::vector<std::string> args = $3 ? std::move(*$3) : std::vector<std::string>();
        delete $3;
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($6)))));
        free($1);
    }
    | IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> args = $3 ? std::move(*$3) : std::vector<std::string>();
        delete $3;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$6);
        delete $6;
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(std::move(block))));
        free($1);
    }
    | IDENT LPAREN RPAREN ARROW expr {
        std::vector<std::string> args;
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($5)))));
        free($1);
    }
;

type:
    IDENT { 
        // Si es un tipo conocido, usarlo, sino usar Object
        std::string type_name($1);
        if (type_name == "Number") {
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::Number()));
        } else if (type_name == "Boolean") {
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::Boolean()));
        } else if (type_name == "String") {
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::String()));
        } else {
            // Para cualquier otro tipo, usar Object y guardar el nombre del tipo
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::Object(type_name)));
        }
        free($1);
    }
;

param_list:
    /* empty */ { $$ = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>(); }
    | param { 
        $$ = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>();
        $$->push_back(*$1);
        delete $1;
    }
    | param_list COMMA param {
        $1->push_back(*$3);
        delete $3;
        $$ = $1;
    }
;

param:
    IDENT { 
        $$ = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string($1), 
            std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)
        );
        free($1);
    }
    | IDENT COLON type {
        $$ = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string($1), 
            *$3
        );
        free($1);
        delete $3;
    }
;

%%