#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <regex>

struct Production
{
    std::string lhs;
    std::vector<std::string> rhs;
};

// Lee la gramática desde un archivo y devuelve la lista de producciones
std::vector<Production> read_grammar(const std::string &filename)
{
    std::vector<Production> productions;
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "No se pudo abrir el archivo de gramática: " << filename << std::endl;
        return productions;
    }
    std::string line;
    std::string last_lhs;
    while (std::getline(file, line))
    {
        // Ignorar líneas vacías o comentarios
        if (line.empty() || line[0] == '#')
            continue;
        size_t arrow = line.find("->");
        if (arrow != std::string::npos)
        {
            // Nueva producción principal
            std::string lhs = line.substr(0, arrow);
            lhs.erase(lhs.find_last_not_of(" \t") + 1);
            lhs.erase(0, lhs.find_first_not_of(" \t"));
            last_lhs = lhs;
            std::string rhs_all = line.substr(arrow + 2);
            std::istringstream rhs_stream(rhs_all);
            std::string rhs_part;
            while (std::getline(rhs_stream, rhs_part, '|'))
            {
                Production prod;
                prod.lhs = lhs;
                std::istringstream rhs_tokens(rhs_part);
                std::string symbol;
                while (rhs_tokens >> symbol)
                {
                    prod.rhs.push_back(symbol);
                }
                productions.push_back(prod);
            }
        }
        else if (!last_lhs.empty() && line.find('|') != std::string::npos)
        {
            // Alternativa de la última LHS
            size_t bar = line.find('|');
            std::string rhs_part = line.substr(bar + 1);
            Production prod;
            prod.lhs = last_lhs;
            std::istringstream rhs_tokens(rhs_part);
            std::string symbol;
            while (rhs_tokens >> symbol)
            {
                prod.rhs.push_back(symbol);
            }
            productions.push_back(prod);
        }
    }
    return productions;
}

// Agrupa las producciones por LHS
std::map<std::string, std::vector<std::vector<std::string>>> group_productions(const std::vector<Production> &productions)
{
    std::map<std::string, std::vector<std::vector<std::string>>> grouped;
    for (const auto &prod : productions)
    {
        grouped[prod.lhs].push_back(prod.rhs);
    }
    return grouped;
}

// Calcula los conjuntos FIRST para cada no terminal
std::map<std::string, std::set<std::string>> compute_first(const std::vector<Production> &productions)
{
    std::map<std::string, std::set<std::string>> first;
    std::set<std::string> nonterminals;
    std::set<std::string> terminals;
    // Identificar no terminales
    for (const auto &prod : productions)
    {
        nonterminals.insert(prod.lhs);
    }
    // Identificar terminales
    for (const auto &prod : productions)
    {
        for (const auto &sym : prod.rhs)
        {
            if (!nonterminals.count(sym) && sym != "ε")
            {
                terminals.insert(sym);
            }
        }
    }
    // Inicializar FIRST
    for (const auto &t : terminals)
        first[t] = {t};
    for (const auto &nt : nonterminals)
        first[nt] = {};
    // Agrupar producciones por LHS
    auto grouped = group_productions(productions);
    // Algoritmo iterativo
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto &nt : nonterminals)
        {
            for (const auto &rhs : grouped[nt])
            {
                auto &first_lhs = first[nt];
                bool derives_epsilon = true;
                for (const auto &sym : rhs)
                {
                    for (const auto &f : first[sym])
                    {
                        if (f != "ε" && !first_lhs.count(f))
                        {
                            first_lhs.insert(f);
                            changed = true;
                        }
                    }
                    if (!first[sym].count("ε"))
                    {
                        derives_epsilon = false;
                        break;
                    }
                }
                if (derives_epsilon && !first_lhs.count("ε"))
                {
                    first_lhs.insert("ε");
                    changed = true;
                }
            }
        }
    }
    return first;
}

