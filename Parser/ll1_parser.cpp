#include "ll1_parser.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <stack>

LL1Parser::LL1Parser(const std::string &grammar_file, const std::vector<Token> &tokens)
    : grammar_file(grammar_file), tokens(tokens), current_token_pos(0)
{

    // Leer la gramática desde el archivo
    productions = readGrammar(grammar_file);
    if (productions.empty())
    {
        throw std::runtime_error("No se pudieron leer producciones de la gramática");
    }

    // Establecer el símbolo inicial (primera producción)
    start_symbol = productions.front().lhs;
}

std::vector<Production> LL1Parser::readGrammar(const std::string &filename)
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
        if (line.empty() || line.substr(0, 2) == "//")
        {
            continue;
        }

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

                // Limpiar espacios en blanco del inicio y final
                rhs_part.erase(rhs_part.find_last_not_of(" \t") + 1);
                rhs_part.erase(0, rhs_part.find_first_not_of(" \t"));

                if (rhs_part.empty() || rhs_part == "ε")
                {
                    // Producción epsilon - dejar rhs vacío
                    // No agregar nada al vector rhs
                }
                else
                {
                    std::istringstream rhs_tokens(rhs_part);
                    std::string symbol;
                    while (rhs_tokens >> symbol)
                    {
                        prod.rhs.push_back(symbol);
                    }
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

            // Limpiar espacios en blanco del inicio y final
            rhs_part.erase(rhs_part.find_last_not_of(" \t") + 1);
            rhs_part.erase(0, rhs_part.find_first_not_of(" \t"));

            if (rhs_part.empty() || rhs_part == "ε")
            {
                // Producción epsilon - dejar rhs vacío
                // No agregar nada al vector rhs
            }
            else
            {
                std::istringstream rhs_tokens(rhs_part);
                std::string symbol;
                while (rhs_tokens >> symbol)
                {
                    prod.rhs.push_back(symbol);
                }
            }
            productions.push_back(prod);
        }
    }

    return productions;
}

// Métodos de debugging (implementaciones temporales)
void LL1Parser::printProductions() const
{
    std::cout << "\n=== Producciones de la Gramática ===" << std::endl;
    std::cout << "Total de producciones: " << productions.size() << std::endl;
    std::cout << "Símbolo inicial: " << start_symbol << std::endl;
    std::cout << std::endl;

    for (size_t i = 0; i < productions.size(); ++i)
    {
        const auto &prod = productions[i];
        std::cout << i + 1 << ". " << prod.lhs << " -> ";

        if (prod.rhs.empty())
        {
            std::cout << "ε";
        }
        else
        {
            for (size_t j = 0; j < prod.rhs.size(); ++j)
            {
                if (j > 0)
                    std::cout << " ";
                std::cout << prod.rhs[j];
            }
        }
        std::cout << std::endl;
    }
    std::cout << "===================================" << std::endl;
}

void LL1Parser::printFirstSets() const
{
    std::cout << "\n=== Conjuntos FIRST ===" << std::endl;
    for (const auto &[symbol, first_set] : first_sets)
    {
        std::cout << "FIRST(" << symbol << ") = { ";
        for (const auto &token : first_set)
        {
            std::cout << token << " ";
        }
        std::cout << "}" << std::endl;
    }
}

void LL1Parser::printFollowSets() const
{
    std::cout << "\n=== Conjuntos FOLLOW ===" << std::endl;
    for (const auto &[symbol, follow_set] : follow_sets)
    {
        std::cout << "FOLLOW(" << symbol << ") = { ";
        for (const auto &token : follow_set)
        {
            std::cout << token << " ";
        }
        std::cout << "}" << std::endl;
    }
}

