#include "theoretical_lexer.hpp"
#include <iostream>
#include <cctype>
#include <algorithm>

TheoreticalLexer::TheoreticalLexer() : currentPos(0), currentLine(1), currentColumn(1)
{
    buildDFA();
}

TheoreticalLexer::TheoreticalLexer(const std::string &input)
    : input(input), currentPos(0), currentLine(1), currentColumn(1)
{
    buildDFA();
}

void TheoreticalLexer::setInput(const std::string &input)
{
    this->input = input;
    reset();
}

NFA TheoreticalLexer::buildNumberNFA()
{
    // Número: [0-9]+(\.[0-9]+)?
    NFA digits = NFA('0', '9').kleenePlus();
    NFA decimal = NFA('.').concatenate(NFA('0', '9').kleenePlus());
    NFA number = digits.concatenate(decimal.optional());
    return number;
}

NFA TheoreticalLexer::buildStringNFA()
{
    // String: "([^"]|\\")*"
    std::set<char> notQuote = {'"'};
    NFA stringContent = NFA(notQuote).kleeneStar();
    NFA string = NFA('"').concatenate(stringContent).concatenate(NFA('"'));
    return string;
}

NFA TheoreticalLexer::buildIdentifierNFA()
{
    // Identificador: [a-zA-Z_][a-zA-Z0-9_]*
    NFA firstChar = NFA('a', 'z').union_(NFA('A', 'Z')).union_(NFA('_'));
    NFA restChars = NFA('a', 'z').union_(NFA('A', 'Z')).union_(NFA('0', '9')).union_(NFA('_'));
    NFA identifier = firstChar.concatenate(restChars.kleeneStar());
    return identifier;
}

NFA TheoreticalLexer::buildKeywordNFA(const std::string &keyword)
{
    NFA result = NFA(keyword[0]);
    for (size_t i = 1; i < keyword.length(); i++)
    {
        result = result.concatenate(NFA(keyword[i]));
    }
    return result;
}

NFA TheoreticalLexer::buildOperatorNFA(const std::string &op)
{
    NFA result = NFA(op[0]);
    for (size_t i = 1; i < op.length(); i++)
    {
        result = result.concatenate(NFA(op[i]));
    }
    return result;
}

NFA TheoreticalLexer::buildPunctuationNFA(char punct)
{
    return NFA(punct);
}

NFA TheoreticalLexer::buildCombinedNFA()
{
    NFA combined;

    // Palabras clave (prioridad alta)
    std::vector<std::pair<std::string, TokenType>> keywords = {
        {"if", IF}, {"else", ELSE}, {"elif", ELIF}, {"let", LET}, {"in", IN}, {"while", WHILE}, {"for", FOR}, {"type", TYPE}, {"self", SELF}, {"new", NEW}, {"base", BASE}, {"inherits", INHERITS}, {"function", FUNCTION}, {"is", IS}, {"as", AS}, {"true", TRUE}, {"false", FALSE}};

    for (const auto &keyword : keywords)
    {
        NFA keywordNFA = buildKeywordNFA(keyword.first);
        // Asignar tipo de token al estado de aceptación
        for (int state : keywordNFA.getAcceptingStates())
        {
            keywordNFA.setTokenType(state, keyword.second);
        }

        if (combined.getNumStates() == 0)
        {
            combined = keywordNFA;
        }
        else
        {
            combined = combined.union_(keywordNFA);
        }
    }

    // Operadores de dos caracteres (prioridad alta)
    std::vector<std::pair<std::string, TokenType>> twoCharOps = {
        {"<=", LE}, {">=", GE}, {"==", EQ}, {"!=", NEQ}, {"||", OR}, {"&&", AND}, {":=", ASSIGN_DESTRUCT}, {"=>", ARROW}, {"@@", CONCAT_WS}, {"**", POW}, {"^", POW}, {"<", LESS_THAN}, {">", GREATER_THAN}};

    for (const auto &op : twoCharOps)
    {
        NFA opNFA = buildOperatorNFA(op.first);
        // Asignar tipo de token al estado de aceptación
        for (int state : opNFA.getAcceptingStates())
        {
            opNFA.setTokenType(state, op.second);
        }
        combined = combined.union_(opNFA);
    }

    // Operadores de un carácter
    std::vector<std::pair<char, TokenType>> oneCharOps = {
        {'+', PLUS}, {'-', MINUS}, {'*', MULT}, {'/', DIV}, {'%', MOD}, {'<', LESS_THAN}, {'>', GREATER_THAN}, {'=', ASSIGN}, {'@', CONCAT}, {',', COMMA}, {';', SEMICOLON}, {'.', DOT}, {':', COLON}, {'(', LPAREN}, {')', RPAREN}, {'{', LBRACE}, {'}', RBRACE}};

    for (const auto &op : oneCharOps)
    {
        NFA opNFA = buildPunctuationNFA(op.first);
        // Asignar tipo de token al estado de aceptación
        for (int state : opNFA.getAcceptingStates())
        {
            opNFA.setTokenType(state, op.second);
        }
        combined = combined.union_(opNFA);
    }

    // Números
    NFA numberNFA = buildNumberNFA();
    // Asignar tipo de token al estado de aceptación
    for (int state : numberNFA.getAcceptingStates())
    {
        numberNFA.setTokenType(state, NUMBER);
    }
    combined = combined.union_(numberNFA);

    // Strings
    NFA stringNFA = buildStringNFA();
    // Asignar tipo de token al estado de aceptación
    for (int state : stringNFA.getAcceptingStates())
    {
        stringNFA.setTokenType(state, STRING);
    }
    combined = combined.union_(stringNFA);

    // Identificadores (prioridad baja)
    NFA identifierNFA = buildIdentifierNFA();
    // Asignar tipo de token al estado de aceptación
    for (int state : identifierNFA.getAcceptingStates())
    {
        identifierNFA.setTokenType(state, IDENT);
    }
    combined = combined.union_(identifierNFA);

    return combined;
}

