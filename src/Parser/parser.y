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
      std::fprintf(stderr,
          "Error de parseo en línea %d, columna %d: %s\n",
          locp->first_line,
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

  // Helper function to set location information for AST nodes
  template<typename T>
  void setLocation(T* node, const YYLTYPE& loc) {
      if (node) {
          node->line_number = loc.first_line;
          node->column_number = loc.first_column;
      }
  }

  // Helper function to set location information for expressions
  void setExprLocation(Expr* expr, const YYLTYPE& loc) {
      setLocation(expr, loc);
  }

  // Helper function to set location information for statements
  void setStmtLocation(Stmt* stmt, const YYLTYPE& loc) {
      setLocation(stmt, loc);
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
%token TYPE DOT SELF NEW INHERITS BASE IS AS
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
      setStmtLocation($2, @2);
      $1->stmts.emplace_back( StmtPtr($2) );
      $$ = $1;
    }
  | program expr SEMICOLON
    {
      auto exprStmt = new ExprStmt(ExprPtr($2));
      setStmtLocation(exprStmt, @2);
      setExprLocation($2, @2);
      $1->stmts.emplace_back( StmtPtr(exprStmt) );
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
        auto exprStmt = new ExprStmt(ExprPtr((yyvsp[(1) - (1)].expr)));
        setStmtLocation(exprStmt, @1);
        (yyval.stmt) = exprStmt;
    }
    | FUNCTION IDENT LPAREN param_list RPAREN COLON type LBRACE stmt_list RBRACE { 
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$4) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        // Create ExprBlock wrapped in ExprStmt instead of Program
        auto exprBlock = new ExprBlock(std::move(*$9));
        auto exprStmt = new ExprStmt(ExprPtr(exprBlock));
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string($2),
            std::move(args),
            StmtPtr(exprStmt),
            std::move(param_types),
            *$7
        ));
        setStmtLocation(funcDecl, @2);
        (yyval.stmt) = funcDecl;
        delete $4; delete $7; delete $9;
        free($2);
    }
    | FUNCTION IDENT LPAREN param_list RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        // Create ExprBlock wrapped in ExprStmt instead of Program
        auto exprBlock = new ExprBlock(std::move(*$7));
        auto exprStmt = new ExprStmt(ExprPtr(exprBlock));
        delete $7;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            StmtPtr(exprStmt), 
            std::move(param_types)
        ));
        delete $4;
        free($2);
    }

    | FUNCTION IDENT LPAREN param_list RPAREN COLON type ARROW expr {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$4) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(args), 
            std::make_unique<ExprStmt>(ExprPtr($9)), 
            std::move(param_types),
            *$7
        ));
        setStmtLocation(funcDecl, @2);
        (yyval.stmt) = funcDecl;
        delete $4; delete $7;
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

    | FUNCTION IDENT LPAREN RPAREN COLON type ARROW expr {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr($8)), 
            std::move(param_types),
            *$6
        ));
        setStmtLocation(funcDecl, @2);
        (yyval.stmt) = funcDecl;
        delete $6;
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

    | FUNCTION IDENT LPAREN RPAREN COLON type LBRACE stmt_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        // Create ExprBlock wrapped in ExprStmt instead of Program
        auto exprBlock = new ExprBlock(std::move(*$8));
        auto exprStmt = new ExprStmt(ExprPtr(exprBlock));
        delete $8;
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            StmtPtr(exprStmt), 
            std::move(param_types),
            *$6
        ));
        setStmtLocation(funcDecl, @2);
        (yyval.stmt) = funcDecl;
        delete $6;
        free($2);
    }
    | FUNCTION IDENT LPAREN RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        // Create ExprBlock wrapped in ExprStmt instead of Program
        auto exprBlock = new ExprBlock(std::move(*$6));
        auto exprStmt = new ExprStmt(ExprPtr(exprBlock));
        delete $6;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string($2), 
            std::move(params), 
            StmtPtr(exprStmt), 
            std::move(param_types)
        ));
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
    NUMBER { 
        setExprLocation($1, @1);
        std::cout << "Evaluated expression statement" << std::endl; 
        $$ = $1; 
    }
  | STRING { $$ = $1; }
  | TRUE { 
        setExprLocation($1, @1);
        $$ = $1; 
    }
  | FALSE { 
        setExprLocation($1, @1);
        $$ = $1; 
    }
  | IDENT { 
        auto varExpr = new VariableExpr($1);
        setExprLocation(varExpr, @1);
        $$ = varExpr; 
        free($1); 
    }
  | expr PLUS expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr MINUS expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr MULT expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr DIV expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr MOD expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr POW expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr CONCAT expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr CONCAT_WS expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr LESS_THAN expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr GREATER_THAN expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr LE expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr GE expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr EQ expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr NEQ expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr AND expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | expr OR expr { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr($1), ExprPtr($3));
        setExprLocation(binExpr, @2);
        $$ = binExpr; 
    }
  | MINUS expr %prec UMINUS { 
        auto unaryExpr = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr($2));
        setExprLocation(unaryExpr, @1);
        $$ = unaryExpr; 
    }
  | IDENT LPAREN argument_list RPAREN { 
      std::vector<ExprPtr> args;
      for (auto& arg : *$3) {
          args.push_back(std::move(arg));
      }
      auto callExpr = new CallExpr($1, std::move(args));
      setExprLocation(callExpr, @1);
      $$ = callExpr; 
      free($1); 
      delete $3; 
  }
  | IDENT LPAREN RPAREN { 
      auto callExpr = new CallExpr($1, std::vector<ExprPtr>());
      setExprLocation(callExpr, @1);
      $$ = callExpr; 
      free($1); 
  }
  | NEW IDENT LPAREN argument_list RPAREN {
      std::vector<ExprPtr> args;
      for (auto& arg : *$4) {
          args.push_back(std::move(arg));
      }
      auto newExpr = new NewExpr($2, std::move(args));
      setExprLocation(newExpr, @2);
      $$ = newExpr;
      free($2);
      delete $4;
  }
  | NEW IDENT LPAREN RPAREN {
      auto newExpr = new NewExpr($2, std::vector<ExprPtr>());
      setExprLocation(newExpr, @2);
      $$ = newExpr;
      free($2);
  }
  | expr DOT IDENT LPAREN argument_list RPAREN {
      std::vector<ExprPtr> args;
      for (auto& arg : *$5) {
          args.push_back(std::move(arg));
      }
      auto methodCall = new MethodCallExpr(ExprPtr($1), $3, std::move(args));
      setExprLocation(methodCall, @3);
      $$ = methodCall;
      free($3);
      delete $5;
  }
  | expr DOT IDENT LPAREN RPAREN {
      auto methodCall = new MethodCallExpr(ExprPtr($1), $3, std::vector<ExprPtr>());
      setExprLocation(methodCall, @3);
      $$ = methodCall;
      free($3);
  }
  | BASE LPAREN argument_list RPAREN {
      std::vector<ExprPtr> args;
      for (auto& arg : *$3) {
          args.push_back(std::move(arg));
      }
      auto baseCall = new BaseCallExpr(std::move(args));
      setExprLocation(baseCall, @1);
      $$ = baseCall;
      delete $3;
  }
  | BASE LPAREN RPAREN {
      auto baseCall = new BaseCallExpr({});
      setExprLocation(baseCall, @1);
      $$ = baseCall;
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
      
      setExprLocation(result, @1);
      $$ = result;
      delete $2; 
  }
  | LET IDENT COLON type ASSIGN expr IN expr {
      auto letExpr = new LetExpr(std::string($2), ExprPtr($6), 
                      std::make_unique<ExprStmt>(ExprPtr($8)), *$4);
      setExprLocation(letExpr, @1);
      $$ = letExpr;
      delete $4;
      free($2);
  }
  | LET IDENT ASSIGN expr IN expr {
      auto letExpr = new LetExpr(std::string($2), ExprPtr($4), 
                      std::make_unique<ExprStmt>(ExprPtr($6)));
      setExprLocation(letExpr, @1);
      $$ = letExpr;
      free($2);
  }
  | if_expr
  
  | WHILE expr LBRACE stmt_list RBRACE { 
      auto block = ExprPtr(new ExprBlock(std::move(*$4)));
      auto whileExpr = new WhileExpr(ExprPtr($2), std::move(block));
      setExprLocation(whileExpr, @1);
      $$ = whileExpr; 
      delete $4;
  }
  | FOR LPAREN IDENT IN expr RPAREN expr {
      auto forExpr = new ForExpr(std::string($3), ExprPtr($5), ExprPtr($7));
      setExprLocation(forExpr, @1);
      $$ = forExpr;
      free($3);
  }
  | FOR LPAREN IDENT IN expr RPAREN LBRACE stmt_list RBRACE {
      auto block = ExprPtr(new ExprBlock(std::move(*$8)));
      auto forExpr = new ForExpr(std::string($3), ExprPtr($5), std::move(block));
      setExprLocation(forExpr, @1);
      $$ = forExpr;
      free($3);
      delete $8;
  }
  | LBRACE stmt_list RBRACE { 
      auto block = new ExprBlock(std::move(*$2));
      setExprLocation(block, @1);
      $$ = block; 
      delete $2;
  }
  | LPAREN expr RPAREN { 
      setExprLocation($2, @2);
      $$ = $2; 
  }
    | expr DOT IDENT {
        auto getAttr = new GetAttrExpr(ExprPtr($1), $3);
        setExprLocation(getAttr, @2);
        $$ = getAttr;
        free($3);
    }
  | expr DOT IDENT ASSIGN_DESTRUCT expr {
        auto setAttr = new SetAttrExpr(ExprPtr($1), $3, ExprPtr($5));
        setExprLocation(setAttr, @2);
        $$ = setAttr;
        free($3);
    }
  | expr DOT IDENT LPAREN argument_list RPAREN {
        auto methodCall = new MethodCallExpr(ExprPtr($1), $3, std::move(*$5));
        setExprLocation(methodCall, @2);
        $$ = methodCall;
        delete $5;
        free($3);
    }
  | SELF {
        auto selfExpr = new SelfExpr();
        setExprLocation(selfExpr, @1);
        $$ = selfExpr;
    }  
  | SELF ASSIGN_DESTRUCT expr {
        auto assignExpr = new AssignExpr("self", ExprPtr($3));
        setExprLocation(assignExpr, @1);
        $$ = assignExpr;
    }
  | BASE LPAREN RPAREN {
        auto baseCall = new BaseCallExpr({});
        setExprLocation(baseCall, @1);
        $$ = baseCall;
    }  
  | IDENT ASSIGN_DESTRUCT expr {
        auto assignExpr = new AssignExpr(std::string($1), ExprPtr($3));
        setExprLocation(assignExpr, @1);
        $$ = assignExpr;
        free($1);
    }
  