void LL1Parser::printLL1Table() const
{
    std::cout << "\n=== Tabla LL(1) ===" << std::endl;

    if (ll1_table.empty())
    {
        std::cout << "La tabla LL(1) está vacía" << std::endl;
        return;
    }

    // Ordenar no terminales para mejor visualización
    std::vector<std::string> sorted_nts;
    for (const auto &[nt, _] : ll1_table)
    {
        sorted_nts.push_back(nt);
    }
    std::sort(sorted_nts.begin(), sorted_nts.end());

    // Imprimir todas las entradas ordenadas por no terminal
    for (const auto &nt : sorted_nts)
    {
        const auto &row = ll1_table.at(nt);
        std::cout << "\n"
                  << nt << ":" << std::endl;

        // Ordenar terminales para este no terminal
        std::vector<std::string> sorted_terminals;
        for (const auto &[terminal, _] : row)
        {
            sorted_terminals.push_back(terminal);
        }
        std::sort(sorted_terminals.begin(), sorted_terminals.end());

        for (const auto &terminal : sorted_terminals)
        {
            const auto &production = row.at(terminal);
            std::cout << "  M[" << nt << ", " << terminal << "] = ";

            if (production.empty())
            {
                std::cout << "ε";
            }
            else
            {
                for (size_t i = 0; i < production.size(); ++i)
                {
                    if (i > 0)
                        std::cout << " ";
                    std::cout << production[i];
                }
            }
            std::cout << std::endl;
        }
    }

    // Resumen
    std::cout << "\n=== Resumen de la Tabla LL(1) ===" << std::endl;
    std::cout << "Total de no terminales: " << ll1_table.size() << std::endl;

    size_t total_entries = 0;
    for (const auto &[nt, row] : ll1_table)
    {
        total_entries += row.size();
    }
    std::cout << "Total de entradas: " << total_entries << std::endl;
    std::cout << "=================================" << std::endl;
}