void TheoreticalLexer::buildDFA()
{
    NFA combinedNFA = buildCombinedNFA();

    // Usar el mapeo de tipos de token integrado en el NFA
    dfa = convertNFAtoDFA(combinedNFA, combinedNFA.getTokenTypes());
}

void TheoreticalLexer::skipWhitespace()
{
    while (currentPos < input.length() && std::isspace(input[currentPos]))
    {
        if (input[currentPos] == '\n')
        {
            currentLine++;
            currentColumn = 1;
        }
        else
        {
            currentColumn++;
        }
        currentPos++;
    }
}

void TheoreticalLexer::skipComments()
{
    if (currentPos < input.length() && input[currentPos] == '#')
    {
        while (currentPos < input.length() && input[currentPos] != '\n')
        {
            currentPos++;
        }
        if (currentPos < input.length() && input[currentPos] == '\n')
        {
            currentLine++;
            currentColumn = 1;
            currentPos++;
        }
    }
}

Token TheoreticalLexer::getNextToken()
{
    // Saltar espacios en blanco y comentarios
    do
    {
        skipWhitespace();
        skipComments();
    } while (currentPos < input.length() && std::isspace(input[currentPos]));

    if (currentPos >= input.length())
    {
        return Token(TOKEN_EOF, "", currentLine, currentColumn);
    }

    // Obtener el lexema más largo
    std::string remainingInput = input.substr(currentPos);

    // Simular el DFA paso a paso
    int currentState = dfa.getStartState();

    int lastAcceptingState = -1;
    int lastAcceptingPos = -1;

    for (size_t i = 0; i < remainingInput.length(); i++)
    {
        char c = remainingInput[i];

        // Buscar la transición en el DFA
        auto stateIt = dfa.getTransitions().find(currentState);
        if (stateIt == dfa.getTransitions().end())
        {
            break;
        }

        auto symbolIt = stateIt->second.find(c);
        if (symbolIt == stateIt->second.end())
        {
            break;
        }

        int nextState = symbolIt->second;

        if (dfa.isAccepting(nextState))
        {
            lastAcceptingState = nextState;
            lastAcceptingPos = i;
        }

        currentState = nextState;
    }

    if (lastAcceptingPos == -1)
    {
        // No se encontró ningún token válido
        char invalidChar = remainingInput[0];
        std::string lexeme(1, invalidChar);
        Token token(ERROR, lexeme, currentLine, currentColumn);
        currentPos++;
        currentColumn++;
        return token;
    }

    // Crear el token con el lexema más largo
    std::string lexeme = remainingInput.substr(0, lastAcceptingPos + 1);
    TokenType tokenType = getTokenType(lastAcceptingState, lexeme);

    Token token(static_cast<TokenType>(tokenType), lexeme, currentLine, currentColumn);

    // Actualizar la posición
    currentPos += lexeme.length();
    for (char c : lexeme)
    {
        if (c == '\n')
        {
            currentLine++;
            currentColumn = 1;
        }
        else
        {
            currentColumn++;
        }
    }

    return token;
}

std::vector<Token> TheoreticalLexer::tokenize()
{
    std::vector<Token> tokens;
    reset();

    while (hasMoreTokens())
    {
        Token token = getNextToken();
        tokens.push_back(token);
        if (token.type == TOKEN_EOF)
        {
            return tokens;
        }
    }
    // Si no se agregó EOF, agrégalo manualmente
    if (tokens.empty() || tokens.back().type != TOKEN_EOF)
    {
        tokens.push_back(Token(TOKEN_EOF, "", currentLine, currentColumn));
    }
    return tokens;
}

bool TheoreticalLexer::hasMoreTokens() const
{
    return currentPos < input.length();
}

void TheoreticalLexer::reset()
{
    currentPos = 0;
    currentLine = 1;
    currentColumn = 1;
}

void TheoreticalLexer::printDFA() const
{
    dfa.print();
}

TokenType TheoreticalLexer::getTokenType(int state)
{
    int tokenType = dfa.getTokenType(state);
    if (tokenType == -1)
    {
        return ERROR;
    }
    return static_cast<TokenType>(tokenType);
}

TokenType TheoreticalLexer::getTokenType(int state, const std::string &lexeme)
{
    // Primero verificar si es una palabra clave
    std::vector<std::pair<std::string, TokenType>> keywords = {
        {"if", IF}, {"else", ELSE}, {"elif", ELIF}, {"let", LET}, {"in", IN}, {"while", WHILE}, {"for", FOR}, {"type", TYPE}, {"self", SELF}, {"new", NEW}, {"base", BASE}, {"inherits", INHERITS}, {"function", FUNCTION}, {"is", IS}, {"as", AS}, {"true", TRUE}, {"false", FALSE}};

    for (const auto &keyword : keywords)
    {
        if (lexeme == keyword.first)
        {
            return keyword.second;
        }
    }

    // Si no es una palabra clave, usar el tipo del DFA
    int tokenType = dfa.getTokenType(state);
    if (tokenType == -1)
    {
        return ERROR;
    }
    return static_cast<TokenType>(tokenType);
}