;



if_expr:
    IF LPAREN expr RPAREN expr ELSE expr {
        auto ifExpr = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr($7));
        setExprLocation(ifExpr, @1);
        $$ = ifExpr;
    }
    | IF LPAREN expr RPAREN expr ELIF LPAREN expr RPAREN expr ELSE expr {
        // Transformar: if (cond1) expr1 elif (cond2) expr2 else expr3
        // A: if (cond1) expr1 else if (cond2) expr2 else expr3
        auto nestedIf = new IfExpr(ExprPtr($8), ExprPtr($10), ExprPtr($12));
        auto ifExpr = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr(nestedIf));
        setExprLocation(ifExpr, @1);
        $$ = ifExpr;
    }
    | IF LPAREN expr RPAREN expr ELIF LPAREN expr RPAREN expr {
        // Transformar: if (cond1) expr1 elif (cond2) expr2
        // A: if (cond1) expr1 else if (cond2) expr2
        auto nestedIf = new IfExpr(ExprPtr($8), ExprPtr($10), nullptr);
        auto ifExpr = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr(nestedIf));
        setExprLocation(ifExpr, @1);
        $$ = ifExpr;
    }
    | IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*$6)));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*$10)));
      auto ifExpr = new IfExpr(ExprPtr($3), std::move(ifBlock), std::move(elseBlock));
      setExprLocation(ifExpr, @1);
      $$ = ifExpr; 
      delete $6;
      delete $10;
  }
  | IF LPAREN expr RPAREN LBRACE stmt_list RBRACE { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*$6)));
      auto ifExpr = new IfExpr(ExprPtr($3), std::move(ifBlock), nullptr);
      setExprLocation(ifExpr, @1);
      $$ = ifExpr; 
      delete $6;
  }
  | IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELIF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE {
      // Transformar: if (cond1) { body1 } elif (cond2) { body2 } else { body3 }
      // A: if (cond1) { body1 } else if (cond2) { body2 } else { body3 }
      auto elifBlock = ExprPtr(new ExprBlock(std::move(*$13)));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*$17)));
      auto nestedIf = new IfExpr(ExprPtr($10), std::move(elifBlock), std::move(elseBlock));
      
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*$6)));
      auto ifExpr = new IfExpr(ExprPtr($3), std::move(ifBlock), ExprPtr(nestedIf));
      setExprLocation(ifExpr, @1);
      $$ = ifExpr;
      delete $6;
      delete $13;
      delete $17;
  }
  | IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELIF LPAREN expr RPAREN LBRACE stmt_list RBRACE {
      // Transformar: if (cond1) { body1 } elif (cond2) { body2 }
      // A: if (cond1) { body1 } else if (cond2) { body2 }
      auto elifBlock = ExprPtr(new ExprBlock(std::move(*$13)));
      auto nestedIf = new IfExpr(ExprPtr($10), std::move(elifBlock), nullptr);
      
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*$6)));
      auto ifExpr = new IfExpr(ExprPtr($3), std::move(ifBlock), ExprPtr(nestedIf));
      setExprLocation(ifExpr, @1);
      $$ = ifExpr;
      delete $6;
      delete $13;
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
    TYPE IDENT LPAREN param_list RPAREN INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            std::move(params),
            std::move(param_types),
            std::move($12->first),
            std::move($12->second),
            std::string($7),
            std::move(*$9)
        ));
        setStmtLocation(typeDecl, @1);
        (yyval.stmt) = typeDecl;
        delete $4; delete $9; delete $12;
        free($2); free($7);
    }
  | TYPE IDENT INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            {},
            {},
            std::move($9->first),
            std::move($9->second),
            std::string($4),
            std::move(*$6)
        ));
        setStmtLocation(typeDecl, @1);
        (yyval.stmt) = typeDecl;
        delete $6; delete $9;
        free($2); free($4);
    }