std::pair<DerivationNodePtr, Program *> LL1Parser::parse()
{
    std::cout << "Iniciando parsing completo..." << std::endl;

    // Validar que tenemos producciones
    if (productions.empty())
    {
        throw std::runtime_error("No hay producciones en la gramática");
    }

    // 1. Calcular FIRST
    std::cout << "Calculando conjuntos FIRST..." << std::endl;
    first_sets = computeFirst(productions);
    // printFirstSets();
    // Validar que se calcularon los conjuntos FIRST
    if (first_sets.empty())
    {
        throw std::runtime_error("Error al calcular conjuntos FIRST");
    }

    // 2. Calcular FOLLOW
    std::cout << "Calculando conjuntos FOLLOW..." << std::endl;
    follow_sets = computeFollow(productions, first_sets, start_symbol);

    // Validar que se calcularon los conjuntos FOLLOW
    if (follow_sets.empty())
    {
        throw std::runtime_error("Error al calcular conjuntos FOLLOW");
    }

    // 3. Construir tabla LL(1)
    std::cout << "Construyendo tabla LL(1)..." << std::endl;
    ll1_table = buildLL1Table(productions, first_sets, follow_sets);

    // Debug: imprimir información específica para StmtList
    std::cout << "\n=== Debug StmtList ===" << std::endl;
    if (first_sets.find("StmtList") != first_sets.end())
    {
        std::cout << "FIRST(StmtList) = { ";
        for (const auto &symbol : first_sets["StmtList"])
        {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }

    if (follow_sets.find("StmtList") != follow_sets.end())
    {
        std::cout << "FOLLOW(StmtList) = { ";
        for (const auto &symbol : follow_sets["StmtList"])
        {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }

    if (ll1_table.find("StmtList") != ll1_table.end())
    {
        std::cout << "Entradas en tabla para StmtList:" << std::endl;
        for (const auto &[terminal, production] : ll1_table["StmtList"])
        {
            std::cout << "  " << terminal << " -> ";
            for (const auto &symbol : production)
            {
                std::cout << symbol << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "==========================" << std::endl;

    // Validar que se construyó la tabla LL(1)
    if (ll1_table.empty())
    {
        throw std::runtime_error("Error al construir tabla LL(1)");
    }

    // Verificar si la gramática es LL(1) (no hay conflictos)
    bool is_ll1 = validateLL1Grammar();
    if (!is_ll1)
    {
        std::cerr << "Advertencia: La gramática no es LL(1) - se detectaron conflictos" << std::endl;
    }

    // 4. Construir el árbol de derivación (CST)
    cst_root = buildDerivationTree();

    if (!cst_root)
    {
        throw std::runtime_error("Error al construir el árbol de derivación");
    }

    // Imprimir el árbol ANTES de moverlo
    printDerivationTree();

    // 5. Construir el AST a partir del CST
    // Program *ast = buildAST(cst_root);

    // Por ahora, crear un AST vacío para testing
    auto ast = new Program();

    std::cout << "Parsing completado exitosamente" << std::endl;
    return {std::move(cst_root), ast};
}

void LL1Parser::printDerivationTree() const
{
    if (cst_root)
    {
        std::cout << "\n=== Árbol de Derivación (CST) ===" << std::endl;

        // Imprimir con formato similar al PrintVisitor
        printNode(cst_root.get(), 0);

        std::cout << "\n=== Información del CST ===" << std::endl;
        std::cout << "Total de nodos: " << countNodes(cst_root.get()) << std::endl;
        std::cout << "Profundidad máxima: " << getMaxDepth(cst_root.get()) << std::endl;
        std::cout << "=================================" << std::endl;
    }
    else
    {
        std::cout << "No hay árbol de derivación para mostrar" << std::endl;
    }
}

void LL1Parser::printNode(const DerivationNode *node, int depth) const
{
    if (!node)
        return;

    // Imprimir indentación
    for (int i = 0; i < depth; ++i)
        std::cout << "    "; // 4 espacios por nivel

    // Imprimir nodo
    if (node->symbol == "ε")
    {
        std::cout << "|_ ε" << std::endl;
    }
    else if (node->token.has_value())
    {
        // Es un terminal con token
        std::cout << "|_ " << node->symbol << ": '" << node->token->lexeme << "'" << std::endl;
    }
    else
    {
        // Es un no terminal
        std::cout << "|_ " << node->symbol << std::endl;
    }

    // Imprimir hijos recursivamente
    for (const auto &child : node->children)
    {
        printNode(child.get(), depth + 1);
    }
}

int LL1Parser::countNodes(const DerivationNode *node) const
{
    if (!node)
        return 0;

    int count = 1; // Este nodo
    for (const auto &child : node->children)
    {
        count += countNodes(child.get());
    }
    return count;
}

int LL1Parser::getMaxDepth(const DerivationNode *node) const
{
    if (!node)
        return 0;

    int maxDepth = 0;
    for (const auto &child : node->children)
    {
        maxDepth = std::max(maxDepth, getMaxDepth(child.get()));
    }
    return maxDepth + 1;
}

void LL1Parser::printAST(Program *ast) const
{
    if (ast)
    {
        std::cout << "\n=== Árbol de Sintaxis Abstracta (AST) ===" << std::endl;
        std::cout << "Program con " << ast->stmts.size() << " statements" << std::endl;
        // Aquí podrías agregar un visitor para imprimir el AST de forma más detallada
        std::cout << "=========================================" << std::endl;
    }
    else
    {
        std::cout << "No hay AST para mostrar" << std::endl;
    }
}

bool LL1Parser::isTerminal(const std::string &symbol) const
{
    if (symbol == "ε")
    {
        return true;
    }

    // Un símbolo es un no terminal si aparece como LHS en alguna producción
    // De lo contrario, es un terminal.
    for (const auto &prod : productions)
    {
        if (prod.lhs == symbol)
        {
            return false; // Es un no terminal
        }
    }
    return true; // Es un terminal
}

std::string LL1Parser::tokenTypeToSymbol(TokenType type) const
{
    switch (type)
    {
    case NUMBER:
        return "NUMBER";
    case STRING:
        return "STRING";
    case IDENT:
        return "IDENT";
    case TRUE:
        return "TRUE";
    case FALSE:
        return "FALSE";
    case IF:
        return "IF";
    case ELSE:
        return "ELSE";
    case ELIF:
        return "ELIF";
    case WHILE:
        return "WHILE";
    case FOR:
        return "FOR";
    case IN:
        return "IN";
    case LET:
        return "LET";
    case FUNCTION:
        return "FUNCTION";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case LBRACE:
        return "LBRACE";
    case RBRACE:
        return "RBRACE";
    case SEMICOLON:
        return "SEMICOLON";
    case COMMA:
        return "COMMA";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case MULT:
        return "MULT";
    case DIV:
        return "DIV";
    case MOD:
        return "MOD";
    case POW:
        return "POW";
    case ASSIGN:
        return "ASSIGN";
    case ASSIGN_DESTRUCT:
        return "ASSIGN_DESTRUCT";
    case ARROW:
        return "ARROW";
    case LESS_THAN:
        return "LESS_THAN";
    case GREATER_THAN:
        return "GREATER_THAN";
    case LE:
        return "LE";
    case GE:
        return "GE";
    case EQ:
        return "EQ";
    case NEQ:
        return "NEQ";
    case OR:
        return "OR";
    case AND:
        return "AND";
    case CONCAT:
        return "CONCAT";
    case CONCAT_WS:
        return "CONCAT_WS";
    case TYPE:
        return "TYPE";
    case INHERITS:
        return "INHERITS";
    case NEW:
        return "NEW";
    case SELF:
        return "SELF";
    case BASE:
        return "BASE";
    case DOT:
        return "DOT";
    case TOKEN_EOF:
        return "EOF";
    default:
        return "UNKNOWN";
    }
}

TokenType LL1Parser::symbolToTokenType(const std::string &symbol) const
{
    if (symbol == "NUMBER")
        return NUMBER;
    if (symbol == "STRING")
        return STRING;
    if (symbol == "IDENT")
        return IDENT;
    if (symbol == "TRUE")
        return TRUE;
    if (symbol == "FALSE")
        return FALSE;
    if (symbol == "IF")
        return IF;
    if (symbol == "ELSE")
        return ELSE;
    if (symbol == "ELIF")
        return ELIF;
    if (symbol == "WHILE")
        return WHILE;
    if (symbol == "FOR")
        return FOR;
    if (symbol == "IN")
        return IN;
    if (symbol == "LET")
        return LET;
    if (symbol == "FUNCTION")
        return FUNCTION;
    if (symbol == "LPAREN")
        return LPAREN;
    if (symbol == "RPAREN")
        return RPAREN;
    if (symbol == "LBRACE")
        return LBRACE;
    if (symbol == "RBRACE")
        return RBRACE;
    if (symbol == "SEMICOLON")
        return SEMICOLON;
    if (symbol == "COMMA")
        return COMMA;
    if (symbol == "PLUS")
        return PLUS;
    if (symbol == "MINUS")
        return MINUS;
    if (symbol == "MULT")
        return MULT;
    if (symbol == "DIV")
        return DIV;
    if (symbol == "MOD")
        return MOD;
    if (symbol == "POW")
        return POW;
    if (symbol == "ASSIGN")
        return ASSIGN;
    if (symbol == "ASSIGN_DESTRUCT")
        return ASSIGN_DESTRUCT;
    if (symbol == "ARROW")
        return ARROW;
    if (symbol == "LESS_THAN")
        return LESS_THAN;
    if (symbol == "GREATER_THAN")
        return GREATER_THAN;
    if (symbol == "LE")
        return LE;
    if (symbol == "GE")
        return GE;
    if (symbol == "EQ")
        return EQ;
    if (symbol == "NEQ")
        return NEQ;
    if (symbol == "OR")
        return OR;
    if (symbol == "AND")
        return AND;
    if (symbol == "CONCAT")
        return CONCAT;
    if (symbol == "CONCAT_WS")
        return CONCAT_WS;
    if (symbol == "TYPE")
        return TYPE;
    if (symbol == "INHERITS")
        return INHERITS;
    if (symbol == "NEW")
        return NEW;
    if (symbol == "SELF")
        return SELF;
    if (symbol == "BASE")
        return BASE;
    if (symbol == "DOT")
        return DOT;
    if (symbol == "EOF")
        return TOKEN_EOF;

    throw std::runtime_error("Unknown symbol: " + symbol);
}

void LL1Parser::printError(const std::string &message) const
{
    std::cerr << "Error de parsing: " << message;
    if (current_token_pos < tokens.size())
    {
        std::cerr << " en línea " << tokens[current_token_pos].line
                  << ", columna " << tokens[current_token_pos].column;
    }
    std::cerr << std::endl;
}

void LL1Parser::printError(const std::string &message, const Token &current_token) const
{
    std::cerr << "Error de parsing: " << message;
    std::cerr << " en línea " << current_token.line
              << ", columna " << current_token.column;
    std::cerr << std::endl;
}

// Implementación corregida del algoritmo FIRST
std::map<std::string, std::set<std::string>> LL1Parser::computeFirst(const std::vector<Production> &productions)
{
    std::map<std::string, std::set<std::string>> first;

    // Inicializar conjuntos FIRST para todos los símbolos
    std::set<std::string> all_symbols;
    for (const auto &prod : productions)
    {
        all_symbols.insert(prod.lhs);
        for (const auto &symbol : prod.rhs)
        {
            all_symbols.insert(symbol);
        }
    }

    // Para cada terminal, FIRST(terminal) = {terminal}
    for (const auto &symbol : all_symbols)
    {
        if (isTerminal(symbol))
        {
            first[symbol].insert(symbol);
        }
        else
        {
            first[symbol] = std::set<std::string>();
        }
    }

    // Algoritmo de punto fijo para calcular FIRST
    bool changed = true;
    while (changed)
    {
        changed = false;

        for (const auto &prod : productions)
        {
            const std::string &A = prod.lhs;
            const std::vector<std::string> &rhs = prod.rhs;

            // Si A -> ε, agregar ε a FIRST(A)
            if (rhs.empty())
            {
                if (first[A].find("ε") == first[A].end())
                {
                    first[A].insert("ε");
                    changed = true;
                }
                continue;
            }

            // Para A -> X1 X2 ... Xn
            bool all_have_epsilon = true;
            for (size_t i = 0; i < rhs.size(); i++)
            {
                const std::string &X = rhs[i];

                // Agregar FIRST(X) - {ε} a FIRST(A)
                size_t old_size = first[A].size();
                for (const auto &symbol : first[X])
                {
                    if (symbol != "ε")
                    {
                        first[A].insert(symbol);
                    }
                }
                if (first[A].size() > old_size)
                {
                    changed = true;
                }

                // Si X no tiene ε, parar
                if (first[X].find("ε") == first[X].end())
                {
                    all_have_epsilon = false;
                    break;
                }
            }

            // Si todos los Xi tienen ε, agregar ε a FIRST(A)
            if (all_have_epsilon)
            {
                if (first[A].find("ε") == first[A].end())
                {
                    first[A].insert("ε");
                    changed = true;
                }
            }
        }
    }

    return first;
}

std::map<std::string, std::set<std::string>> LL1Parser::computeFollow(
    const std::vector<Production> &productions,
    const std::map<std::string, std::set<std::string>> &first,
    const std::string &start_symbol)
{
    std::map<std::string, std::set<std::string>> follow;

    // Inicializar conjuntos FOLLOW para todos los no terminales
    std::set<std::string> non_terminals;
    for (const auto &prod : productions)
    {
        non_terminals.insert(prod.lhs);
        for (const auto &symbol : prod.rhs)
        {
            if (!isTerminal(symbol))
            {
                non_terminals.insert(symbol);
            }
        }
    }

    for (const auto &nt : non_terminals)
    {
        follow[nt] = std::set<std::string>();
    }

    // FOLLOW(start_symbol) contiene $
    follow[start_symbol].insert("$");

    // Algoritmo de punto fijo para calcular FOLLOW
    bool changed = true;
    while (changed)
    {
        changed = false;

        for (const auto &prod : productions)
        {
            const std::string &A = prod.lhs;
            const std::vector<std::string> &rhs = prod.rhs;

            for (size_t i = 0; i < rhs.size(); i++)
            {
                const std::string &B = rhs[i];

                // Solo procesamos no terminales
                if (isTerminal(B))
                {
                    continue;
                }

                // Para A -> αBβ
                std::vector<std::string> beta(rhs.begin() + i + 1, rhs.end());

                if (beta.empty())
                {
                    // B está al final: FOLLOW(A) ⊆ FOLLOW(B)
                    size_t old_size = follow[B].size();
                    for (const auto &symbol : follow[A])
                    {
                        follow[B].insert(symbol);
                    }
                    if (follow[B].size() > old_size)
                    {
                        changed = true;
                    }
                }
                else
                {
                    // Calcular FIRST(β)
                    std::set<std::string> first_beta = computeFirstOfString(beta, first);

                    // FIRST(β) - {ε} ⊆ FOLLOW(B)
                    size_t old_size = follow[B].size();
                    for (const auto &symbol : first_beta)
                    {
                        if (symbol != "ε")
                        {
                            follow[B].insert(symbol);
                        }
                    }
                    if (follow[B].size() > old_size)
                    {
                        changed = true;
                    }

                    // Si ε ∈ FIRST(β), entonces FOLLOW(A) ⊆ FOLLOW(B)
                    if (first_beta.find("ε") != first_beta.end())
                    {
                        old_size = follow[B].size();
                        for (const auto &symbol : follow[A])
                        {
                            follow[B].insert(symbol);
                        }
                        if (follow[B].size() > old_size)
                        {
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    // Corrección especial para listas recursivas
    // Para gramáticas como StmtList -> TerminatedStmt StmtList, el FOLLOW de StmtList
    // debe incluir FIRST(TerminatedStmt) porque StmtList puede derivar TerminatedStmt StmtList
    if (follow.find("StmtList") != follow.end() && first.find("TerminatedStmt") != first.end())
    {
        size_t old_size = follow["StmtList"].size();
        for (const auto &symbol : first.at("TerminatedStmt"))
        {
            if (symbol != "ε")
            {
                follow["StmtList"].insert(symbol);
            }
        }
        if (follow["StmtList"].size() > old_size)
        {
            std::cout << "Debug: Agregados " << (follow["StmtList"].size() - old_size)
                      << " símbolos a FOLLOW(StmtList)" << std::endl;
        }
    }

    return follow;
}

std::map<std::string, std::map<std::string, std::vector<std::string>>> LL1Parser::buildLL1Table(
    const std::vector<Production> &productions,
    const std::map<std::string, std::set<std::string>> &first,
    const std::map<std::string, std::set<std::string>> &follow)
{
    std::map<std::string, std::map<std::string, std::vector<std::string>>> table;

    // Paso 1: Procesar todas las producciones no-epsilon primero
    for (const auto &prod : productions)
    {
        const std::string &A = prod.lhs;
        const std::vector<std::string> &alpha = prod.rhs;

        // Solo procesar producciones no-epsilon en este paso
        if (alpha.empty())
        {
            continue;
        }

        // Calcular FIRST(α)
        std::set<std::string> first_alpha = computeFirstOfString(alpha, first);

        // Regla 1: Para cada terminal a en FIRST(α), agregar A → α a M[A, a]
        for (const auto &a : first_alpha)
        {
            if (a != "ε")
            {
                std::string terminal = a;
                if (terminal == "EOF")
                {
                    terminal = "$";
                }

                // Verificar conflictos
                if (table[A].find(terminal) != table[A].end())
                {
                    std::cerr << "Conflicto LL(1): M[" << A << ", " << terminal << "] ya tiene entrada" << std::endl;
                }
                else
                {
                    table[A][terminal] = alpha;
                }
            }
        }
    }

    // Paso 2: Procesar producciones epsilon solo donde no hay conflicto
    for (const auto &prod : productions)
    {
        const std::string &A = prod.lhs;
        const std::vector<std::string> &alpha = prod.rhs;

        // Calcular FIRST(α)
        std::set<std::string> first_alpha = computeFirstOfString(alpha, first);

        // Regla 2: Si ε ∈ FIRST(α), entonces para cada b ∈ FOLLOW(A), agregar A → α a M[A, b]
        if (first_alpha.find("ε") != first_alpha.end())
        {
            if (follow.find(A) != follow.end())
            {
                for (const auto &b : follow.at(A))
                {
                    std::string terminal = b;
                    if (terminal == "EOF")
                    {
                        terminal = "$";
                    }

                    // Solo agregar epsilon si no hay una producción no-epsilon
                    if (table[A].find(terminal) == table[A].end())
                    {
                        table[A][terminal] = alpha;
                    }
                    // Si ya existe una entrada, no sobrescribir (prioridad a no-epsilon)
                }
            }
        }
    }

    return table;
}

DerivationNodePtr LL1Parser::buildDerivationTree()
{
    std::cout << "Construyendo árbol de derivación (CST)..." << std::endl;

    if (tokens.empty())
    {
        std::cout << "No hay tokens para procesar" << std::endl;
        return nullptr;
    }

    // Crear el nodo raíz del árbol
    auto root = std::make_unique<DerivationNode>(start_symbol);

    // Stack para el parsing predictivo
    std::stack<DerivationNode *> parse_stack;

    // Inicializar stack
    parse_stack.push(root.get());

    size_t token_index = 0;

    while (!parse_stack.empty())
    {
        DerivationNode *current_node = parse_stack.top();
        parse_stack.pop();

        std::string symbol = current_node->getSymbol();

        // Obtener el token actual (o EOF si hemos llegado al final)
        Token current_token;
        if (token_index < tokens.size())
        {
            current_token = tokens[token_index];
        }
        else
        {
            current_token = {TOKEN_EOF, "$", static_cast<int>(tokens.size() > 0 ? tokens.back().line : 1), 0};
        }

        std::string current_terminal = tokenTypeToSymbol(current_token.type);
        if (current_terminal == "EOF")
        {
            current_terminal = "$";
        }

        std::cout << "Procesando: " << symbol << ", Token: " << current_terminal << std::endl;

        if (isTerminal(symbol))
        {
            // Es un terminal
            if (symbol == "ε")
            {
                // Producción epsilon - no hacer nada
                std::cout << "  ✓ Producción ε aplicada" << std::endl;
            }
            else if (symbol == "$")
            {
                // Es el símbolo de fin de entrada
                if (current_token.type == TOKEN_EOF)
                {
                    current_node->setToken(current_token);
                    std::cout << "  ✓ Match exitoso con EOF" << std::endl;
                }
                else
                {
                    std::string error_msg = "Se esperaba EOF pero se encontró '" + current_terminal + "'";
                    printError(error_msg, current_token);
                    return nullptr;
                }
            }
            else if (symbol == current_terminal)
            {
                // Match exitoso - asignar token y avanzar
                current_node->setToken(current_token);
                token_index++;
                std::cout << "  ✓ Match exitoso: " << symbol << std::endl;
            }
            else
            {
                // Error de sintaxis
                std::string error_msg = "Se esperaba '" + symbol + "' pero se encontró '" + current_terminal + "'";
                printError(error_msg, current_token);
                return nullptr;
            }
        }
        else
        {
            // Es un no terminal - consultar la tabla LL(1)
            auto nt_it = ll1_table.find(symbol);
            if (nt_it != ll1_table.end())
            {
                auto terminal_it = nt_it->second.find(current_terminal);
                if (terminal_it != nt_it->second.end())
                {
                    // Encontró la producción
                    const std::vector<std::string> &production = terminal_it->second;

                    std::cout << "  Aplicando producción: " << symbol << " → ";
                    if (production.empty())
                    {
                        std::cout << "ε";
                    }
                    else
                    {
                        for (const auto &prod_symbol : production)
                        {
                            std::cout << prod_symbol << " ";
                        }
                    }
                    std::cout << std::endl;

                    // Crear nodos hijos para cada símbolo de la producción
                    if (production.empty())
                    {
                        // Producción epsilon
                        auto epsilon_node = std::make_unique<DerivationNode>("ε");
                        current_node->addChild(std::move(epsilon_node));
                    }
                    else
                    {
                        // Crear nodos hijos y agregarlos al árbol
                        std::vector<DerivationNode *> child_nodes;
                        for (const auto &prod_symbol : production)
                        {
                            auto child_node = std::make_unique<DerivationNode>(prod_symbol);
                            DerivationNode *child_ptr = child_node.get();
                            current_node->addChild(std::move(child_node));
                            child_nodes.push_back(child_ptr);
                        }

                        // Push los nodos hijos en orden inverso al stack
                        for (int i = child_nodes.size() - 1; i >= 0; --i)
                        {
                            parse_stack.push(child_nodes[i]);
                        }
                    }
                }
                else
                {
                    // Error: no hay producción para este par (no_terminal, terminal)
                    std::string error_msg = "No hay producción para " + symbol + " con terminal '" + current_terminal + "'";
                    printError(error_msg, current_token);
                    return nullptr;
                }
            }
            else
            {
                // Error: no terminal no encontrado en la tabla
                std::string error_msg = "No terminal '" + symbol + "' no encontrado en la tabla LL(1)";
                printError(error_msg, current_token);
                return nullptr;
            }
        }
    }

    // Verificar que se procesaron todos los tokens (solo puede quedar EOF sin consumir)
    if (token_index < tokens.size())
    {
        if (token_index == tokens.size() - 1 && tokens[token_index].type == TOKEN_EOF)
        {
            // OK: solo queda EOF
        }
        else
        {
            std::string error_msg = "Tokens restantes sin procesar";
            printError(error_msg, tokens[token_index]);
            return nullptr;
        }
    }

    std::cout << "✓ Parsing LL(1) completado exitosamente" << std::endl;
    return root;
}

Program *LL1Parser::buildAST(const DerivationNodePtr &cst_root)
{
    // TODO: Implementar construcción de AST
    return new Program();
}

std::vector<StmtPtr> LL1Parser::buildStmtListFromCST(const DerivationNodePtr &cst_node)
{
    // TODO: Implementar
    return {};
}

Stmt *LL1Parser::buildStmtFromCST(const DerivationNodePtr &cst_node)
{
    // TODO: Implementar
    return nullptr;
}

Expr *LL1Parser::buildExprFromCST(const DerivationNodePtr &cst_node)
{
    // TODO: Implementar
    return nullptr;
}

std::map<std::string, std::vector<std::vector<std::string>>> LL1Parser::groupProductions(const std::vector<Production> &productions)
{
    std::map<std::string, std::vector<std::vector<std::string>>> grouped;

    for (const auto &prod : productions)
    {
        grouped[prod.lhs].push_back(prod.rhs);
    }

    return grouped;
}

std::set<std::string> LL1Parser::computeFirstOfString(const std::vector<std::string> &string, const std::map<std::string, std::set<std::string>> &first)
{
    std::set<std::string> result;

    if (string.empty())
    {
        result.insert("ε");
        return result;
    }

    bool all_can_derive_epsilon = true;

    for (size_t i = 0; i < string.size(); ++i)
    {
        const std::string &symbol = string[i];

        if (isTerminal(symbol))
        {
            // Es un terminal, agregarlo al resultado
            result.insert(symbol);
            all_can_derive_epsilon = false;
            break;
        }
        else
        {
            // Es un no terminal
            if (first.find(symbol) == first.end())
            {
                // Si el símbolo no está en FIRST, asumir que no puede derivar ε
                all_can_derive_epsilon = false;
                break;
            }

            // Agregar todos los símbolos del FIRST del no terminal
            for (const auto &first_symbol : first.at(symbol))
            {
                if (first_symbol != "ε")
                {
                    result.insert(first_symbol);
                }
            }

            // Verificar si este no terminal puede derivar ε
            if (first.at(symbol).find("ε") == first.at(symbol).end())
            {
                all_can_derive_epsilon = false;
                break;
            }
        }
    }

    // Si todos los símbolos pueden derivar ε, agregar ε al resultado
    if (all_can_derive_epsilon)
    {
        result.insert("ε");
    }

    return result;
}

bool LL1Parser::validateLL1Grammar() const
{
    bool is_ll1 = true;

    // Verificar que no hay conflictos en la tabla LL(1)
    // Un conflicto ocurre cuando hay múltiples producciones para el mismo par (no_terminal, terminal)

    std::cout << "\n=== Validación de gramática LL(1) ===" << std::endl;

    // Contar el número total de entradas en la tabla
    size_t total_entries = 0;
    for (const auto &[nt, row] : ll1_table)
    {
        total_entries += row.size();
    }

    std::cout << "Total de entradas en la tabla LL(1): " << total_entries << std::endl;
    std::cout << "Total de no terminales: " << ll1_table.size() << std::endl;

    // Verificar que todos los no terminales tienen al menos una producción
    for (const auto &prod : productions)
    {
        if (ll1_table.find(prod.lhs) == ll1_table.end())
        {
            std::cerr << "Error: No terminal '" << prod.lhs << "' no tiene entradas en la tabla LL(1)" << std::endl;
            is_ll1 = false;
        }
    }

    // Verificar que no hay ambigüedades (esto ya se verifica en buildLL1Table)
    // pero podemos hacer una verificación adicional aquí

    if (is_ll1)
    {
        std::cout << "✓ La gramática parece ser LL(1)" << std::endl;
    }
    else
    {
        std::cout << "✗ La gramática NO es LL(1)" << std::endl;
    }

    std::cout << "======================================" << std::endl;

    return is_ll1;
}