// Calcula los conjuntos FOLLOW para cada no terminal
std::map<std::string, std::set<std::string>> compute_follow(
    const std::vector<Production> &productions,
    const std::map<std::string, std::set<std::string>> &first,
    const std::string &start_symbol)
{
    std::set<std::string> nonterminals;
    for (const auto &prod : productions)
        nonterminals.insert(prod.lhs);
    std::map<std::string, std::set<std::string>> follow;
    for (const auto &nt : nonterminals)
        follow[nt] = {};
    follow[start_symbol].insert("$"); // $ = EOF
    // Agrupar producciones por LHS
    auto grouped = group_productions(productions);
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto &prod : productions)
        {
            const std::string &A = prod.lhs;
            const std::vector<std::string> &beta = prod.rhs;
            for (size_t i = 0; i < beta.size(); ++i)
            {
                const std::string &B = beta[i];
                if (!nonterminals.count(B))
                    continue;
                // Siguiente símbolo después de B
                bool epsilon_in_next = true;
                for (size_t j = i + 1; j < beta.size(); ++j)
                {
                    epsilon_in_next = false;
                    for (const auto &f : first.at(beta[j]))
                    {
                        if (f != "ε" && !follow[B].count(f))
                        {
                            follow[B].insert(f);
                            changed = true;
                        }
                        if (f == "ε")
                            epsilon_in_next = true;
                    }
                    if (!epsilon_in_next)
                        break;
                }
                // Si todo lo que sigue puede derivar ε, agrega FOLLOW(A) a FOLLOW(B)
                if (i + 1 == beta.size() || epsilon_in_next)
                {
                    for (const auto &f : follow[A])
                    {
                        if (!follow[B].count(f))
                        {
                            follow[B].insert(f);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    return follow;
}

// Construye la tabla LL(1) y la imprime
void build_and_print_ll1_table(
    const std::vector<Production> &productions,
    const std::map<std::string, std::set<std::string>> &first,
    const std::map<std::string, std::set<std::string>> &follow)
{
    using Table = std::map<std::string, std::map<std::string, std::vector<std::string>>>;
    Table table;
    bool has_conflict = false;
    std::set<std::string> nonterminals;
    for (const auto &prod : productions)
        nonterminals.insert(prod.lhs);

    // Para rastrear conflictos específicos
    std::vector<std::string> conflicts;

    for (const auto &prod : productions)
    {
        const std::string &A = prod.lhs;
        const std::vector<std::string> &alpha = prod.rhs;
        // Calcular FIRST(alpha)
        std::set<std::string> first_alpha;
        bool derives_epsilon = true;
        for (const auto &sym : alpha)
        {
            for (const auto &f : first.at(sym))
            {
                if (f != "ε")
                    first_alpha.insert(f);
            }
            if (!first.at(sym).count("ε"))
            {
                derives_epsilon = false;
                break;
            }
        }
        // 1. Para cada terminal t en FIRST(alpha) (excepto ε)
        for (const auto &t : first_alpha)
        {
            if (!table[A][t].empty())
            {
                has_conflict = true;
                std::string conflict_msg = "Conflicto en M[" + A + ", " + t + "]: ";
                conflict_msg += "Ya existe: ";
                for (const auto &sym : table[A][t])
                    conflict_msg += sym + " ";
                conflict_msg += "vs Nueva: ";
                for (const auto &sym : alpha)
                    conflict_msg += sym + " ";
                conflicts.push_back(conflict_msg);
            }
            table[A][t] = alpha;
        }
        // 2. Si FIRST(alpha) contiene ε, para cada t en FOLLOW(A)
        if (derives_epsilon)
        {
            for (const auto &t : follow.at(A))
            {
                if (!table[A][t].empty())
                {
                    has_conflict = true;
                    std::string conflict_msg = "Conflicto en M[" + A + ", " + t + "]: ";
                    conflict_msg += "Ya existe: ";
                    for (const auto &sym : table[A][t])
                        conflict_msg += sym + " ";
                    conflict_msg += "vs Nueva: ";
                    for (const auto &sym : alpha)
                        conflict_msg += sym + " ";
                    conflicts.push_back(conflict_msg);
                }
                table[A][t] = alpha;
            }
        }
    }
    // Imprimir la tabla LL(1)
    std::cout << "\nTabla LL(1):\n";
    for (const auto &[nt, row] : table)
    {
        for (const auto &[tok, prod] : row)
        {
            std::cout << "M[" << nt << ", " << tok << "] = ";
            for (const auto &sym : prod)
                std::cout << sym << ' ';
            std::cout << std::endl;
        }
    }
    if (has_conflict)
    {
        std::cout << "\n¡Conflicto detectado en la tabla LL(1)! La gramática NO es LL(1)." << std::endl;
        std::cout << "\nDetalles de los conflictos:" << std::endl;
        for (const auto &conflict : conflicts)
        {
            std::cout << "  " << conflict << std::endl;
        }
    }
    else
    {
        std::cout << "\nTabla LL(1) construida sin conflictos. La gramática es LL(1)." << std::endl;
    }
}

// Genera el archivo C++ del parser LL(1)
void generate_parser_cpp(const std::set<std::string> &nonterminals, const std::vector<Production> &productions)
{
    // --- Generar archivo de encabezado ---
    std::ofstream header("ll1_parser_generated.hpp");
    if (!header)
    {
        std::cerr << "No se pudo crear el archivo ll1_parser_generated.hpp" << std::endl;
        return;
    }

    header << "#pragma once\n";
    header << "#include <vector>\n";
    header << "#include <iostream>\n";
    header << "#include \"theoretical/token.hpp\"\n";
    header << "#include \"AST/ast.hpp\"\n";
    header << "using AttributeDeclPtr = std::unique_ptr<AttributeDecl>;\n";
    header << "using MethodDeclPtr = std::unique_ptr<MethodDecl>;\n";
    header << "class LL1ParserGenerated {\n";
    header << "public:\n";
    header << "    LL1ParserGenerated(const std::vector<Token>& tokens);\n";

    // Generar declaraciones de métodos
    for (const auto &nt : nonterminals)
    {
        std::string method_name = nt;
        std::replace(method_name.begin(), method_name.end(), '\'', '_');
        method_name = std::string(std::regex_replace(method_name, std::regex("_+"), "_prime"));

        if (nt == "Program")
        {
            header << "    Program* parse_Program();\n";
        }
        else if (nt == "StmtList")
        {
            header << "    std::vector<StmtPtr> parse_StmtList();\n";
        }
        else if (nt == "StmtListTail")
        {
            header << "    std::vector<StmtPtr> parse_StmtListTail();\n";
        }
        else if (nt == "Stmt")
        {
            header << "    Stmt* parse_Stmt();\n";
        }
        else if (nt == "WhileStmt")
        {
            header << "    Expr* parse_WhileStmt();\n";
        }
        else if (nt == "WhileBody")
        {
            header << "    Expr* parse_WhileBody();\n";
        }
        else if (nt == "ForStmt")
        {
            header << "    Expr* parse_ForStmt();\n";
        }
        else if (nt == "ForBody")
        {
            header << "    Expr* parse_ForBody();\n";
        }
        else if (nt == "BlockStmt")
        {
            header << "    Expr* parse_BlockStmt();\n";
        }
        else if (nt == "Primary")
        {
            header << "    Expr* parse_Primary();\n";
        }
        else if (nt == "PrimaryTail")
        {
            header << "    Expr* parse_PrimaryTail(const std::string& varName);\n";
        }
        else if (nt == "FunctionDef")
        {
            header << "    Stmt* parse_FunctionDef();\n";
        }
        else if (nt == "ArgList")
        {
            header << "    std::vector<ExprPtr> parse_ArgList();\n";
        }
        else if (nt == "ArgListTail")
        {
            header << "    void parse_ArgListTail(std::vector<std::string>& args);\n";
        }
        else if (nt == "FunctionBody")
        {
            header << "    Expr* parse_FunctionBody();\n";
        }
        else if (nt == "IfExpr")
        {
            header << "    Expr* parse_IfExpr();\n";
        }
        else if (nt == "IfBody")
        {
            header << "    Expr* parse_IfBody();\n";
        }
        else if (nt == "ElifList")
        {
            header << "    std::vector<std::pair<ExprPtr, ExprPtr>> parse_ElifList();\n";
        }
        else if (nt == "ElifBranch")
        {
            header << "    std::pair<ExprPtr, ExprPtr> parse_ElifBranch();\n";
        }
        else if (nt == "ArgIdList")
        {
            header << "    std::vector<std::string> parse_ArgIdList();\n";
        }
        else if (nt == "ArgIdListTail")
        {
            header << "    void parse_ArgIdListTail(std::vector<std::string>& args);\n";
        }
        else if (nt == "LetExpr")
        {
            header << "    Expr* parse_LetExpr();\n";
        }
        else if (nt == "LetBody")
        {
            header << "    Expr* parse_LetBody();\n";
        }
        else if (nt == "VarBindingList")
        {
            header << "    std::vector<std::pair<std::string, ExprPtr>> parse_VarBindingList();\n";
        }
        else if (nt == "VarBindingListTail")
        {
            header << "    void parse_VarBindingListTail(std::vector<std::pair<std::string, ExprPtr>>& bindings);\n";
        }
        else if (nt == "TypeDef")
        {
            header << "    Stmt* parse_TypeDef();\n";
        }
        else if (nt == "TypeParams")
        {
            header << "    std::vector<std::string> parse_TypeParams();\n";
        }
        else if (nt == "TypeInheritance")
        {
            header << "    std::pair<std::string, std::vector<ExprPtr>> parse_TypeInheritance();\n";
        }
        else if (nt == "TypeBaseArgs")
        {
            header << "    std::vector<ExprPtr> parse_TypeBaseArgs();\n";
        }
        else if (nt == "TypeBody")
        {
            header << "    std::pair<std::vector<std::unique_ptr<AttributeDecl>>, std::vector<std::unique_ptr<MethodDecl>>> parse_TypeBody();\n";
        }
        else if (nt == "TypeMember")
        {
            header << "    std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> parse_TypeMember();\n";
        }
        else if (nt == "TypeMemberTail")
        {
            header << "    std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> parse_TypeMemberTail(const std::string& memberName);\n";
        }
        else if (nt == "AttributeDef")
        {
            header << "    std::unique_ptr<AttributeDecl> parse_AttributeDef();\n";
        }
        else if (nt == "MethodDef")
        {
            header << "    std::unique_ptr<MethodDecl> parse_MethodDef();\n";
        }
        else
        {
            header << "    Expr* parse_" << method_name << "();\n";
        }
    }

    header << "private:\n";
    header << "    std::vector<Token> tokens;\n";
    header << "    size_t pos;\n";
    header << "    const Token& lookahead() const { return tokens[pos]; }\n";
    header << "    std::string tokenTypeToString(TokenType t) {\n";
    header << "        switch(t) {\n";
    header << "            case NUMBER: return \"NUMBER\";\n";
    header << "            case STRING: return \"STRING\";\n";
    header << "            case IDENT: return \"IDENT\";\n";
    header << "            case TRUE: return \"TRUE\";\n";
    header << "            case FALSE: return \"FALSE\";\n";
    header << "            case IF: return \"IF\";\n";
    header << "            case ELSE: return \"ELSE\";\n";
    header << "            case ELIF: return \"ELIF\";\n";
    header << "            case WHILE: return \"WHILE\";\n";
    header << "            case FOR: return \"FOR\";\n";
    header << "            case IN: return \"IN\";\n";
    header << "            case LET: return \"LET\";\n";
    header << "            case FUNCTION: return \"FUNCTION\";\n";
    header << "            case LPAREN: return \"LPAREN\";\n";
    header << "            case RPAREN: return \"RPAREN\";\n";
    header << "            case LBRACE: return \"LBRACE\";\n";
    header << "            case RBRACE: return \"RBRACE\";\n";
    header << "            case SEMICOLON: return \"SEMICOLON\";\n";
    header << "            case COMMA: return \"COMMA\";\n";
    header << "            case PLUS: return \"PLUS\";\n";
    header << "            case MINUS: return \"MINUS\";\n";
    header << "            case MULT: return \"MULT\";\n";
    header << "            case DIV: return \"DIV\";\n";
    header << "            case MOD: return \"MOD\";\n";
    header << "            case POW: return \"POW\";\n";
    header << "            case ASSIGN: return \"ASSIGN\";\n";
    header << "            case ASSIGN_DESTRUCT: return \"ASSIGN_DESTRUCT\";\n";
    header << "            case ARROW: return \"ARROW\";\n";
    header << "            case LESS_THAN: return \"LESS_THAN\";\n";
    header << "            case GREATER_THAN: return \"GREATER_THAN\";\n";
    header << "            case LE: return \"LE\";\n";
    header << "            case GE: return \"GE\";\n";
    header << "            case EQ: return \"EQ\";\n";
    header << "            case NEQ: return \"NEQ\";\n";
    header << "            case OR: return \"OR\";\n";
    header << "            case AND: return \"AND\";\n";
    header << "            case CONCAT: return \"CONCAT\";\n";
    header << "            case CONCAT_WS: return \"CONCAT_WS\";\n";
    header << "            case TYPE: return \"TYPE\";\n";
    header << "            case INHERITS: return \"INHERITS\";\n";
    header << "            case NEW: return \"NEW\";\n";
    header << "            case SELF: return \"SELF\";\n";
    header << "            case BASE: return \"BASE\";\n";
    header << "            case DOT: return \"DOT\";\n";
    header << "            case TOKEN_EOF: return \"EOF\";\n";
    header << "            default: return \"UNKNOWN\";\n";
    header << "        }\n";
    header << "    }\n";
    header << "    void match(TokenType t) { if (tokens[pos].type != t) { const Token& tok = tokens[pos]; std::cerr << \"Error de parseo: Se esperaba token \" << tokenTypeToString(t) << \" pero se encontró '\" << tok.lexeme << \"' (\" << tokenTypeToString(tok.type) << \") en línea \" << tok.line << \", columna \" << tok.column << std::endl; throw std::runtime_error(\"Token mismatch\"); } ++pos; }\n";
    header << "};\n";
    header.close();

    // --- Generar archivo de implementación ---
    std::ofstream out("ll1_parser_generated.cpp");
    if (!out)
    {
        std::cerr << "No se pudo crear el archivo ll1_parser_generated.cpp" << std::endl;
        return;
    }

    // Calcular FIRST sets
    auto first = compute_first(productions);

    // Calcular FIRST(Stmt) dinámicamente
    std::set<std::string> first_stmt;
    for (const auto &prod : productions)
    {
        if (prod.lhs == "Stmt" && !prod.rhs.empty())
        {
            std::string first_symbol = prod.rhs[0];
            if (first.count(first_symbol))
            {
                for (const auto &token : first[first_symbol])
                {
                    if (token != "ε")
                    {
                        first_stmt.insert(token);
                    }
                }
            }
        }
    }

    out << "#include \"ll1_parser_generated.hpp\"\n";
    out << "// --- Métodos de parseo ---\n";
    out << "LL1ParserGenerated::LL1ParserGenerated(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}\n";

    // Generar métodos para cada no terminal
    for (const auto &nt : nonterminals)
    {
        if (nt == "Program")
        {
            out << "Program* LL1ParserGenerated::parse_Program() {\n";
            out << "    std::cout << \"Aplicando: Program -> StmtList\\n\";\n";
            out << "    auto prog = new Program();\n";
            out << "    prog->stmts = parse_StmtList();\n";
            out << "    return prog;\n";
            out << "}\n";
            continue;
        }
        std::string method_name = nt;
        std::replace(method_name.begin(), method_name.end(), '\'', '_');
        method_name = std::string(std::regex_replace(method_name, std::regex("_+"), "_prime"));
        if (nt == "StmtList")
        {
            out << "std::vector<StmtPtr> LL1ParserGenerated::parse_StmtList() {\n";
            out << "    std::vector<StmtPtr> stmts;\n";
            out << "    // FIRST(Stmt): ";
            bool first_token = true;
            for (const auto &token : first_stmt)
            {
                if (!first_token)
                    out << " ";
                out << token;
                first_token = false;
            }
            out << "\n";
            out << "    TokenType t = lookahead().type;\n";
            out << "    if (";
            first_token = true;
            for (const auto &token : first_stmt)
            {
                if (!first_token)
                    out << " || ";
                out << "t == " << token;
                first_token = false;
            }
            out << ") {\n";
            out << "        Stmt* stmt = parse_Stmt();\n";
            out << "        std::vector<StmtPtr> tail = parse_StmtListTail();\n";
            out << "        stmts.push_back(StmtPtr(stmt));\n";
            out << "        stmts.insert(stmts.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));\n";
            out << "    }\n";
            out << "    return stmts;\n";
            out << "}\n";
        }
        else if (nt == "StmtListTail")
        {
            out << "std::vector<StmtPtr> LL1ParserGenerated::parse_StmtListTail() {\n";
            out << "    std::vector<StmtPtr> stmts;\n";
            out << "    if (lookahead().type == SEMICOLON) {\n";
            out << "        match(SEMICOLON);\n";
            out << "        std::vector<StmtPtr> tail = parse_StmtList();\n";
            out << "        stmts.insert(stmts.end(), std::make_move_iterator(tail.begin()), std::make_move_iterator(tail.end()));\n";
            out << "    }\n";
            out << "    return stmts;\n";
            out << "}\n";
        }
        else if (nt == "Stmt")
        {
            out << "Stmt* LL1ParserGenerated::parse_Stmt() {\n";
            out << "    TokenType t = lookahead().type;\n";
            out << "    if (t == WHILE) { return new ExprStmt(ExprPtr(parse_WhileStmt())); }\n";
            out << "    else if (t == FOR) { return new ExprStmt(ExprPtr(parse_ForStmt())); }\n";
            out << "    else if (t == LBRACE) { return new ExprStmt(ExprPtr(parse_BlockStmt())); }\n";
            out << "    else if (t == FUNCTION) { return parse_FunctionDef(); }\n";
            out << "    else if (t == TYPE) { return parse_TypeDef(); }\n";
            out << "    else { return new ExprStmt(ExprPtr(parse_Expr())); }\n";
            out << "}\n";
        }
        else if (nt == "WhileStmt")
        {
            out << "Expr* LL1ParserGenerated::parse_WhileStmt() {\n";
            out << "    match(WHILE);\n";
            out << "    Expr* condition = parse_Expr();\n";
            out << "    Expr* body = parse_WhileBody();\n";
            out << "    return new WhileExpr(ExprPtr(condition), ExprPtr(body));\n";
            out << "}\n";
        }
        else if (nt == "WhileBody")
        {
            out << "Expr* LL1ParserGenerated::parse_WhileBody() {\n";
            out << "    if (lookahead().type == LBRACE) {\n";
            out << "        return parse_BlockStmt();\n";
            out << "    } else {\n";
            out << "        return parse_Expr();\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "ForStmt")
        {
            out << "Expr* LL1ParserGenerated::parse_ForStmt() {\n";
            out << "    match(FOR); match(LPAREN);\n";
            out << "    std::string var = lookahead().lexeme; match(IDENT);\n";
            out << "    match(IN);\n";
            out << "    Expr* iterable = parse_Expr();\n";
            out << "    match(RPAREN);\n";
            out << "    Expr* body = parse_ForBody();\n";
            out << "    return new ForExpr(var, ExprPtr(iterable), ExprPtr(body));\n";
            out << "}\n";
        }
        else if (nt == "ForBody")
        {
            out << "Expr* LL1ParserGenerated::parse_ForBody() {\n";
            out << "    if (lookahead().type == LBRACE) {\n";
            out << "        return parse_BlockStmt();\n";
            out << "    } else {\n";
            out << "        return parse_Expr();\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "BlockStmt")
        {
            out << "Expr* LL1ParserGenerated::parse_BlockStmt() {\n";
            out << "    match(LBRACE);\n";
            out << "    std::vector<StmtPtr> stmts;\n";
            out << "    while (lookahead().type != RBRACE && lookahead().type != TOKEN_EOF) {\n";
            out << "        Stmt* stmt = parse_Stmt();\n";
            out << "        if (lookahead().type == SEMICOLON) match(SEMICOLON);\n";
            out << "        stmts.push_back(StmtPtr(stmt));\n";
            out << "    }\n";
            out << "    match(RBRACE);\n";
            out << "    return new ExprBlock(std::move(stmts));\n";
            out << "}\n";
        }
        else if (nt == "Primary")
        {
            out << "Expr* LL1ParserGenerated::parse_Primary() {\n";
            out << "    std::cout << \"Aplicando: Primary -> ...\\n\";\n";
            out << "    const Token& tok = lookahead();\n";
            out << "    if (tok.type == NUMBER) { match(NUMBER); auto expr = new NumberExpr(std::stod(tok.lexeme)); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == STRING) { match(STRING); std::string val = tok.lexeme; if (!val.empty() && val.front() == '\"' && val.back() == '\"') val = val.substr(1, val.size()-2); auto expr = new StringExpr(val); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == TRUE) { match(TRUE); auto expr = new BooleanExpr(true); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == FALSE) { match(FALSE); auto expr = new BooleanExpr(false); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == IDENT) { match(IDENT); auto expr = parse_PrimaryTail(tok.lexeme); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == LPAREN) { match(LPAREN); Expr* e = parse_Expr(); match(RPAREN); e->line_number = tok.line; e->column_number = tok.column; return e; }\n";
            out << "    if (tok.type == NEW) { match(NEW); std::string typeName = lookahead().lexeme; match(IDENT); match(LPAREN); std::vector<ExprPtr> args = parse_ArgList(); match(RPAREN); auto expr = new NewExpr(typeName, std::move(args)); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == SELF) { match(SELF); auto expr = parse_PrimaryTail(\"self\"); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    if (tok.type == BASE) { match(BASE); match(LPAREN); std::vector<ExprPtr> args = parse_ArgList(); match(RPAREN); auto expr = new BaseCallExpr(std::move(args)); expr->line_number = tok.line; expr->column_number = tok.column; return expr; }\n";
            out << "    std::cerr << \"Error de parseo: Token inesperado '\" << tok.lexeme << \"' (tipo: \" << static_cast<int>(tok.type) << \") en línea \" << tok.line << \", columna \" << tok.column << std::endl;\n";
            out << "    throw std::runtime_error(\"Unexpected token in Primary\");\n";
            out << "}\n";
        }
        else if (nt == "PrimaryTail")
        {
            out << "Expr* LL1ParserGenerated::parse_PrimaryTail(const std::string& varName) {\n";
            out << "    const Token& tok = lookahead();\n";
            out << "    Expr* expr = new VariableExpr(varName);\n";
            out << "    expr->line_number = tok.line; expr->column_number = tok.column;\n";
            out << "    while (true) {\n";
            out << "        if (lookahead().type == LPAREN) {\n";
            out << "            match(LPAREN);\n";
            out << "            std::vector<ExprPtr> args = parse_ArgList();\n";
            out << "            match(RPAREN);\n";
            out << "            // Si expr es VariableExpr, es una llamada a función normal\n";
            out << "            if (auto var = dynamic_cast<VariableExpr*>(expr)) {\n";
            out << "                auto callExpr = new CallExpr(var->name, std::move(args));\n";
            out << "                callExpr->line_number = tok.line; callExpr->column_number = tok.column;\n";
            out << "                delete expr;\n";
            out << "                expr = callExpr;\n";
            out << "            } else {\n";
            out << "                // Si no es VariableExpr, es una llamada a método\n";
            out << "                std::string methodName = \"\";\n";
            out << "                if (auto getAttr = dynamic_cast<GetAttrExpr*>(expr)) {\n";
            out << "                    methodName = getAttr->attrName;\n";
            out << "                    auto methodCall = new MethodCallExpr(std::move(getAttr->object), methodName, std::move(args));\n";
            out << "                    methodCall->line_number = tok.line; methodCall->column_number = tok.column;\n";
            out << "                    delete getAttr;\n";
            out << "                    expr = methodCall;\n";
            out << "                } else {\n";
            out << "                    // Fallback: tratar como llamada a función\n";
            out << "                    auto callExpr = new CallExpr(\"\", std::move(args));\n";
            out << "                    callExpr->line_number = tok.line; callExpr->column_number = tok.column;\n";
            out << "                    delete expr;\n";
            out << "                    expr = callExpr;\n";
            out << "                }\n";
            out << "            }\n";
            out << "        } else if (lookahead().type == DOT) {\n";
            out << "            match(DOT);\n";
            out << "            std::string memberName = lookahead().lexeme; match(IDENT);\n";
            out << "            auto getAttr = new GetAttrExpr(ExprPtr(expr), memberName);\n";
            out << "            getAttr->line_number = tok.line; getAttr->column_number = tok.column;\n";
            out << "            expr = getAttr;\n";
            out << "        } else if (lookahead().type == ASSIGN_DESTRUCT) {\n";
            out << "            match(ASSIGN_DESTRUCT);\n";
            out << "            Expr* value = parse_Expr();\n";
            out << "            // Si expr es GetAttrExpr, asignar a atributo; si es VariableExpr, asignar a variable\n";
            out << "            if (auto getAttr = dynamic_cast<GetAttrExpr*>(expr)) {\n";
            out << "                auto setAttr = new SetAttrExpr(std::move(getAttr->object), getAttr->attrName, ExprPtr(value));\n";
            out << "                setAttr->line_number = tok.line; setAttr->column_number = tok.column;\n";
            out << "                delete getAttr;\n";
            out << "                return setAttr;\n";
            out << "            } else if (auto var = dynamic_cast<VariableExpr*>(expr)) {\n";
            out << "                auto assignExpr = new AssignExpr(var->name, ExprPtr(value));\n";
            out << "                assignExpr->line_number = tok.line; assignExpr->column_number = tok.column;\n";
            out << "                delete var;\n";
            out << "                return assignExpr;\n";
            out << "            } else {\n";
            out << "                std::cerr << \"Error: Asignación destructiva inválida\\n\";\n";
            out << "                throw std::runtime_error(\"Invalid destructive assignment\");\n";
            out << "            }\n";
            out << "        } else if (lookahead().type == ASSIGN) {\n";
            out << "            const Token& tok = lookahead();\n";
            out << "            std::cerr << \"Error de sintaxis: Asignación normal (=) no está permitida. Use asignación destructiva (:=) en línea \" << tok.line << \" columna \" << tok.column << std::endl;\n";
            out << "            throw std::runtime_error(\"Invalid assignment operator\");\n";
            out << "        } else {\n";
            out << "            // No hay más tails, terminar y devolver la expresión actual\n";
            out << "            break;\n";
            out << "        }\n";
            out << "    }\n";
            out << "    return expr;\n";
            out << "}\n";
        }
        else if (nt == "FunctionDef")
        {
            out << "Stmt* LL1ParserGenerated::parse_FunctionDef() {\n";
            out << "    const Token& funcToken = lookahead();\n";
            out << "    match(FUNCTION);\n";
            out << "    std::string name = lookahead().lexeme; match(IDENT);\n";
            out << "    match(LPAREN);\n";
            out << "    std::vector<std::string> args = parse_ArgIdList();\n";
            out << "    match(RPAREN);\n";
            out << "    Expr* bodyExpr = parse_FunctionBody();\n";
            out << "    StmtPtr body = std::make_unique<ExprStmt>(ExprPtr(bodyExpr));\n";
            out << "    \n";
            out << "    // Crear lista de tipos de parámetros con la misma cantidad que args\n";
            out << "    std::vector<std::shared_ptr<TypeInfo>> paramTypes;\n";
            out << "    for (size_t i = 0; i < args.size(); ++i) {\n";
            out << "        paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));\n";
            out << "    }\n";
            out << "    \n";
            out << "    // Crear tipo de retorno por defecto (Unknown)\n";
            out << "    auto returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);\n";
            out << "    \n";
            out << "    auto funcDecl = new FunctionDecl(name, std::move(args), std::move(body), std::move(paramTypes), std::move(returnType));\n";
            out << "    funcDecl->line_number = funcToken.line; funcDecl->column_number = funcToken.column;\n";
            out << "    return funcDecl;\n";
            out << "}\n";
        }
        else if (nt == "ArgList")
        {
            out << "std::vector<ExprPtr> LL1ParserGenerated::parse_ArgList() {\n";
            out << "    std::vector<ExprPtr> args;\n";
            out << "    if (lookahead().type != RPAREN) {\n";
            out << "        args.push_back(ExprPtr(parse_Expr()));\n";
            out << "        while (lookahead().type == COMMA) {\n";
            out << "            match(COMMA);\n";
            out << "            args.push_back(ExprPtr(parse_Expr()));\n";
            out << "        }\n";
            out << "    }\n";
            out << "    return args;\n";
            out << "}\n";
        }
        else if (nt == "ArgListTail")
        {
            out << "void LL1ParserGenerated::parse_ArgListTail(std::vector<std::string>& args) {\n";
            out << "    if (lookahead().type == COMMA) {\n";
            out << "        match(COMMA);\n";
            out << "        args.push_back(lookahead().lexeme); match(IDENT);\n";
            out << "        parse_ArgListTail(args);\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "FunctionBody")
        {
            out << "Expr* LL1ParserGenerated::parse_FunctionBody() {\n";
            out << "    if (lookahead().type == ARROW) {\n";
            out << "        match(ARROW);\n";
            out << "        Expr* body = parse_Expr();\n";
            out << "        // No consumir SEMICOLON aquí, será manejado por parse_StmtListTail\n";
            out << "        return body;\n";
            out << "    } else {\n";
            out << "        return parse_BlockStmt();\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "Expr")
        {
            out << "Expr* LL1ParserGenerated::parse_Expr() {\n";
            out << "    TokenType t = lookahead().type;\n";
            out << "    if (t == IF) {\n";
            out << "        return parse_IfExpr();\n";
            out << "    } else if (t == LET) {\n";
            out << "        return parse_LetExpr();\n";
            out << "    } else {\n";
            out << "        return parse_OrExpr();\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "IfExpr")
        {
            out << "Expr* LL1ParserGenerated::parse_IfExpr() {\n";
            out << "    const Token& ifToken = lookahead();\n";
            out << "    match(IF); match(LPAREN);\n";
            out << "    Expr* condition = parse_Expr();\n";
            out << "    match(RPAREN);\n";
            out << "    Expr* thenBranch = parse_IfBody();\n";
            out << "    std::vector<std::pair<ExprPtr, ExprPtr>> elifs = parse_ElifList();\n";
            out << "    match(ELSE);\n";
            out << "    Expr* elseBranch = parse_IfBody();\n";
            out << "    \n";
            out << "    // Construir if-elif-else anidados\\n\";\n";
            out << "    Expr* result = elseBranch;\n";
            out << "    for (auto it = elifs.rbegin(); it != elifs.rend(); ++it) {\n";
            out << "        auto ifExpr = new IfExpr(std::move(it->first), std::move(it->second), ExprPtr(result));\n";
            out << "        ifExpr->line_number = ifToken.line; ifExpr->column_number = ifToken.column;\n";
            out << "        result = ifExpr;\n";
            out << "    }\n";
            out << "    auto ifExpr = new IfExpr(ExprPtr(condition), ExprPtr(thenBranch), ExprPtr(result));\n";
            out << "    ifExpr->line_number = ifToken.line; ifExpr->column_number = ifToken.column;\n";
            out << "    return ifExpr;\n";
            out << "}\n";
        }
        else if (nt == "IfBody")
        {
            out << "Expr* LL1ParserGenerated::parse_IfBody() {\n";
            out << "    if (lookahead().type == LBRACE) {\n";
            out << "        return parse_BlockStmt();\n";
            out << "    } else {\n";
            out << "        return parse_Expr();\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "ElifList")
        {
            out << "std::vector<std::pair<ExprPtr, ExprPtr>> LL1ParserGenerated::parse_ElifList() {\n";
            out << "    std::vector<std::pair<ExprPtr, ExprPtr>> elifs;\n";
            out << "    while (lookahead().type == ELIF) {\n";
            out << "        auto elif = parse_ElifBranch();\n";
            out << "        elifs.emplace_back(std::move(elif.first), std::move(elif.second));\n";
            out << "    }\n";
            out << "    return elifs;\n";
            out << "}\n";
        }
        else if (nt == "ElifBranch")
        {
            out << "std::pair<ExprPtr, ExprPtr> LL1ParserGenerated::parse_ElifBranch() {\n";
            out << "    match(ELIF); match(LPAREN);\n";
            out << "    Expr* condition = parse_Expr();\n";
            out << "    match(RPAREN);\n";
            out << "    Expr* thenBranch = parse_IfBody();\n";
            out << "    return {ExprPtr(condition), ExprPtr(thenBranch)};\n";
            out << "}\n";
        }
        else if (nt == "ArgIdList")
        {
            out << "std::vector<std::string> LL1ParserGenerated::parse_ArgIdList() {\n";
            out << "    std::vector<std::string> args;\n";
            out << "    if (lookahead().type == IDENT) {\n";
            out << "        args.push_back(lookahead().lexeme); match(IDENT);\n";
            out << "        parse_ArgIdListTail(args);\n";
            out << "    }\n";
            out << "    return args;\n";
            out << "}\n";
        }
        else if (nt == "ArgIdListTail")
        {
            out << "void LL1ParserGenerated::parse_ArgIdListTail(std::vector<std::string>& args) {\n";
            out << "    if (lookahead().type == COMMA) {\n";
            out << "        match(COMMA);\n";
            out << "        args.push_back(lookahead().lexeme); match(IDENT);\n";
            out << "        parse_ArgIdListTail(args);\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "LetExpr")
        {
            out << "Expr* LL1ParserGenerated::parse_LetExpr() {\n";
            out << "    const Token& letToken = lookahead();\n";
            out << "    match(LET);\n";
            out << "    std::vector<std::pair<std::string, ExprPtr>> bindings = parse_VarBindingList();\n";
            out << "    match(IN);\n";
            out << "    Expr* body = parse_LetBody();\n";
            out << "    \n";
            out << "    // Crear let expressions anidados para múltiples variables\\n\";\n";
            out << "    Expr* result = body;\n";
            out << "    for (auto it = bindings.rbegin(); it != bindings.rend(); ++it) {\n";
            out << "        auto letExpr = new LetExpr(it->first, std::move(it->second), StmtPtr(new ExprStmt(ExprPtr(result))));\n";
            out << "        letExpr->line_number = letToken.line; letExpr->column_number = letToken.column;\n";
            out << "        result = letExpr;\n";
            out << "    }\n";
            out << "    return result;\n";
            out << "}\n";
        }
        else if (nt == "VarBindingList")
        {
            out << "std::vector<std::pair<std::string, ExprPtr>> LL1ParserGenerated::parse_VarBindingList() {\n";
            out << "    std::vector<std::pair<std::string, ExprPtr>> bindings;\n";
            out << "    std::string varName = lookahead().lexeme; match(IDENT);\n";
            out << "    match(ASSIGN);\n";
            out << "    Expr* value = parse_Expr();\n";
            out << "    bindings.emplace_back(varName, ExprPtr(value));\n";
            out << "    parse_VarBindingListTail(bindings);\n";
            out << "    return bindings;\n";
            out << "}\n";
        }
        else if (nt == "VarBindingListTail")
        {
            out << "void LL1ParserGenerated::parse_VarBindingListTail(std::vector<std::pair<std::string, ExprPtr>>& bindings) {\n";
            out << "    if (lookahead().type == COMMA) {\n";
            out << "        match(COMMA);\n";
            out << "        std::string varName = lookahead().lexeme; match(IDENT);\n";
            out << "        match(ASSIGN);\n";
            out << "        Expr* value = parse_Expr();\n";
            out << "        bindings.emplace_back(varName, ExprPtr(value));\n";
            out << "        parse_VarBindingListTail(bindings);\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "LetBody")
        {
            out << "Expr* LL1ParserGenerated::parse_LetBody() {\n";
            out << "    if (lookahead().type == LBRACE) {\n";
            out << "        return parse_BlockStmt();\n";
            out << "    } else {\n";
            out << "        return parse_Expr();\n";
            out << "    }\n";
            out << "}\n";
        }
        else if (nt == "TypeDef")
        {
            out << "Stmt* LL1ParserGenerated::parse_TypeDef() {\n";
            out << "    const Token& typeToken = lookahead();\n";
            out << "    match(TYPE);\n";
            out << "    std::string name = lookahead().lexeme; match(IDENT);\n";
            out << "    std::vector<std::string> params = parse_TypeParams();\n";
            out << "    auto inheritance = parse_TypeInheritance();\n";
            out << "    match(LBRACE);\n";
            out << "    auto body = parse_TypeBody();\n";
            out << "    match(RBRACE);\n";
            out << "    \n";
            out << "    // Crear listas de tipos de parámetros con la misma cantidad que params\n";
            out << "    std::vector<std::shared_ptr<TypeInfo>> paramTypes;\n";
            out << "    for (size_t i = 0; i < params.size(); ++i) {\n";
            out << "        paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));\n";
            out << "    }\n";
            out << "    \n";
            out << "    auto typeDecl = new TypeDecl(name, std::move(params), std::move(paramTypes), std::move(body.first), std::move(body.second), inheritance.first, std::move(inheritance.second));\n";
            out << "    typeDecl->line_number = typeToken.line; typeDecl->column_number = typeToken.column;\n";
            out << "    return typeDecl;\n";
            out << "}\n";
        }
        else if (nt == "TypeParams")
        {
            out << "std::vector<std::string> LL1ParserGenerated::parse_TypeParams() {\n";
            out << "    if (lookahead().type == LPAREN) {\n";
            out << "        match(LPAREN);\n";
            out << "        std::vector<std::string> params = parse_ArgIdList();\n";
            out << "        match(RPAREN);\n";
            out << "        return params;\n";
            out << "    }\n";
            out << "    return {};\n";
            out << "}\n";
        }
        else if (nt == "TypeInheritance")
        {
            out << "std::pair<std::string, std::vector<ExprPtr>> LL1ParserGenerated::parse_TypeInheritance() {\n";
            out << "    if (lookahead().type == INHERITS) {\n";
            out << "        match(INHERITS);\n";
            out << "        std::string baseType = lookahead().lexeme; match(IDENT);\n";
            out << "        std::vector<ExprPtr> baseArgs = parse_TypeBaseArgs();\n";
            out << "        return std::make_pair(std::move(baseType), std::move(baseArgs));\n";
            out << "    }\n";
            out << "    return std::make_pair(\"Object\", std::vector<ExprPtr>{});\n";
            out << "}\n";
        }
        else if (nt == "TypeBaseArgs")
        {
            out << "std::vector<ExprPtr> LL1ParserGenerated::parse_TypeBaseArgs() {\n";
            out << "    if (lookahead().type == LPAREN) {\n";
            out << "        match(LPAREN);\n";
            out << "        std::vector<ExprPtr> args = parse_ArgList();\n";
            out << "        match(RPAREN);\n";
            out << "        return args;\n";
            out << "    }\n";
            out << "    return {};\n";
            out << "}\n";
        }
        else if (nt == "TypeBody")
        {
            out << "std::pair<std::vector<std::unique_ptr<AttributeDecl>>, std::vector<std::unique_ptr<MethodDecl>>> LL1ParserGenerated::parse_TypeBody() {\n";
            out << "    std::vector<std::unique_ptr<AttributeDecl>> attributes;\n";
            out << "    std::vector<std::unique_ptr<MethodDecl>> methods;\n";
            out << "    \n";
            out << "    while (lookahead().type != RBRACE && lookahead().type != TOKEN_EOF) {\n";
            out << "        auto member = parse_TypeMember();\n";
            out << "        if (member.first) {\n";
            out << "            attributes.push_back(std::move(member.first));\n";
            out << "        }\n";
            out << "        if (member.second) {\n";
            out << "            methods.push_back(std::move(member.second));\n";
            out << "        }\n";
            out << "    }\n";
            out << "    \n";
            out << "    return {std::move(attributes), std::move(methods)};\n";
            out << "}\n";
        }
        else if (nt == "TypeMember")
        {
            out << "std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> LL1ParserGenerated::parse_TypeMember() {\n";
            out << "    std::string memberName = lookahead().lexeme; match(IDENT);\n";
            out << "    return parse_TypeMemberTail(memberName);\n";
            out << "}\n";
        }
        else if (nt == "TypeMemberTail")
        {
            out << "std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> LL1ParserGenerated::parse_TypeMemberTail(const std::string& memberName) {\n";
            out << "    if (lookahead().type == ASSIGN) {\n";
            out << "        // Es un atributo\n";
            out << "        match(ASSIGN);\n";
            out << "        Expr* initializer = parse_Expr();\n";
            out << "        match(SEMICOLON);\n";
            out << "        auto attrDecl = std::make_unique<AttributeDecl>(memberName, ExprPtr(initializer));\n";
            out << "        return {std::move(attrDecl), std::unique_ptr<MethodDecl>(nullptr)};\n";
            out << "    } else if (lookahead().type == LPAREN) {\n";
            out << "        // Es un método\n";
            out << "        match(LPAREN);\n";
            out << "        std::vector<std::string> params = parse_ArgIdList();\n";
            out << "        match(RPAREN);\n";
            out << "        Expr* bodyExpr = parse_FunctionBody();\n";
            out << "        match(SEMICOLON); // Consumir el SEMICOLON después del método\n";
            out << "        StmtPtr body = std::make_unique<ExprStmt>(ExprPtr(bodyExpr));\n";
            out << "        \n";
            out << "        // Crear lista de tipos de parámetros con la misma cantidad que params\n";
            out << "        std::vector<std::shared_ptr<TypeInfo>> paramTypes;\n";
            out << "        for (size_t i = 0; i < params.size(); ++i) {\n";
            out << "            paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));\n";
            out << "        }\n";
            out << "        \n";
            out << "        // Crear tipo de retorno por defecto (Unknown)\n";
            out << "        auto returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);\n";
            out << "        \n";
            out << "        auto methodDecl = std::make_unique<MethodDecl>(memberName, std::move(params), std::move(body), std::move(paramTypes), std::move(returnType));\n";
            out << "        return {std::unique_ptr<AttributeDecl>(nullptr), std::move(methodDecl)};\n";
            out << "    }\n";
            out << "    return {std::unique_ptr<AttributeDecl>(nullptr), std::unique_ptr<MethodDecl>(nullptr)};\n";
            out << "}\n";
        }
        else if (nt == "AttributeDef")
        {
            out << "std::unique_ptr<AttributeDecl> LL1ParserGenerated::parse_AttributeDef() {\n";
            out << "    const Token& attrToken = lookahead();\n";
            out << "    std::string name = attrToken.lexeme; match(IDENT);\n";
            out << "    match(ASSIGN);\n";
            out << "    Expr* initializer = parse_Expr();\n";
            out << "    match(SEMICOLON);\n";
            out << "    \n";
            out << "    auto attrDecl = std::make_unique<AttributeDecl>(name, ExprPtr(initializer));\n";
            out << "    attrDecl->line_number = attrToken.line; attrDecl->column_number = attrToken.column;\n";
            out << "    return attrDecl;\n";
            out << "}\n";
        }
        else if (nt == "MethodDef")
        {
            out << "std::unique_ptr<MethodDecl> LL1ParserGenerated::parse_MethodDef() {\n";
            out << "    const Token& methodToken = lookahead();\n";
            out << "    std::string name = methodToken.lexeme; match(IDENT);\n";
            out << "    match(LPAREN);\n";
            out << "    std::vector<std::string> params = parse_ArgIdList();\n";
            out << "    match(RPAREN);\n";
            out << "    Expr* bodyExpr = parse_FunctionBody();\n";
            out << "    StmtPtr body = std::make_unique<ExprStmt>(ExprPtr(bodyExpr));\n";
            out << "    \n";
            out << "    // Crear lista de tipos de parámetros con la misma cantidad que params\n";
            out << "    std::vector<std::shared_ptr<TypeInfo>> paramTypes;\n";
            out << "    for (size_t i = 0; i < params.size(); ++i) {\n";
            out << "        paramTypes.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));\n";
            out << "    }\n";
            out << "    \n";
            out << "    // Crear tipo de retorno por defecto (Unknown)\n";
            out << "    auto returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);\n";
            out << "    \n";
            out << "    auto methodDecl = std::make_unique<MethodDecl>(name, std::move(params), std::move(body), std::move(paramTypes), std::move(returnType));\n";
            out << "    methodDecl->line_number = methodToken.line; methodDecl->column_number = methodToken.column;\n";
            out << "    return methodDecl;\n";
            out << "}\n";
        }
        else
        {
            out << "Expr* LL1ParserGenerated::parse_" << method_name << "() {\n";
            out << "    std::cout << \"Aplicando: " << nt << " -> ...\\n\";\n";
            if (nt == "OrExpr")
            {
                out << "    Expr* left = parse_AndExpr();\n";
                out << "    while (lookahead().type == OR) {\n";
                out << "        const Token& opToken = lookahead();\n";
                out << "        match(OR);\n";
                out << "        Expr* right = parse_AndExpr();\n";
                out << "        auto binExpr = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr(left), ExprPtr(right));\n";
                out << "        binExpr->line_number = opToken.line; binExpr->column_number = opToken.column;\n";
                out << "        left = binExpr;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "AndExpr")
            {
                out << "    Expr* left = parse_CmpExpr();\n";
                out << "    while (lookahead().type == AND) {\n";
                out << "        const Token& opToken = lookahead();\n";
                out << "        match(AND);\n";
                out << "        Expr* right = parse_CmpExpr();\n";
                out << "        auto binExpr = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr(left), ExprPtr(right));\n";
                out << "        binExpr->line_number = opToken.line; binExpr->column_number = opToken.column;\n";
                out << "        left = binExpr;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "CmpExpr")
            {
                out << "    Expr* left = parse_ConcatExpr();\n";
                out << "    while (true) {\n";
                out << "        TokenType t = lookahead().type;\n";
                out << "        if (t == LESS_THAN) { const Token& opToken = lookahead(); match(LESS_THAN); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == GREATER_THAN) { const Token& opToken = lookahead(); match(GREATER_THAN); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == LE) { const Token& opToken = lookahead(); match(LE); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == GE) { const Token& opToken = lookahead(); match(GE); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == EQ) { const Token& opToken = lookahead(); match(EQ); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == NEQ) { const Token& opToken = lookahead(); match(NEQ); Expr* right = parse_ConcatExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else break;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "ConcatExpr")
            {
                out << "    Expr* left = parse_AddExpr();\n";
                out << "    while (true) {\n";
                out << "        TokenType t = lookahead().type;\n";
                out << "        if (t == CONCAT) { const Token& opToken = lookahead(); match(CONCAT); Expr* right = parse_AddExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == CONCAT_WS) { const Token& opToken = lookahead(); match(CONCAT_WS); Expr* right = parse_AddExpr(); auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else break;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "AddExpr")
            {
                out << "    Expr* left = parse_Term();\n";
                out << "    while (true) {\n";
                out << "        TokenType t = lookahead().type;\n";
                out << "        if (t == PLUS) { const Token& opToken = lookahead(); match(PLUS); Expr* right = parse_Term(); auto binExpr = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == MINUS) { const Token& opToken = lookahead(); match(MINUS); Expr* right = parse_Term(); auto binExpr = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else break;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "Term")
            {
                out << "    Expr* left = parse_Factor();\n";
                out << "    while (true) {\n";
                out << "        TokenType t = lookahead().type;\n";
                out << "        if (t == MULT) { const Token& opToken = lookahead(); match(MULT); Expr* right = parse_Factor(); auto binExpr = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == DIV) { const Token& opToken = lookahead(); match(DIV); Expr* right = parse_Factor(); auto binExpr = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else if (t == MOD) { const Token& opToken = lookahead(); match(MOD); Expr* right = parse_Factor(); auto binExpr = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr(left), ExprPtr(right)); binExpr->line_number = opToken.line; binExpr->column_number = opToken.column; left = binExpr; }\n";
                out << "        else break;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "Factor")
            {
                out << "    Expr* left = parse_Power();\n";
                out << "    while (lookahead().type == POW) {\n";
                out << "        const Token& opToken = lookahead();\n";
                out << "        match(POW);\n";
                out << "        Expr* right = parse_Power();\n";
                out << "        auto binExpr = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr(left), ExprPtr(right));\n";
                out << "        binExpr->line_number = opToken.line; binExpr->column_number = opToken.column;\n";
                out << "        left = binExpr;\n";
                out << "    }\n";
                out << "    return left;\n";
            }
            else if (nt == "Power")
            {
                out << "    return parse_Unary();\n";
            }
            else if (nt == "Unary")
            {
                out << "    if (lookahead().type == MINUS) { const Token& opToken = lookahead(); match(MINUS); Expr* operand = parse_Unary(); auto unaryExpr = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr(operand)); unaryExpr->line_number = opToken.line; unaryExpr->column_number = opToken.column; return unaryExpr; }\n";
                out << "    return parse_Primary();\n";
            }
            else
            {
                // No generar métodos para no-terminales que ya tienen implementación específica
                if (nt != "Primary" && nt != "PrimaryTail" && nt != "FunctionDef" && nt != "ArgList" && nt != "ArgListTail")
                {
                    out << "    // Implementar lógica de parseo para " << nt << "\n    return nullptr;\n";
                }
            }
            out << "}\n";
        }
    }
    out.close();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <archivo_gramatica.ll1>" << std::endl;
        return 1;
    }
    std::string grammar_file = argv[1];
    auto productions = read_grammar(grammar_file);
    std::cout << "Producciones leídas:\n";
    for (const auto &prod : productions)
    {
        std::cout << prod.lhs << " -> ";
        for (const auto &sym : prod.rhs)
            std::cout << sym << ' ';
        std::cout << std::endl;
    }
    // Agrupar producciones por LHS y depurar
    auto grouped = group_productions(productions);
    std::cout << "\nAgrupamiento de producciones por LHS (depuración):\n";
    for (const auto &[lhs, alternatives] : grouped)
    {
        std::cout << lhs << ":\n";
        for (const auto &rhs : alternatives)
        {
            std::cout << "    ";
            for (const auto &sym : rhs)
                std::cout << sym << ' ';
            std::cout << std::endl;
        }
    }
    // Calcular y mostrar FIRST
    auto first = compute_first(productions);
    std::cout << "\nConjuntos FIRST:\n";
    for (const auto &[sym, fset] : first)
    {
        std::cout << "FIRST(" << sym << ") = { ";
        for (const auto &t : fset)
            std::cout << t << ' ';
        std::cout << "}\n";
    }
    // Calcular y mostrar FOLLOW
    std::string start_symbol = productions.front().lhs;
    auto follow = compute_follow(productions, first, start_symbol);
    std::cout << "\nConjuntos FOLLOW:\n";
    for (const auto &[sym, fset] : follow)
    {
        std::cout << "FOLLOW(" << sym << ") = { ";
        for (const auto &t : fset)
            std::cout << t << ' ';
        std::cout << "}\n";
    }
    // Construir e imprimir la tabla LL(1)
    build_and_print_ll1_table(productions, first, follow);
    // Generar el parser C++
    std::set<std::string> nonterminals;
    for (const auto &prod : productions)
        nonterminals.insert(prod.lhs);
    generate_parser_cpp(nonterminals, productions);
    return 0;
}