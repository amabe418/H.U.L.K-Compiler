#include "ll1_parser_generated.hpp"
// --- Métodos de parseo ---
LL1ParserGenerated::LL1ParserGenerated(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}
Expr* LL1ParserGenerated::parse_AddExpr() {
    std::cout << "Aplicando: AddExpr -> ...\n";
    Expr* left = parse_Term();
    while (true) {
        TokenType t = lookahead().type;
        if (t == PLUS) { const Token& opToken = lookahead(); match(PLUS); Expr* right = parse_Term(); auto binExpr = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == MINUS) { const Token& opToken = lookahead(); match(MINUS); Expr* right = parse_Term(); auto binExpr = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else break;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_AddExpr_prime() {
    std::cout << "Aplicando: AddExpr' -> ...\n";
    // Implementar lógica de parseo para AddExpr'
    return nullptr;
}
Expr* LL1ParserGenerated::parse_AndExpr() {
    std::cout << "Aplicando: AndExpr -> ...\n";
    Expr* left = parse_CmpExpr();
    while (lookahead().type == AND) {
        const Token& opToken = lookahead();
        match(AND);
        Expr* right = parse_CmpExpr();
        auto binExpr = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr(left), ExprPtr(right));
        binExpr->line_number = opToken.line; binExpr->column_number = opToken.column;
        left = binExpr;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_AndExpr_prime() {
    std::cout << "Aplicando: AndExpr' -> ...\n";
    // Implementar lógica de parseo para AndExpr'
    return nullptr;
}
std::vector<std::string> LL1ParserGenerated::parse_ArgIdList() {
    std::vector<std::string> args;
    if (lookahead().type == IDENT) {
        args.push_back(lookahead().lexeme); match(IDENT);
        parse_ArgIdListTail(args);
    }
    return args;
}
void LL1ParserGenerated::parse_ArgIdListTail(std::vector<std::string>& args) {
    if (lookahead().type == COMMA) {
        match(COMMA);
        args.push_back(lookahead().lexeme); match(IDENT);
        parse_ArgIdListTail(args);
    }
}
std::vector<ExprPtr> LL1ParserGenerated::parse_ArgList() {
    std::vector<ExprPtr> args;
    if (lookahead().type != RPAREN) {
        args.push_back(ExprPtr(parse_Expr()));
        while (lookahead().type == COMMA) {
            match(COMMA);
            args.push_back(ExprPtr(parse_Expr()));
        }
    }
    return args;
}
void LL1ParserGenerated::parse_ArgListTail(std::vector<std::string>& args) {
    if (lookahead().type == COMMA) {
        match(COMMA);
        args.push_back(lookahead().lexeme); match(IDENT);
        parse_ArgListTail(args);
    }
}
std::unique_ptr<AttributeDecl> LL1ParserGenerated::parse_AttributeDef() {
    const Token& attrToken = lookahead();
    std::string name = attrToken.lexeme; match(IDENT);
    match(ASSIGN);
    Expr* initializer = parse_Expr();
    match(SEMICOLON);
    
    auto attrDecl = std::make_unique<AttributeDecl>(name, ExprPtr(initializer));
    attrDecl->line_number = attrToken.line; attrDecl->column_number = attrToken.column;
    return attrDecl;
}
Expr* LL1ParserGenerated::parse_BlockStmt() {
    match(LBRACE);
    std::vector<StmtPtr> stmts;
    while (lookahead().type != RBRACE && lookahead().type != TOKEN_EOF) {
        Stmt* stmt = parse_Stmt();
        if (lookahead().type == SEMICOLON) match(SEMICOLON);
        stmts.push_back(StmtPtr(stmt));
    }
    match(RBRACE);
    return new ExprBlock(std::move(stmts));
}
Expr* LL1ParserGenerated::parse_CmpExpr() {
    std::cout << "Aplicando: CmpExpr -> ...\n";
    Expr* left = parse_ConcatExpr();
    while (true) {
        TokenType t = lookahead().type;
        if (t == LESS_THAN) { const Token& opToken = lookahead(); match(LESS_THAN); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == GREATER_THAN) { const Token& opToken = lookahead(); match(GREATER_THAN); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == LE) { const Token& opToken = lookahead(); match(LE); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == GE) { const Token& opToken = lookahead(); match(GE); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == EQ) { const Token& opToken = lookahead(); match(EQ); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == NEQ) { const Token& opToken = lookahead(); match(NEQ); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else break;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_CmpExpr_prime() {
    std::cout << "Aplicando: CmpExpr' -> ...\n";
    // Implementar lógica de parseo para CmpExpr'
    return nullptr;
}
Expr* LL1ParserGenerated::parse_ConcatExpr() {
    std::cout << "Aplicando: ConcatExpr -> ...\n";
    Expr* left = parse_AddExpr();
    while (true) {
        TokenType t = lookahead().type;
        if (t == CONCAT) { const Token& opToken = lookahead(); match(CONCAT); Expr* right = parse_AddExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == CONCAT_WS) { const Token& opToken = lookahead(); match(CONCAT_WS); Expr* right = parse_AddExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else break;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_ConcatExpr_prime() {
    std::cout << "Aplicando: ConcatExpr' -> ...\n";
    // Implementar lógica de parseo para ConcatExpr'
    return nullptr;
}
std::pair<ExprPtr, ExprPtr> LL1ParserGenerated::parse_ElifBranch() {
    match(ELIF); match(LPAREN);
    Expr* condition = parse_Expr();
    match(RPAREN);
    Expr* thenBranch = parse_IfBody();
    return {ExprPtr(condition), ExprPtr(thenBranch)};
}
std::vector<std::pair<ExprPtr, ExprPtr>> LL1ParserGenerated::parse_ElifList() {
    std::vector<std::pair<ExprPtr, ExprPtr>> elifs;
    while (lookahead().type == ELIF) {
        auto elif = parse_ElifBranch();
        elifs.emplace_back(std::move(elif.first), std::move(elif.second));
    }
    return elifs;
}
Expr* LL1ParserGenerated::parse_Expr() {
    TokenType t = lookahead().type;
    if (t == IF) {
        return parse_IfExpr();
    } else if (t == LET) {
        return parse_LetExpr();
    } else {
        return parse_OrExpr();
    }
}
Expr* LL1ParserGenerated::parse_Factor() {
    std::cout << "Aplicando: Factor -> ...\n";
    Expr* left = parse_Power();
    while (lookahead().type == POW) {
        const Token& opToken = lookahead();
        match(POW);
        Expr* right = parse_Power();
        auto binExpr = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr(left), ExprPtr(right));
        binExpr->line_number = opToken.line; binExpr->column_number = opToken.column;
        left = binExpr;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_Factor_prime() {
    std::cout << "Aplicando: Factor' -> ...\n";
    // Implementar lógica de parseo para Factor'
    return nullptr;
}
Expr* LL1ParserGenerated::parse_ForBody() {
    if (lookahead().type == LBRACE) {
        return parse_BlockStmt();
    } else {
        return parse_Expr();
    }
}
Expr* LL1ParserGenerated::parse_ForStmt() {
    match(FOR); match(LPAREN);
    std::string var = lookahead().lexeme; match(IDENT);
    match(IN);
    Expr* iterable = parse_Expr();
    match(RPAREN);
    Expr* body = parse_ForBody();
    return new ForExpr(var, ExprPtr(iterable), ExprPtr(body));
}
Expr* LL1ParserGenerated::parse_FunctionBody() {
    if (lookahead().type == ARROW) {
        match(ARROW);
        Expr* body = parse_Expr();
        // No consumir SEMICOLON aquí, será manejado por parse_StmtListTail
        return body;
    } else {
        return parse_BlockStmt();
    }
}
Stmt* LL1ParserGenerated::parse_FunctionDef() {
    const Token& funcToken = lookahead();
    match(FUNCTION);
    std::string name = lookahead().lexeme; match(IDENT);
    match(LPAREN);
    std::vector<std::string> args = parse_ArgIdList();
    match(RPAREN);
    Expr* bodyExpr = parse_FunctionBody();
    StmtPtr body = std::make_unique<ExprStmt>(ExprPtr(bodyExpr));
    
    // Crear lista de tipos de parámetros con la misma cantidad que args
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    for (size_t i = 0; i < args.size(); ++i) {
        paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
    }
    
    // Crear tipo de retorno por defecto (Unknown)
    auto returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    
    auto funcDecl = new FunctionDecl(name, std::move(args), std::move(body), std::move(paramTypes), std::move(returnType));
    funcDecl->line_number = funcToken.line; funcDecl->column_number = funcToken.column;
    return funcDecl;
}
Expr* LL1ParserGenerated::parse_IfBody() {
    if (lookahead().type == LBRACE) {
        return parse_BlockStmt();
    } else {
        return parse_Expr();
    }
}
Expr* LL1ParserGenerated::parse_IfExpr() {
    const Token& ifToken = lookahead();
    match(IF); match(LPAREN);
    Expr* condition = parse_Expr();
    match(RPAREN);
    Expr* thenBranch = parse_IfBody();
    std::vector<std::pair<ExprPtr, ExprPtr>> elifs = parse_ElifList();
    match(ELSE);
    Expr* elseBranch = parse_IfBody();
    
    // Construir if-elif-else anidados\n";
    Expr* result = elseBranch;
    for (auto it = elifs.rbegin(); it != elifs.rend(); ++it) {
        auto ifExpr = new IfExpr(std::move(it->first), std::move(it->second), ExprPtr(result));
        ifExpr->line_number = ifToken.line; ifExpr->column_number = ifToken.column;
        result = ifExpr;
    }
    auto ifExpr = new IfExpr(ExprPtr(condition), ExprPtr(thenBranch), ExprPtr(result));
    ifExpr->line_number = ifToken.line; ifExpr->column_number = ifToken.column;
    return ifExpr;
}
Expr* LL1ParserGenerated::parse_LetBody() {
    if (lookahead().type == LBRACE) {
        return parse_BlockStmt();
    } else {
        return parse_Expr();
    }
}
Expr* LL1ParserGenerated::parse_LetExpr() {
    const Token& letToken = lookahead();
    match(LET);
    std::vector<std::pair<std::string, ExprPtr>> bindings = parse_VarBindingList();
    match(IN);
    Expr* body = parse_LetBody();
    
    // Crear let expressions anidados para múltiples variables\n";
    Expr* result = body;
    for (auto it = bindings.rbegin(); it != bindings.rend(); ++it) {
        auto letExpr = new LetExpr(it->first, std::move(it->second), StmtPtr(new ExprStmt(ExprPtr(result))));
        letExpr->line_number = letToken.line; letExpr->column_number = letToken.column;
        result = letExpr;
    }
    return result;
}
std::unique_ptr<MethodDecl> LL1ParserGenerated::parse_MethodDef() {
    const Token& methodToken = lookahead();
    std::string name = methodToken.lexeme; match(IDENT);
    match(LPAREN);
    std::vector<std::string> params = parse_ArgIdList();
    match(RPAREN);
    Expr* bodyExpr = parse_FunctionBody();
    StmtPtr body = std::make_unique<ExprStmt>(ExprPtr(bodyExpr));
    
    // Crear lista de tipos de parámetros con la misma cantidad que params
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    for (size_t i = 0; i < params.size(); ++i) {
        paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
    }
    
    // Crear tipo de retorno por defecto (Unknown)
    auto returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    
    auto methodDecl = std::make_unique<MethodDecl>(name, std::move(params), std::move(body), std::move(paramTypes), std::move(returnType));
    methodDecl->line_number = methodToken.line; methodDecl->column_number = methodToken.column;
    return methodDecl;
}
Expr* LL1ParserGenerated::parse_OrExpr() {
    std::cout << "Aplicando: OrExpr -> ...\n";
    Expr* left = parse_AndExpr();
    while (lookahead().type == OR) {
        const Token& opToken = lookahead();
        match(OR);
        Expr* right = parse_AndExpr();
        auto binExpr = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr(left), ExprPtr(right));
        binExpr->line_number = opToken.line; binExpr->column_number = opToken.column;
        left = binExpr;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_OrExpr_prime() {
    std::cout << "Aplicando: OrExpr' -> ...\n";
    // Implementar lógica de parseo para OrExpr'
    return nullptr;
}
Expr* LL1ParserGenerated::parse_Power() {
    std::cout << "Aplicando: Power -> ...\n";
    return parse_Unary();
}
Expr* LL1ParserGenerated::parse_Primary() {
    std::cout << "Aplicando: Primary -> ...\n";
    const Token& tok = lookahead();
    if (tok.type == NUMBER) { match(NUMBER); auto expr = new NumberExpr(std::stod(tok.lexeme)); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == STRING) { match(STRING); std::string val = tok.lexeme; if (!val.empty() && val.front() == '"' && val.back() == '"') val = val.substr(1, val.size()-2); auto expr = new StringExpr(val); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == TRUE) { match(TRUE); auto expr = new BooleanExpr(true); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == FALSE) { match(FALSE); auto expr = new BooleanExpr(false); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == IDENT) { match(IDENT); auto expr = parse_PrimaryTail(tok.lexeme); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == LPAREN) { match(LPAREN); Expr* e = parse_Expr(); match(RPAREN); e->line_number = tok.line; e->column_number = tok.column; return e; }
    if (tok.type == NEW) { match(NEW); std::string typeName = lookahead().lexeme; match(IDENT); match(LPAREN); std::vector<ExprPtr> args = parse_ArgList(); match(RPAREN); auto expr = new NewExpr(typeName, std::move(args)); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == SELF) { match(SELF); auto expr = parse_PrimaryTail("self"); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    if (tok.type == BASE) { match(BASE); match(LPAREN); std::vector<ExprPtr> args = parse_ArgList(); match(RPAREN); auto expr = new BaseCallExpr(std::move(args)); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }
    std::cerr << "Error de parseo: Token inesperado '" << tok.lexeme << "' (tipo: " << static_cast<int>(tok.type) << ") en línea " << tok.line << ", columna " << tok.column << std::endl;
    throw std::runtime_error("Unexpected token in Primary");
}
Expr* LL1ParserGenerated::parse_PrimaryTail(const std::string& varName) {
    const Token& tok = lookahead();
    Expr* expr = new VariableExpr(varName);
    expr->line_number = tok.line; expr->column_number = tok.column;
    while (true) {
        if (lookahead().type == LPAREN) {
            match(LPAREN);
            std::vector<ExprPtr> args = parse_ArgList();
            match(RPAREN);
            // Si expr es VariableExpr, es una llamada a función normal
            if (auto var = dynamic_cast<VariableExpr*>(expr)) {
                auto callExpr = new CallExpr(var->name, std::move(args));
                callExpr->line_number = tok.line; callExpr->column_number = tok.column;
                delete expr;
                expr = callExpr;
            } else {
                // Si no es VariableExpr, es una llamada a método
                std::string methodName = "";
                if (auto getAttr = dynamic_cast<GetAttrExpr*>(expr)) {
                    methodName = getAttr->attrName;
                    auto methodCall = new MethodCallExpr(std::move(getAttr->object), methodName, std::move(args));
                    methodCall->line_number = tok.line; methodCall->column_number = tok.column;
                    delete getAttr;
                    expr = methodCall;
                } else {
                    // Fallback: tratar como llamada a función
                    auto callExpr = new CallExpr("", std::move(args));
                    callExpr->line_number = tok.line; callExpr->column_number = tok.column;
                    delete expr;
                    expr = callExpr;
                }
            }
        } else if (lookahead().type == DOT) {
            match(DOT);
            std::string memberName = lookahead().lexeme; match(IDENT);
            auto getAttr = new GetAttrExpr(ExprPtr(expr), memberName);
            getAttr->line_number = tok.line; getAttr->column_number = tok.column;
            expr = getAttr;
        } else if (lookahead().type == ASSIGN_DESTRUCT) {
            match(ASSIGN_DESTRUCT);
            Expr* value = parse_Expr();
            // Si expr es GetAttrExpr, asignar a atributo; si es VariableExpr, asignar a variable
            if (auto getAttr = dynamic_cast<GetAttrExpr*>(expr)) {
                auto setAttr = new SetAttrExpr(std::move(getAttr->object), getAttr->attrName, ExprPtr(value));
                setAttr->line_number = tok.line; setAttr->column_number = tok.column;
                delete getAttr;
                return setAttr;
            } else if (auto var = dynamic_cast<VariableExpr*>(expr)) {
                auto assignExpr = new AssignExpr(var->name, ExprPtr(value));
                assignExpr->line_number = tok.line; assignExpr->column_number = tok.column;
                delete var;
                return assignExpr;
            } else {
                std::cerr << "Error: Asignación destructiva inválida\n";
                throw std::runtime_error("Invalid destructive assignment");
            }
        } else if (lookahead().type == ASSIGN) {
            const Token& tok = lookahead();
            std::cerr << "Error de sintaxis: Asignación normal (=) no está permitida. Use asignación destructiva (:=) en línea " << tok.line << " columna " << tok.column << std::endl;
            throw std::runtime_error("Invalid assignment operator");
        } else {
            // No hay más tails, terminar y devolver la expresión actual
            break;
        }
    }
    return expr;
}
Program* LL1ParserGenerated::parse_Program() {
    std::cout << "Aplicando: Program -> StmtList\n";
    auto prog = new Program();
    prog->stmts = parse_StmtList();
    return prog;
}
Stmt* LL1ParserGenerated::parse_Stmt() {
    TokenType t = lookahead().type;
    if (t == WHILE) { return new ExprStmt(ExprPtr(parse_WhileStmt())); }
    else if (t == FOR) { return new ExprStmt(ExprPtr(parse_ForStmt())); }
    else if (t == LBRACE) { return new ExprStmt(ExprPtr(parse_BlockStmt())); }
    else if (t == FUNCTION) { return parse_FunctionDef(); }
    else if (t == TYPE) { return parse_TypeDef(); }
    else { return new ExprStmt(ExprPtr(parse_Expr())); }
}
std::vector<StmtPtr> LL1ParserGenerated::parse_StmtList() {
    std::vector<StmtPtr> stmts;
    // FIRST(Stmt): BASE FALSE FOR FUNCTION IDENT IF LBRACE LET LPAREN MINUS NEW NUMBER SELF STRING TRUE TYPE WHILE
    TokenType t = lookahead().type;
    if (t == BASE || t == FALSE || t == FOR || t == FUNCTION || t == IDENT || t == IF || t == LBRACE || t == LET || t == LPAREN || t == MINUS || t == NEW || t == NUMBER || t == SELF || t == STRING || t == TRUE || t == TYPE || t == WHILE) {
        Stmt* stmt = parse_Stmt();
        std::vector<StmtPtr> tail = parse_StmtListTail();
        stmts.push_back(StmtPtr(stmt));
        stmts.insert(stmts.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));
    }
    return stmts;
}
std::vector<StmtPtr> LL1ParserGenerated::parse_StmtListTail() {
    std::vector<StmtPtr> stmts;
    if (lookahead().type == SEMICOLON) {
        match(SEMICOLON);
        std::vector<StmtPtr> tail = parse_StmtList();
        stmts.insert(stmts.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));
    }
    return stmts;
}
Expr* LL1ParserGenerated::parse_Term() {
    std::cout << "Aplicando: Term -> ...\n";
    Expr* left = parse_Factor();
    while (true) {
        TokenType t = lookahead().type;
        if (t == MULT) { const Token& opToken = lookahead(); match(MULT); Expr* right = parse_Factor(); auto binExpr = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == DIV) { const Token& opToken = lookahead(); match(DIV); Expr* right = parse_Factor(); auto binExpr = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else if (t == MOD) { const Token& opToken = lookahead(); match(MOD); Expr* right = parse_Factor(); auto binExpr = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }
        else break;
    }
    return left;
}
Expr* LL1ParserGenerated::parse_Term_prime() {
    std::cout << "Aplicando: Term' -> ...\n";
    // Implementar lógica de parseo para Term'
    return nullptr;
}
std::vector<ExprPtr> LL1ParserGenerated::parse_TypeBaseArgs() {
    if (lookahead().type == LPAREN) {
        match(LPAREN);
        std::vector<ExprPtr> args = parse_ArgList();
        match(RPAREN);
        return args;
    }
    return {};
}
std::pair<std::vector<std::unique_ptr<AttributeDecl>>, std::vector<std::unique_ptr<MethodDecl>>> LL1ParserGenerated::parse_TypeBody() {
    std::vector<std::unique_ptr<AttributeDecl>> attributes;
    std::vector<std::unique_ptr<MethodDecl>> methods;
    
    while (lookahead().type != RBRACE && lookahead().type != TOKEN_EOF) {
        auto member = parse_TypeMember();
        if (member.first) {
            attributes.push_back(std::move(member.first));
        }
        if (member.second) {
            methods.push_back(std::move(member.second));
        }
    }
    
    return {std::move(attributes), std::move(methods)};
}
Stmt* LL1ParserGenerated::parse_TypeDef() {
    const Token& typeToken = lookahead();
    match(TYPE);
    std::string name = lookahead().lexeme; match(IDENT);
    std::vector<std::string> params = parse_TypeParams();
    auto inheritance = parse_TypeInheritance();
    match(LBRACE);
    auto body = parse_TypeBody();
    match(RBRACE);
    
    // Crear listas de tipos de parámetros con la misma cantidad que params
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    for (size_t i = 0; i < params.size(); ++i) {
        paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
    }
    
    auto typeDecl = new TypeDecl(name, std::move(params), std::move(paramTypes), std::move(body.first), std::move(body.second), inheritance.first, std::move(inheritance.second));
    typeDecl->line_number = typeToken.line; typeDecl->column_number = typeToken.column;
    return typeDecl;
}
std::pair<std::string, std::vector<ExprPtr>> LL1ParserGenerated::parse_TypeInheritance() {
    if (lookahead().type == INHERITS) {
        match(INHERITS);
        std::string baseType = lookahead().lexeme; match(IDENT);
        std::vector<ExprPtr> baseArgs = parse_TypeBaseArgs();
        return std::make_pair(std::move(baseType), std::move(baseArgs));
    }
    return std::make_pair("Object", std::vector<ExprPtr>{});
}
std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> LL1ParserGenerated::parse_TypeMember() {
    std::string memberName = lookahead().lexeme; match(IDENT);
    return parse_TypeMemberTail(memberName);
}
std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> LL1ParserGenerated::parse_TypeMemberTail(const std::string& memberName) {
    if (lookahead().type == ASSIGN) {
        // Es un atributo
        match(ASSIGN);
        Expr* initializer = parse_Expr();
        match(SEMICOLON);
        auto attrDecl = std::make_unique<AttributeDecl>(memberName, ExprPtr(initializer));
        return {std::move(attrDecl), std::unique_ptr<MethodDecl>(nullptr)};
    } else if (lookahead().type == LPAREN) {
        // Es un método
        match(LPAREN);
        std::vector<std::string> params = parse_ArgIdList();
        match(RPAREN);
        Expr* bodyExpr = parse_FunctionBody();
        match(SEMICOLON); // Consumir el SEMICOLON después del método
        StmtPtr body = std::make_unique<ExprStmt>(ExprPtr(bodyExpr));
        
        // Crear lista de tipos de parámetros con la misma cantidad que params
        std::vector<std::shared_ptr<TypeInfo>> paramTypes;
        for (size_t i = 0; i < params.size(); ++i) {
            paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
        }
        
        // Crear tipo de retorno por defecto (Unknown)
        auto returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
        
        auto methodDecl = std::make_unique<MethodDecl>(memberName, std::move(params), std::move(body), std::move(paramTypes), std::move(returnType));
        return {std::unique_ptr<AttributeDecl>(nullptr), std::move(methodDecl)};
    }
    return {std::unique_ptr<AttributeDecl>(nullptr), std::unique_ptr<MethodDecl>(nullptr)};
}
std::vector<std::string> LL1ParserGenerated::parse_TypeParams() {
    if (lookahead().type == LPAREN) {
        match(LPAREN);
        std::vector<std::string> params = parse_ArgIdList();
        match(RPAREN);
        return params;
    }
    return {};
}
Expr* LL1ParserGenerated::parse_Unary() {
    std::cout << "Aplicando: Unary -> ...\n";
    if (lookahead().type == MINUS) { const Token& opToken = lookahead(); match(MINUS); Expr* operand = parse_Unary(); auto unaryExpr = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr(operand)); unaryExpr->line_number = opToken.line; unaryExpr->column_number = opToken.column; return unaryExpr; }
    return parse_Primary();
}
Expr* LL1ParserGenerated::parse_VarBinding() {
    std::cout << "Aplicando: VarBinding -> ...\n";
    // Implementar lógica de parseo para VarBinding
    return nullptr;
}
std::vector<std::pair<std::string, ExprPtr>> LL1ParserGenerated::parse_VarBindingList() {
    std::vector<std::pair<std::string, ExprPtr>> bindings;
    std::string varName = lookahead().lexeme; match(IDENT);
    match(ASSIGN);
    Expr* value = parse_Expr();
    bindings.emplace_back(varName, ExprPtr(value));
    parse_VarBindingListTail(bindings);
    return bindings;
}
void LL1ParserGenerated::parse_VarBindingListTail(std::vector<std::pair<std::string, ExprPtr>>& bindings) {
    if (lookahead().type == COMMA) {
        match(COMMA);
        std::string varName = lookahead().lexeme; match(IDENT);
        match(ASSIGN);
        Expr* value = parse_Expr();
        bindings.emplace_back(varName, ExprPtr(value));
        parse_VarBindingListTail(bindings);
    }
}
Expr* LL1ParserGenerated::parse_WhileBody() {
    if (lookahead().type == LBRACE) {
        return parse_BlockStmt();
    } else {
        return parse_Expr();
    }
}
Expr* LL1ParserGenerated::parse_WhileStmt() {
    match(WHILE);
    Expr* condition = parse_Expr();
    Expr* body = parse_WhileBody();
    return new WhileExpr(ExprPtr(condition), ExprPtr(body));
}