| TYPE IDENT INHERITS IDENT LBRACE member_list RBRACE {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            {}, // sin parámetros propios
            {},
            std::move($6->first),
            std::move($6->second),
            std::string($4), // tipo base
            {} // sin baseArgs explícitos: herencia implícita
        ));
        setStmtLocation(typeDecl, @1);
        (yyval.stmt) = typeDecl;
        delete $6;
        free($2); free($4);
    }
  | TYPE IDENT LPAREN param_list RPAREN LBRACE member_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            std::move(params),
            std::move(param_types),
            std::move($7->first),
            std::move($7->second)
        ));
        setStmtLocation(typeDecl, @1);
        (yyval.stmt) = typeDecl;
        delete $4; delete $7;
        free($2);
    }
  | TYPE IDENT LPAREN param_list RPAREN INHERITS IDENT LBRACE member_list RBRACE {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$4) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            std::move(params),
            std::move(param_types),
            std::move($9->first),
            std::move($9->second),
            std::string($7),
            {} // sin baseArgs
        ));
        setStmtLocation(typeDecl, @1);
        (yyval.stmt) = typeDecl;
        delete $4; delete $9;
        free($2); free($7);
    }
  | TYPE IDENT LBRACE member_list RBRACE {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string($2),
            {},
            {},
            std::move($4->first),
            std::move($4->second)
        ));
        setStmtLocation(typeDecl, @1);
        (yyval.stmt) = typeDecl;
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
        auto attrDecl = static_cast<AttributeDecl*>(new AttributeDecl($1, ExprPtr($3)));
        setStmtLocation(attrDecl, @1);
        (yyval.attribute_decl) = attrDecl;
        free($1);
    }
    | IDENT COLON type ASSIGN expr {
        auto attrDecl = static_cast<AttributeDecl*>(new AttributeDecl($1, ExprPtr($5), *$3));
        setStmtLocation(attrDecl, @1);
        (yyval.attribute_decl) = attrDecl;
        free($1);
        delete $3;
    }
;

method:
    IDENT LPAREN ident_list RPAREN ARROW expr {
        std::vector<std::string> args = $3 ? std::move(*$3) : std::vector<std::string>();
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        // Create paramTypes with Unknown types for each parameter
        for (const auto& param : args) {
            param_types.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
        }
        delete $3;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($6))), std::move(param_types)));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        free($1);
    }
    | IDENT LPAREN param_list RPAREN ARROW expr {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$3) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($6))), std::move(param_types)));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        delete $3;
        free($1);
    }
    | IDENT LPAREN param_list RPAREN COLON type ARROW expr {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$3) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($8))), std::move(param_types), *$6));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        delete $3;
        delete $6;
        free($1);
    }
    | IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> args = $3 ? std::move(*$3) : std::vector<std::string>();
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        // Create paramTypes with Unknown types for each parameter
        for (const auto& param : args) {
            param_types.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
        }
        delete $3;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$6);
        delete $6;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        free($1);
    }
    | IDENT LPAREN param_list RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$3) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$6);
        delete $6;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        delete $3;
        free($1);
    }
    | IDENT LPAREN param_list RPAREN COLON type LBRACE stmt_list RBRACE {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *$3) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$8);
        delete $8;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(std::move(block)), std::move(param_types), *$6));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        delete $3;
        delete $6;
        free($1);
    }
    | IDENT LPAREN RPAREN ARROW expr {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($5))), std::move(param_types)));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        free($1);
    }
    | IDENT LPAREN RPAREN COLON type ARROW expr {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(new ExprStmt(ExprPtr($7))), std::move(param_types), *$5));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        delete $5;
        free($1);
    }
    | IDENT LPAREN RPAREN LBRACE stmt_list RBRACE {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$5);
        delete $5;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        free($1);
    }
    | IDENT LPAREN RPAREN COLON type LBRACE stmt_list RBRACE {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$7);
        delete $7;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl($1, std::move(args), StmtPtr(std::move(block)), std::move(param_types), *$5));
        setStmtLocation(methodDecl, @1);
        (yyval.method_decl) = methodDecl;
        delete $5;
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