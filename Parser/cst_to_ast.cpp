#include "cst_to_ast.hpp"
#include <iostream>
#include <stdexcept>

std::unique_ptr<Program> CSTToASTConverter::convertToAST(DerivationNode *cst)
{
    if (!cst || cst->getSymbol() != "Program")
    {
        throw std::runtime_error("CST root must be a Program node");
    }

    auto program = std::make_unique<Program>();

    // Program → StmtList
    if (!cst->children.empty())
    {
        std::cout << "DEBUG: Program tiene " << cst->children.size() << " hijos" << std::endl;
        std::cout << "DEBUG: Primer hijo es " << cst->children[0]->getSymbol() << std::endl;
        convertStmtList(cst->children[0].get(), program->stmts);
    }

    std::cout << "DEBUG: Program tiene " << program->stmts.size() << " statements" << std::endl;
    return program;
}

void CSTToASTConverter::convertStmtList(DerivationNode *node, std::vector<std::unique_ptr<Stmt>> &stmts)
{
    if (!node || node->getSymbol() != "StmtList")
    {
        std::cout << "DEBUG: StmtList - nodo inválido o no es StmtList" << std::endl;
        return;
    }

    // StmtList → TerminatedStmt StmtList | ε
    if (node->children.empty())
    {
        std::cout << "DEBUG: StmtList - ε production" << std::endl;
        return; // ε production
    }

    std::cout << "DEBUG: StmtList tiene " << node->children.size() << " hijos" << std::endl;

    // First child: TerminatedStmt
    if (node->children.size() >= 1)
    {
        auto terminatedStmt = node->children[0].get();
        if (terminatedStmt && terminatedStmt->getSymbol() == "TerminatedStmt")
        {
            std::cout << "DEBUG: Procesando TerminatedStmt" << std::endl;
            // TerminatedStmt → Stmt SEMICOLON
            if (!terminatedStmt->children.empty())
            {
                auto stmt = terminatedStmt->children[0].get();
                if (stmt)
                {
                    std::cout << "DEBUG: Convirtiendo Stmt de tipo " << stmt->getSymbol() << std::endl;
                    auto astStmt = convertStmt(stmt);
                    if (astStmt)
                    {
                        std::cout << "DEBUG: Statement convertido exitosamente" << std::endl;
                        stmts.push_back(std::move(astStmt));
                    }
                    else
                    {
                        std::cout << "DEBUG: Error al convertir statement" << std::endl;
                    }
                }
            }
        }
    }

    // Second child: StmtList (recursive)
    if (node->children.size() >= 2)
    {
        std::cout << "DEBUG: Procesando siguiente StmtList" << std::endl;
        convertStmtList(node->children[1].get(), stmts);
    }
}

std::unique_ptr<Stmt> CSTToASTConverter::convertStmt(DerivationNode *node)
{
    if (!node || node->getSymbol() != "Stmt")
    {
        std::cout << "DEBUG: convertStmt - nodo inválido o no es Stmt" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertStmt - procesando Stmt con " << node->children.size() << " hijos" << std::endl;

    // Stmt → Expr | WhileStmt | ForStmt | BlockStmt | FunctionDef | TypeDef
    if (!node->children.empty())
    {
        auto child = node->children[0].get();
        if (!child)
        {
            std::cout << "DEBUG: convertStmt - primer hijo es null" << std::endl;
            return nullptr;
        }

        std::string childSymbol = child->getSymbol();
        std::cout << "DEBUG: convertStmt - primer hijo es " << childSymbol << std::endl;

        std::unique_ptr<Stmt> result = nullptr;

        if (childSymbol == "Expr")
        {
            std::cout << "DEBUG: convertStmt - convirtiendo Expr" << std::endl;
            result = std::make_unique<ExprStmt>(convertExpr(child));
        }
        else if (childSymbol == "FunctionDef")
        {
            result = convertFunctionDef(child);
        }
        else if (childSymbol == "TypeDef")
        {
            result = convertTypeDef(child);
        }
        else if (childSymbol == "WhileStmt")
        {
            result = std::make_unique<ExprStmt>(convertWhileExpr(child));
        }
        else if (childSymbol == "ForStmt")
        {
            result = std::make_unique<ExprStmt>(convertForExpr(child));
        }
        else if (childSymbol == "BlockStmt")
        {
            result = std::make_unique<ExprStmt>(convertBlockStmt(child));
        }

        // Set line and column information
        if (result)
        {
            setLineColumn(result.get(), node);
        }

        return result;
    }

    std::cout << "DEBUG: convertStmt - no se pudo convertir el statement" << std::endl;
    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertExpr(DerivationNode *node)
{
    if (!node)
    {
        std::cout << "DEBUG: convertExpr - nodo nulo" << std::endl;
        return nullptr;
    }

    std::string symbol = node->getSymbol();
    std::cout << "DEBUG: convertExpr - procesando nodo de tipo " << symbol << std::endl;

    if (symbol == "Expr")
    {
        // Expr → OrExpr | IfExpr | LetExpr
        if (!node->children.empty())
        {
            auto child = node->children[0].get();
            if (!child)
            {
                std::cout << "DEBUG: convertExpr - hijo nulo" << std::endl;
                return nullptr;
            }

            std::string childSymbol = child->getSymbol();
            std::cout << "DEBUG: convertExpr - procesando hijo de tipo " << childSymbol << std::endl;

            std::unique_ptr<Expr> result = nullptr;

            if (childSymbol == "OrExpr")
            {
                result = convertOrExpr(child);
                if (result)
                {
                    std::cout << "DEBUG: convertExpr - OrExpr convertido exitosamente" << std::endl;
                }
                else
                {
                    std::cout << "DEBUG: convertExpr - error al convertir OrExpr" << std::endl;
                }
            }
            else if (childSymbol == "IfExpr")
            {
                result = convertIfExpr(child);
            }
            else if (childSymbol == "LetExpr")
            {
                std::cout << "DEBUG: convertExpr - convirtiendo LetExpr" << std::endl;
                result = convertLetExpr(child);
            }

            // Set line and column information
            if (result)
            {
                setLineColumn(result.get(), node);
            }

            return result;
        }
        else
        {
            std::cout << "DEBUG: convertExpr - nodo sin hijos" << std::endl;
        }
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertOrExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "OrExpr")
    {
        return nullptr;
    }

    // OrExpr → AndExpr OrExpr'
    if (node->children.size() >= 2)
    {
        auto andExpr = convertAndExpr(node->children[0].get());
        return convertOrExprPrime(std::move(andExpr), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertOrExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // OrExpr' → OR AndExpr OrExpr'
    if (node->children.size() >= 3)
    {
        auto rightAndExpr = convertAndExpr(node->children[1].get());
        auto temp = std::make_unique<BinaryExpr>(BinaryExpr::OP_OR, std::move(inherited), std::move(rightAndExpr));
        return convertOrExprPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertAndExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "AndExpr")
    {
        return nullptr;
    }

    // AndExpr → CmpExpr AndExpr'
    if (node->children.size() >= 2)
    {
        auto cmpExpr = convertCmpExpr(node->children[0].get());
        return convertAndExprPrime(std::move(cmpExpr), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertAndExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // AndExpr' → AND CmpExpr AndExpr'
    if (node->children.size() >= 3)
    {
        auto rightCmpExpr = convertCmpExpr(node->children[1].get());
        auto temp = std::make_unique<BinaryExpr>(BinaryExpr::OP_AND, std::move(inherited), std::move(rightCmpExpr));
        return convertAndExprPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertCmpExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "CmpExpr")
    {
        return nullptr;
    }

    // CmpExpr → ConcatExpr CmpExpr'
    if (node->children.size() >= 2)
    {
        auto concatExpr = convertConcatExpr(node->children[0].get());
        return convertCmpExprPrime(std::move(concatExpr), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertCmpExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // CmpExpr' → LESS_THAN ConcatExpr CmpExpr' | GREATER_THAN ConcatExpr CmpExpr' | ...
    //           | IS IDENT CmpExpr'
    if (node->children.size() >= 3)
    {
        auto opNode = node->children[0].get();

        if (opNode && opNode->getSymbol() == "IS")
        {
            // IS IDENT CmpExpr'
            auto typeIdent = node->children[1].get();
            if (!typeIdent)
            {
                std::cout << "DEBUG: convertCmpExprPrime - tipo nulo en IS" << std::endl;
                return inherited;
            }

            std::string typeName = getTokenValue(typeIdent);
            std::cout << "DEBUG: convertCmpExprPrime - IS " << typeName << std::endl;

            auto isExpr = std::make_unique<IsExpr>(std::move(inherited), typeName);
            return convertCmpExprPrime(std::move(isExpr), node->children[2].get());
        }

        auto rightConcatExpr = convertConcatExpr(node->children[1].get());

        BinaryExpr::Op op = convertBinaryOp(opNode->getSymbol());
        auto temp = std::make_unique<BinaryExpr>(op, std::move(inherited), std::move(rightConcatExpr));
        return convertCmpExprPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertConcatExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "ConcatExpr")
    {
        return nullptr;
    }

    // ConcatExpr → AddExpr ConcatExpr'
    if (node->children.size() >= 2)
    {
        auto addExpr = convertAddExpr(node->children[0].get());
        return convertConcatExprPrime(std::move(addExpr), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertConcatExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // ConcatExpr' → CONCAT AddExpr ConcatExpr' | CONCAT_WS AddExpr ConcatExpr'
    if (node->children.size() >= 3)
    {
        auto opNode = node->children[0].get();
        auto rightAddExpr = convertAddExpr(node->children[1].get());

        BinaryExpr::Op op = convertBinaryOp(opNode->getSymbol());
        auto temp = std::make_unique<BinaryExpr>(op, std::move(inherited), std::move(rightAddExpr));
        return convertConcatExprPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertAddExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "AddExpr")
    {
        return nullptr;
    }

    // AddExpr → Term AddExpr'
    if (node->children.size() >= 2)
    {
        auto term = convertTerm(node->children[0].get());
        return convertAddExprPrime(std::move(term), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertAddExprPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // AddExpr' → PLUS Term AddExpr' | MINUS Term AddExpr'
    if (node->children.size() >= 3)
    {
        auto opNode = node->children[0].get();
        auto rightTerm = convertTerm(node->children[1].get());

        BinaryExpr::Op op = convertBinaryOp(opNode->getSymbol());
        auto temp = std::make_unique<BinaryExpr>(op, std::move(inherited), std::move(rightTerm));
        return convertAddExprPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertTerm(DerivationNode *node)
{
    if (!node || node->getSymbol() != "Term")
    {
        return nullptr;
    }

    // Term → Factor Term'
    if (node->children.size() >= 2)
    {
        auto factor = convertFactor(node->children[0].get());
        return convertTermPrime(std::move(factor), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertTermPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // Term' → MULT Factor Term' | DIV Factor Term' | MOD Factor Term'
    if (node->children.size() >= 3)
    {
        auto opNode = node->children[0].get();
        auto rightFactor = convertFactor(node->children[1].get());

        BinaryExpr::Op op = convertBinaryOp(opNode->getSymbol());
        auto temp = std::make_unique<BinaryExpr>(op, std::move(inherited), std::move(rightFactor));
        setLineColumn(temp.get(), opNode);
        return convertTermPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertFactor(DerivationNode *node)
{
    if (!node || node->getSymbol() != "Factor")
    {
        return nullptr;
    }

    // Factor → Power Factor'
    if (node->children.size() >= 2)
    {
        auto power = convertPower(node->children[0].get());
        return convertFactorPrime(std::move(power), node->children[1].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertFactorPrime(std::unique_ptr<Expr> inherited, DerivationNode *node)
{
    if (!node || node->children.empty())
    {
        return inherited; // ε production
    }

    // Factor' → POW Power Factor'
    if (node->children.size() >= 3)
    {
        auto rightPower = convertPower(node->children[1].get());
        auto temp = std::make_unique<BinaryExpr>(BinaryExpr::OP_POW, std::move(inherited), std::move(rightPower));
        setLineColumn(temp.get(), node->children[0].get());
        return convertFactorPrime(std::move(temp), node->children[2].get());
    }

    return inherited;
}

std::unique_ptr<Expr> CSTToASTConverter::convertPower(DerivationNode *node)
{
    if (!node || node->getSymbol() != "Power")
    {
        return nullptr;
    }

    // Power → Unary
    if (!node->children.empty())
    {
        return convertUnary(node->children[0].get());
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertUnary(DerivationNode *node)
{
    if (!node || node->getSymbol() != "Unary")
    {
        return nullptr;
    }

    // Unary → MINUS Unary | Primary AsExpr
    if (node->children.size() >= 2)
    {
        auto firstChild = node->children[0].get();
        if (firstChild && firstChild->getSymbol() == "MINUS")
        {
            // MINUS Unary
            auto innerExpr = convertUnary(node->children[1].get());
            return std::make_unique<UnaryExpr>(UnaryExpr::OP_NEG, std::move(innerExpr));
        }
        else
        {
            // Primary AsExpr
            auto primary = convertPrimary(firstChild);
            if (!primary)
            {
                return nullptr;
            }

            // Manejar AsExpr si existe
            if (node->children.size() >= 2)
            {
                auto asExprNode = node->children[1].get();
                if (asExprNode)
                {
                    auto asExpr = convertAsExpr(asExprNode);
                    if (asExpr)
                    {
                        // Asignar la expresión base al AsExpr
                        asExpr->expr = std::move(primary);
                        return asExpr;
                    }
                }
            }

            return primary;
        }
    }

    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertPrimary(DerivationNode *node)
{
    if (!node || node->getSymbol() != "Primary")
    {
        return nullptr;
    }

    if (node->children.empty())
    {
        return nullptr;
    }

    auto firstChild = node->children[0].get();
    if (!firstChild)
    {
        return nullptr;
    }

    std::string childSymbol = firstChild->getSymbol();

    // Primary → NUMBER | STRING | TRUE | FALSE | IDENT PrimaryTail | ...
    if (childSymbol == "NUMBER")
    {
        std::string value = getTokenValue(firstChild);
        auto result = std::make_unique<NumberExpr>(std::stod(value));
        setLineColumn(result.get(), firstChild);
        return result;
    }

    if (childSymbol == "STRING")
    {
        std::string value = getTokenValue(firstChild);
        auto result = std::make_unique<StringExpr>(value);
        setLineColumn(result.get(), firstChild);
        return result;
    }

    if (childSymbol == "TRUE")
    {
        auto result = std::make_unique<BooleanExpr>(true);
        setLineColumn(result.get(), firstChild);
        return result;
    }

    if (childSymbol == "FALSE")
    {
        auto result = std::make_unique<BooleanExpr>(false);
        setLineColumn(result.get(), firstChild);
        return result;
    }

    if (childSymbol == "IDENT")
    {
        std::string name = getTokenValue(firstChild);

        // Manejar PrimaryTail si existe
        if (node->children.size() >= 2)
        {
            auto primaryTail = node->children[1].get();
            if (primaryTail)
            {
                auto result = convertPrimaryTail(std::make_unique<VariableExpr>(name), primaryTail);
                if (result)
                {
                    setLineColumn(result.get(), firstChild);
                }
                return result;
            }
        }

        auto result = std::make_unique<VariableExpr>(name);
        setLineColumn(result.get(), firstChild);
        return result;
    }

    if (childSymbol == "SELF")
    {
        // Manejar PrimaryTail si existe
        if (node->children.size() >= 2)
        {
            auto primaryTail = node->children[1].get();
            if (primaryTail)
            {
                auto result = convertPrimaryTail(std::make_unique<SelfExpr>(), primaryTail);
                if (result)
                {
                    setLineColumn(result.get(), firstChild);
                }
                return result;
            }
        }

        return std::make_unique<SelfExpr>();
    }

    if (childSymbol == "BASE")
    {
        // BASE LPAREN ArgList RPAREN
        if (node->children.size() >= 4)
        {
            auto argList = node->children[2].get();

            std::vector<std::unique_ptr<Expr>> args;
            if (argList)
            {
                args = convertArgList(argList);
            }

            std::cout << "DEBUG: convertPrimary - BASE con " << args.size() << " argumentos" << std::endl;
            return std::make_unique<BaseCallExpr>(std::move(args));
        }
    }

    if (childSymbol == "NEW")
    {
        // NEW IDENT LPAREN ArgList RPAREN
        if (node->children.size() >= 5)
        {
            auto typeName = node->children[1].get();
            auto argList = node->children[3].get();

            if (!typeName)
            {
                std::cout << "DEBUG: convertPrimary - nombre de tipo nulo en NEW" << std::endl;
                return nullptr;
            }

            std::string name = getTokenValue(typeName);
            std::cout << "DEBUG: convertPrimary - instanciando tipo: " << name << std::endl;

            std::vector<std::unique_ptr<Expr>> args;
            if (argList)
            {
                args = convertArgList(argList);
            }

            return std::make_unique<NewExpr>(name, std::move(args));
        }
    }

    if (childSymbol == "LPAREN")
    {
        // LPAREN Expr RPAREN
        if (node->children.size() >= 3)
        {
            return convertExpr(node->children[1].get());
        }
    }

    return nullptr;
}

// Stub implementations for functions not yet implemented
std::unique_ptr<FunctionDecl> CSTToASTConverter::convertFunctionDef(DerivationNode *node)
{
    if (!node || node->getSymbol() != "FunctionDef")
    {
        std::cout << "DEBUG: convertFunctionDef - nodo inválido o no es FunctionDef" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertFunctionDef - procesando FunctionDef con " << node->children.size() << " hijos" << std::endl;

    // FunctionDef → FUNCTION IDENT LPAREN ArgIdList RPAREN TypeAnnotation FunctionBody
    if (node->children.size() >= 7)
    {
        // children[0] = FUNCTION (terminal)
        // children[1] = IDENT (function name)
        // children[2] = LPAREN (terminal)
        // children[3] = ArgIdList
        // children[4] = RPAREN (terminal)
        // children[5] = TypeAnnotation
        // children[6] = FunctionBody

        auto functionName = node->children[1].get();
        auto argIdList = node->children[3].get();
        auto typeAnnotation = node->children[5].get();
        auto functionBody = node->children[6].get();

        if (!functionName || !functionBody)
        {
            std::cout << "DEBUG: convertFunctionDef - hijos nulos" << std::endl;
            return nullptr;
        }

        std::string name = getTokenValue(functionName);
        std::cout << "DEBUG: convertFunctionDef - nombre de función: " << name << std::endl;

        // Convertir argumentos
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> paramTypes;
        if (argIdList)
        {
            std::cout << "DEBUG: convertFunctionDef - convirtiendo argumentos" << std::endl;
            auto argInfo = convertArgIdListWithTypes(argIdList);
            params = std::move(argInfo.first);
            paramTypes = std::move(argInfo.second);
        }

        // Convertir tipo de retorno
        std::shared_ptr<TypeInfo> returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
        if (typeAnnotation)
        {
            returnType = convertTypeAnnotation(typeAnnotation);
        }

        // Convertir cuerpo de función
        std::cout << "DEBUG: convertFunctionDef - convirtiendo cuerpo" << std::endl;
        auto body = convertFunctionBody(functionBody);
        if (!body)
        {
            std::cout << "DEBUG: convertFunctionDef - error al convertir cuerpo" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertFunctionDef - creando FunctionDecl" << std::endl;
        auto result = std::make_unique<FunctionDecl>(
            name,
            std::move(params),
            std::move(body),
            std::move(paramTypes),
            std::move(returnType));
        setLineColumn(result.get(), node->children[0].get());
        return result;
    }

    std::cout << "DEBUG: convertFunctionDef - estructura inválida" << std::endl;
    return nullptr;
}

std::unique_ptr<TypeDecl> CSTToASTConverter::convertTypeDef(DerivationNode *node)
{
    if (!node || node->getSymbol() != "TypeDef")
    {
        std::cout << "DEBUG: convertTypeDef - nodo inválido o no es TypeDef" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertTypeDef - procesando TypeDef con " << node->children.size() << " hijos" << std::endl;

    // TypeDef → TYPE IDENT TypeParams TypeInheritance LBRACE TypeBody RBRACE
    if (node->children.size() >= 7)
    {
        // children[0] = TYPE (terminal)
        // children[1] = IDENT (type name)
        // children[2] = TypeParams
        // children[3] = TypeInheritance
        // children[4] = LBRACE (terminal)
        // children[5] = TypeBody
        // children[6] = RBRACE (terminal)

        auto typeName = node->children[1].get();
        auto typeParams = node->children[2].get();
        auto typeInheritance = node->children[3].get();
        auto typeBody = node->children[5].get();

        if (!typeName)
        {
            std::cout << "DEBUG: convertTypeDef - nombre de tipo nulo" << std::endl;
            return nullptr;
        }

        std::string name = getTokenValue(typeName);
        std::cout << "DEBUG: convertTypeDef - nombre de tipo: " << name << std::endl;

        // Convertir parámetros del tipo
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> paramTypes;
        if (typeParams)
        {
            std::cout << "DEBUG: convertTypeDef - convirtiendo parámetros" << std::endl;
            auto paramInfo = convertTypeParams(typeParams);
            params = std::move(paramInfo.first);
            paramTypes = std::move(paramInfo.second);
        }

        // Convertir herencia
        std::string baseType = "Object";
        std::vector<std::unique_ptr<Expr>> baseArgs;
        if (typeInheritance)
        {
            std::cout << "DEBUG: convertTypeDef - convirtiendo herencia" << std::endl;
            auto inheritanceInfo = convertTypeInheritance(typeInheritance);
            baseType = std::move(inheritanceInfo.first);
            baseArgs = std::move(inheritanceInfo.second);
        }

        // Convertir cuerpo del tipo (atributos y métodos)
        std::vector<std::unique_ptr<AttributeDecl>> attributes;
        std::vector<std::unique_ptr<MethodDecl>> methods;
        if (typeBody)
        {
            std::cout << "DEBUG: convertTypeDef - convirtiendo cuerpo" << std::endl;
            auto bodyInfo = convertTypeBody(typeBody);
            attributes = std::move(bodyInfo.first);
            methods = std::move(bodyInfo.second);
        }

        std::cout << "DEBUG: convertTypeDef - creando TypeDecl" << std::endl;
        auto result = std::make_unique<TypeDecl>(
            name,
            std::move(params),
            std::move(paramTypes),
            std::move(attributes),
            std::move(methods),
            std::move(baseType),
            std::move(baseArgs));
        setLineColumn(result.get(), node->children[0].get());
        return result;
    }

    std::cout << "DEBUG: convertTypeDef - estructura inválida" << std::endl;
    return nullptr;
}

std::unique_ptr<Expr> CSTToASTConverter::convertBlockStmt(DerivationNode *node)
{
    if (!node || node->getSymbol() != "BlockStmt")
    {
        std::cout << "DEBUG: convertBlockStmt - nodo inválido o no es BlockStmt" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertBlockStmt - procesando BlockStmt con " << node->children.size() << " hijos" << std::endl;

    // BlockStmt → LBRACE StmtList RBRACE
    if (node->children.size() >= 3)
    {
        // children[0] = LBRACE (terminal)
        // children[1] = StmtList
        // children[2] = RBRACE (terminal)

        auto stmtList = node->children[1].get();

        if (!stmtList)
        {
            std::cout << "DEBUG: convertBlockStmt - StmtList nulo" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertBlockStmt - convirtiendo StmtList" << std::endl;
        std::vector<std::unique_ptr<Stmt>> stmts;
        convertStmtList(stmtList, stmts);

        if (stmts.empty())
        {
            std::cout << "DEBUG: convertBlockStmt - no se encontraron statements" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertBlockStmt - creando BlockExpr con " << stmts.size() << " statements" << std::endl;
        return std::make_unique<ExprBlock>(std::move(stmts));
    }

    std::cout << "DEBUG: convertBlockStmt - estructura inválida" << std::endl;
    return nullptr;
}

std::unique_ptr<WhileExpr> CSTToASTConverter::convertWhileExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "WhileStmt")
    {
        std::cout << "DEBUG: convertWhileExpr - nodo inválido o no es WhileStmt" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertWhileExpr - procesando WhileStmt con " << node->children.size() << " hijos" << std::endl;

    // WhileStmt → WHILE Expr WhileBody
    if (node->children.size() >= 3)
    {
        // children[0] = WHILE (terminal)
        // children[1] = Expr (condition)
        // children[2] = WhileBody

        auto condition = node->children[1].get();
        auto whileBody = node->children[2].get();

        if (!condition || !whileBody)
        {
            std::cout << "DEBUG: convertWhileExpr - hijos nulos" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertWhileExpr - convirtiendo condición" << std::endl;
        auto condExpr = convertExpr(condition);
        if (!condExpr)
        {
            std::cout << "DEBUG: convertWhileExpr - error al convertir condición" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertWhileExpr - convirtiendo while body" << std::endl;
        auto bodyStmt = convertWhileBody(whileBody);
        if (!bodyStmt)
        {
            std::cout << "DEBUG: convertWhileExpr - error al convertir while body" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertWhileExpr - creando WhileExpr" << std::endl;
        // Convertir Stmt a Expr (ExprStmt contiene un Expr)
        auto exprStmt = static_cast<ExprStmt *>(bodyStmt.get());
        auto bodyExpr = std::move(exprStmt->expr);
        return std::make_unique<WhileExpr>(
            std::move(condExpr),
            std::move(bodyExpr));
    }

    std::cout << "DEBUG: convertWhileExpr - estructura inválida" << std::endl;
    return nullptr;
}

std::unique_ptr<ForExpr> CSTToASTConverter::convertForExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "ForStmt")
    {
        std::cout << "DEBUG: convertForExpr - nodo inválido o no es ForStmt" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertForExpr - procesando ForStmt con " << node->children.size() << " hijos" << std::endl;

    // ForStmt → FOR LPAREN IDENT IN Expr RPAREN ForBody
    if (node->children.size() >= 7)
    {
        // children[0] = FOR (terminal)
        // children[1] = LPAREN (terminal)
        // children[2] = IDENT (iterator variable)
        // children[3] = IN (terminal)
        // children[4] = Expr (collection)
        // children[5] = RPAREN (terminal)
        // children[6] = ForBody

        auto iteratorVar = node->children[2].get();
        auto collection = node->children[4].get();
        auto forBody = node->children[6].get();

        if (!iteratorVar || !collection || !forBody)
        {
            std::cout << "DEBUG: convertForExpr - hijos nulos" << std::endl;
            return nullptr;
        }

        std::string iteratorName = getTokenValue(iteratorVar);
        std::cout << "DEBUG: convertForExpr - variable iteradora: " << iteratorName << std::endl;

        std::cout << "DEBUG: convertForExpr - convirtiendo colección" << std::endl;
        auto collectionExpr = convertExpr(collection);
        if (!collectionExpr)
        {
            std::cout << "DEBUG: convertForExpr - error al convertir colección" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertForExpr - convirtiendo for body" << std::endl;
        auto bodyStmt = convertForBody(forBody);
        if (!bodyStmt)
        {
            std::cout << "DEBUG: convertForExpr - error al convertir for body" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertForExpr - creando ForExpr" << std::endl;
        // Convertir Stmt a Expr (ExprStmt contiene un Expr)
        auto exprStmt = static_cast<ExprStmt *>(bodyStmt.get());
        auto bodyExpr = std::move(exprStmt->expr);
        return std::make_unique<ForExpr>(
            iteratorName,
            std::move(collectionExpr),
            std::move(bodyExpr));
    }

    std::cout << "DEBUG: convertForExpr - estructura inválida" << std::endl;
    return nullptr;
}

std::unique_ptr<IfExpr> CSTToASTConverter::convertIfExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "IfExpr")
    {
        std::cout << "DEBUG: convertIfExpr - nodo inválido o no es IfExpr" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertIfExpr - procesando IfExpr con " << node->children.size() << " hijos" << std::endl;

    // IfExpr → IF LPAREN Expr RPAREN IfBody ElifList ELSE IfBody
    if (node->children.size() >= 8)
    {
        // children[0] = IF (terminal)
        // children[1] = LPAREN (terminal)
        // children[2] = Expr (condition)
        // children[3] = RPAREN (terminal)
        // children[4] = IfBody (then branch)
        // children[5] = ElifList
        // children[6] = ELSE (terminal)
        // children[7] = IfBody (else branch)

        auto condition = node->children[2].get();
        auto thenBody = node->children[4].get();
        auto elifList = node->children[5].get();
        auto elseBody = node->children[7].get();

        if (!condition || !thenBody || !elseBody)
        {
            std::cout << "DEBUG: convertIfExpr - hijos nulos" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertIfExpr - convirtiendo condición" << std::endl;
        auto condExpr = convertExpr(condition);
        if (!condExpr)
        {
            std::cout << "DEBUG: convertIfExpr - error al convertir condición" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertIfExpr - convirtiendo then body" << std::endl;
        auto thenStmt = convertIfBody(thenBody);
        if (!thenStmt)
        {
            std::cout << "DEBUG: convertIfExpr - error al convertir then body" << std::endl;
            return nullptr;
        }

        // Convertir Stmt a Expr para then branch
        auto thenExprStmt = static_cast<ExprStmt *>(thenStmt.get());
        auto thenExpr = std::move(thenExprStmt->expr);

        // Procesar ElifList para construir la cadena de if-elif-else
        std::cout << "DEBUG: convertIfExpr - procesando ElifList" << std::endl;
        auto elifBranches = convertElifList(elifList);

        // Convertir Stmt a Expr para else branch final
        auto elseStmt = convertIfBody(elseBody);
        if (!elseStmt)
        {
            std::cout << "DEBUG: convertIfExpr - error al convertir else body" << std::endl;
            return nullptr;
        }
        auto elseExprStmt = static_cast<ExprStmt *>(elseStmt.get());
        auto elseExpr = std::move(elseExprStmt->expr);

        // Construir la cadena de IfExpr anidados desde el último elif hacia el primer if
        std::unique_ptr<Expr> currentElse = std::move(elseExpr);

        // Procesar elif branches en orden inverso (desde el último hacia el primero)
        for (int i = elifBranches.size() - 1; i >= 0; i--)
        {
            std::cout << "DEBUG: convertIfExpr - creando IfExpr para elif branch " << i << std::endl;
            currentElse = std::make_unique<IfExpr>(
                std::move(elifBranches[i]->condition),
                std::move(elifBranches[i]->thenBranch),
                std::move(currentElse));
        }

        std::cout << "DEBUG: convertIfExpr - creando IfExpr principal" << std::endl;
        return std::make_unique<IfExpr>(
            std::move(condExpr),
            std::move(thenExpr),
            std::move(currentElse));
    }

    std::cout << "DEBUG: convertIfExpr - estructura inválida" << std::endl;
    return nullptr;
}

std::unique_ptr<LetExpr> CSTToASTConverter::convertLetExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "LetExpr")
    {
        std::cout << "DEBUG: convertLetExpr - nodo inválido o no es LetExpr" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertLetExpr - procesando LetExpr con " << node->children.size() << " hijos" << std::endl;

    // LetExpr → LET VarBindingList IN LetBody
    if (node->children.size() >= 4)
    {
        // children[0] = LET (terminal)
        // children[1] = VarBindingList
        // children[2] = IN (terminal)
        // children[3] = LetBody

        auto varBindingList = node->children[1].get();
        auto letBody = node->children[3].get();

        if (!varBindingList || !letBody)
        {
            std::cout << "DEBUG: convertLetExpr - hijos nulos" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertLetExpr - procesando VarBindingList" << std::endl;
        auto bindings = convertVarBindingList(varBindingList);

        if (bindings.empty())
        {
            std::cout << "DEBUG: convertLetExpr - no se encontraron bindings" << std::endl;
            return nullptr;
        }

        // Convertir múltiples variables en LetExpr anidados
        std::cout << "DEBUG: convertLetExpr - convirtiendo LetBody" << std::endl;
        auto body = convertLetBody(letBody);

        if (!body)
        {
            std::cout << "DEBUG: convertLetExpr - error al convertir LetBody" << std::endl;
            return nullptr;
        }

        // Crear LetExpr anidados desde la última variable hacia la primera
        std::unique_ptr<Stmt> currentBody = std::move(body);

        for (int i = bindings.size() - 1; i >= 0; i--)
        {
            std::cout << "DEBUG: convertLetExpr - creando LetExpr para variable " << bindings[i].name << std::endl;
            currentBody = std::make_unique<ExprStmt>(
                std::make_unique<LetExpr>(
                    bindings[i].name,
                    std::move(bindings[i].initializer),
                    std::move(currentBody),
                    std::move(bindings[i].declaredType)));
        }

        // Extraer el LetExpr del ExprStmt más externo
        auto exprStmt = static_cast<ExprStmt *>(currentBody.get());
        auto letExpr = std::move(exprStmt->expr);
        return std::unique_ptr<LetExpr>(static_cast<LetExpr *>(letExpr.release()));
    }

    std::cout << "DEBUG: convertLetExpr - estructura inválida" << std::endl;
    return nullptr;
}

// Helper functions
std::string CSTToASTConverter::getTokenValue(DerivationNode *node)
{
    if (!node || !isTerminal(node))
    {
        return "";
    }
    return node->token->lexeme;
}

bool CSTToASTConverter::isTerminal(DerivationNode *node)
{
    return node && node->token.has_value();
}

BinaryExpr::Op CSTToASTConverter::convertBinaryOp(const std::string &op)
{
    if (op == "PLUS")
        return BinaryExpr::OP_ADD;
    if (op == "MINUS")
        return BinaryExpr::OP_SUB;
    if (op == "MULT")
        return BinaryExpr::OP_MUL;
    if (op == "DIV")
        return BinaryExpr::OP_DIV;
    if (op == "POW")
        return BinaryExpr::OP_POW;
    if (op == "MOD")
        return BinaryExpr::OP_MOD;
    if (op == "LESS_THAN")
        return BinaryExpr::OP_LT;
    if (op == "GREATER_THAN")
        return BinaryExpr::OP_GT;
    if (op == "LE")
        return BinaryExpr::OP_LE;
    if (op == "GE")
        return BinaryExpr::OP_GE;
    if (op == "EQ")
        return BinaryExpr::OP_EQ;
    if (op == "NEQ")
        return BinaryExpr::OP_NEQ;
    if (op == "AND")
        return BinaryExpr::OP_AND;
    if (op == "OR")
        return BinaryExpr::OP_OR;
    if (op == "CONCAT")
        return BinaryExpr::OP_CONCAT;
    if (op == "CONCAT_WS")
        return BinaryExpr::OP_CONCAT_WS;

    throw std::runtime_error("Unknown binary operator: " + op);
}

UnaryExpr::Op CSTToASTConverter::convertUnaryOp(const std::string &op)
{
    if (op == "MINUS")
        return UnaryExpr::OP_NEG;
    throw std::runtime_error("Unknown unary operator: " + op);
}

// Stub implementations for unused functions
std::vector<std::string> CSTToASTConverter::convertArgIdList(DerivationNode *node)
{
    return {};
}

std::vector<std::unique_ptr<Expr>> CSTToASTConverter::convertArgList(DerivationNode *node)
{
    std::vector<std::unique_ptr<Expr>> args;

    if (!node || node->getSymbol() != "ArgList")
    {
        std::cout << "DEBUG: convertArgList - nodo inválido" << std::endl;
        return args;
    }

    std::cout << "DEBUG: convertArgList - procesando con " << node->children.size() << " hijos" << std::endl;

    // ArgList → Expr ArgListTail | ε
    if (node->children.empty())
    {
        return args; // ε production
    }

    // ArgList → Expr ArgListTail
    if (node->children.size() >= 2)
    {
        auto expr = node->children[0].get();
        auto argListTail = node->children[1].get();

        if (expr)
        {
            auto argExpr = convertExpr(expr);
            if (argExpr)
            {
                args.push_back(std::move(argExpr));
            }
        }

        // Procesar recursivamente el tail
        if (argListTail)
        {
            auto tailArgs = convertArgListTail(argListTail);
            args.insert(args.end(),
                        std::make_move_iterator(tailArgs.begin()),
                        std::make_move_iterator(tailArgs.end()));
        }
    }

    std::cout << "DEBUG: convertArgList - retornando " << args.size() << " argumentos" << std::endl;
    return args;
}

std::shared_ptr<TypeInfo> CSTToASTConverter::convertTypeAnnotation(DerivationNode *node)
{
    if (!node || node->getSymbol() != "TypeAnnotation")
    {
        std::cout << "DEBUG: convertTypeAnnotation - nodo inválido" << std::endl;
        return std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    }

    std::cout << "DEBUG: convertTypeAnnotation - procesando con " << node->children.size() << " hijos" << std::endl;

    // TypeAnnotation → COLON IDENT | ε
    if (node->children.empty())
    {
        std::cout << "DEBUG: convertTypeAnnotation - ε production" << std::endl;
        return std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
    }

    // TypeAnnotation → COLON IDENT
    if (node->children.size() >= 2)
    {
        auto colon = node->children[0].get();
        auto ident = node->children[1].get();

        if (!colon || !ident)
        {
            std::cout << "DEBUG: convertTypeAnnotation - hijos nulos" << std::endl;
            return std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
        }

        std::string typeName = getTokenValue(ident);
        std::cout << "DEBUG: convertTypeAnnotation - tipo: " << typeName << std::endl;

        // Convertir el nombre del tipo a TypeInfo según la lógica de parser.y
        if (typeName == "Number")
        {
            return std::make_shared<TypeInfo>(TypeInfo::Number());
        }
        else if (typeName == "String")
        {
            return std::make_shared<TypeInfo>(TypeInfo::String());
        }
        else if (typeName == "Boolean")
        {
            return std::make_shared<TypeInfo>(TypeInfo::Boolean());
        }
        else
        {
            // Para cualquier otro tipo, usar Object y guardar el nombre del tipo
            return std::make_shared<TypeInfo>(TypeInfo::Object(typeName));
        }
    }

    std::cout << "DEBUG: convertTypeAnnotation - estructura inválida" << std::endl;
    return std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
}

std::pair<std::vector<std::string>, std::vector<std::shared_ptr<TypeInfo>>> CSTToASTConverter::convertTypeParams(DerivationNode *node)
{
    std::vector<std::string> params;
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;

    if (!node || node->getSymbol() != "TypeParams")
    {
        std::cout << "DEBUG: convertTypeParams - nodo inválido" << std::endl;
        return {params, paramTypes};
    }

    std::cout << "DEBUG: convertTypeParams - procesando con " << node->children.size() << " hijos" << std::endl;

    // TypeParams → LPAREN ArgIdList RPAREN | ε
    if (node->children.empty())
    {
        std::cout << "DEBUG: convertTypeParams - ε production" << std::endl;
        return {params, paramTypes}; // ε production
    }

    // TypeParams → LPAREN ArgIdList RPAREN
    if (node->children.size() >= 3)
    {
        auto argIdList = node->children[1].get();
        if (argIdList)
        {
            auto paramInfo = convertArgIdListWithTypes(argIdList);
            params = std::move(paramInfo.first);
            paramTypes = std::move(paramInfo.second);
        }
    }

    std::cout << "DEBUG: convertTypeParams - retornando " << params.size() << " parámetros" << std::endl;
    return {params, paramTypes};
}

// Legacy functions kept for compatibility (not used in new implementation)
DerivationNode *CSTToASTConverter::findChild(DerivationNode *node, const std::string &symbol)
{
    return nullptr;
}

std::vector<DerivationNode *> CSTToASTConverter::findChildren(DerivationNode *node, const std::string &symbol)
{
    return {};
}

bool CSTToASTConverter::isEpsilon(DerivationNode *node)
{
    return node && node->children.empty() && node->getSymbol().find("'") != std::string::npos;
}

// LetExpr helper functions
std::vector<VarBinding> CSTToASTConverter::convertVarBindingList(DerivationNode *node)
{
    std::vector<VarBinding> bindings;

    if (!node || node->getSymbol() != "VarBindingList")
    {
        std::cout << "DEBUG: convertVarBindingList - nodo inválido" << std::endl;
        return bindings;
    }

    std::cout << "DEBUG: convertVarBindingList - procesando con " << node->children.size() << " hijos" << std::endl;

    // VarBindingList → VarBinding VarBindingListTail
    if (node->children.size() >= 2)
    {
        auto varBinding = node->children[0].get();
        auto varBindingListTail = node->children[1].get();

        if (varBinding)
        {
            std::cout << "DEBUG: convertVarBindingList - convirtiendo VarBinding" << std::endl;
            auto binding = convertVarBinding(varBinding);
            bindings.push_back(std::move(binding));
        }

        // Procesar recursivamente el tail
        if (varBindingListTail)
        {
            std::cout << "DEBUG: convertVarBindingList - procesando tail" << std::endl;
            auto tailBindings = convertVarBindingListTail(varBindingListTail);
            bindings.insert(bindings.end(),
                            std::make_move_iterator(tailBindings.begin()),
                            std::make_move_iterator(tailBindings.end()));
        }
    }

    std::cout << "DEBUG: convertVarBindingList - retornando " << bindings.size() << " bindings" << std::endl;
    return bindings;
}

std::vector<VarBinding> CSTToASTConverter::convertVarBindingListTail(DerivationNode *node)
{
    std::vector<VarBinding> bindings;

    if (!node || node->children.empty())
    {
        return bindings; // ε production
    }

    // VarBindingListTail → COMMA VarBinding VarBindingListTail
    if (node->children.size() >= 3)
    {
        auto varBinding = node->children[1].get();
        auto varBindingListTail = node->children[2].get();

        if (varBinding)
        {
            auto binding = convertVarBinding(varBinding);
            bindings.push_back(std::move(binding));
        }

        // Procesar recursivamente el tail
        if (varBindingListTail)
        {
            auto tailBindings = convertVarBindingListTail(varBindingListTail);
            bindings.insert(bindings.end(),
                            std::make_move_iterator(tailBindings.begin()),
                            std::make_move_iterator(tailBindings.end()));
        }
    }

    return bindings;
}

VarBinding CSTToASTConverter::convertVarBinding(DerivationNode *node)
{
    if (!node || node->getSymbol() != "VarBinding")
    {
        std::cout << "DEBUG: convertVarBinding - nodo inválido" << std::endl;
        return VarBinding("", nullptr);
    }

    std::cout << "DEBUG: convertVarBinding - procesando con " << node->children.size() << " hijos" << std::endl;

    // VarBinding → IDENT TypeAnnotation ASSIGN Expr
    if (node->children.size() >= 4)
    {
        auto ident = node->children[0].get();
        auto typeAnnotation = node->children[1].get();
        auto assign = node->children[2].get();
        auto expr = node->children[3].get();

        if (!ident || !expr)
        {
            std::cout << "DEBUG: convertVarBinding - hijos nulos" << std::endl;
            return VarBinding("", nullptr);
        }

        std::string name = getTokenValue(ident);
        std::cout << "DEBUG: convertVarBinding - nombre de variable: " << name << std::endl;

        auto initializer = convertExpr(expr);
        if (!initializer)
        {
            std::cout << "DEBUG: convertVarBinding - error al convertir expresión" << std::endl;
            return VarBinding("", nullptr);
        }

        std::shared_ptr<TypeInfo> type = nullptr;
        if (typeAnnotation)
        {
            type = convertTypeAnnotation(typeAnnotation);
        }

        std::cout << "DEBUG: convertVarBinding - binding creado exitosamente" << std::endl;
        return VarBinding(name, std::move(initializer), std::move(type));
    }

    std::cout << "DEBUG: convertVarBinding - estructura inválida" << std::endl;
    return VarBinding("", nullptr);
}

std::unique_ptr<Stmt> CSTToASTConverter::convertLetBody(DerivationNode *node)
{
    if (!node || node->getSymbol() != "LetBody")
    {
        std::cout << "DEBUG: convertLetBody - nodo inválido" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertLetBody - procesando con " << node->children.size() << " hijos" << std::endl;

    // LetBody → Expr | WhileStmt | ForStmt | BlockStmt
    if (!node->children.empty())
    {
        auto child = node->children[0].get();
        if (!child)
        {
            std::cout << "DEBUG: convertLetBody - hijo nulo" << std::endl;
            return nullptr;
        }

        std::string childSymbol = child->getSymbol();
        std::cout << "DEBUG: convertLetBody - hijo es " << childSymbol << std::endl;

        if (childSymbol == "Expr")
        {
            auto expr = convertExpr(child);
            if (expr)
            {
                return std::make_unique<ExprStmt>(std::move(expr));
            }
        }
        else if (childSymbol == "WhileStmt")
        {
            auto whileExpr = convertWhileExpr(child);
            if (whileExpr)
            {
                return std::make_unique<ExprStmt>(std::move(whileExpr));
            }
        }
        else if (childSymbol == "ForStmt")
        {
            auto forExpr = convertForExpr(child);
            if (forExpr)
            {
                return std::make_unique<ExprStmt>(std::move(forExpr));
            }
        }
        else if (childSymbol == "BlockStmt")
        {
            auto blockExpr = convertBlockStmt(child);
            if (blockExpr)
            {
                return std::make_unique<ExprStmt>(std::move(blockExpr));
            }
        }
    }

    std::cout << "DEBUG: convertLetBody - no se pudo convertir" << std::endl;
    return nullptr;
}

// IfExpr helper functions
std::unique_ptr<Stmt> CSTToASTConverter::convertIfBody(DerivationNode *node)
{
    if (!node || node->getSymbol() != "IfBody")
    {
        std::cout << "DEBUG: convertIfBody - nodo inválido" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertIfBody - procesando con " << node->children.size() << " hijos" << std::endl;

    // IfBody → BlockStmt | Expr
    if (!node->children.empty())
    {
        auto child = node->children[0].get();
        if (!child)
        {
            std::cout << "DEBUG: convertIfBody - hijo nulo" << std::endl;
            return nullptr;
        }

        std::string childSymbol = child->getSymbol();
        std::cout << "DEBUG: convertIfBody - hijo es " << childSymbol << std::endl;

        if (childSymbol == "BlockStmt")
        {
            auto blockExpr = convertBlockStmt(child);
            if (blockExpr)
            {
                return std::make_unique<ExprStmt>(std::move(blockExpr));
            }
        }
        else if (childSymbol == "Expr")
        {
            auto expr = convertExpr(child);
            if (expr)
            {
                return std::make_unique<ExprStmt>(std::move(expr));
            }
        }
    }

    std::cout << "DEBUG: convertIfBody - no se pudo convertir" << std::endl;
    return nullptr;
}

std::vector<std::unique_ptr<IfExpr>> CSTToASTConverter::convertElifList(DerivationNode *node)
{
    std::vector<std::unique_ptr<IfExpr>> elifBranches;

    if (!node || node->getSymbol() != "ElifList")
    {
        std::cout << "DEBUG: convertElifList - nodo inválido" << std::endl;
        return elifBranches;
    }

    std::cout << "DEBUG: convertElifList - procesando con " << node->children.size() << " hijos" << std::endl;

    // ElifList → ElifBranch ElifList | ε
    if (node->children.empty())
    {
        std::cout << "DEBUG: convertElifList - ε production" << std::endl;
        return elifBranches; // ε production
    }

    // ElifList → ElifBranch ElifList
    if (node->children.size() >= 2)
    {
        auto elifBranch = node->children[0].get();
        auto elifList = node->children[1].get();

        if (elifBranch)
        {
            std::cout << "DEBUG: convertElifList - convirtiendo ElifBranch" << std::endl;
            auto elifExpr = convertElifBranch(elifBranch);
            if (elifExpr)
            {
                elifBranches.push_back(std::move(elifExpr));
            }
        }

        // Procesar recursivamente el resto de elif branches
        if (elifList)
        {
            std::cout << "DEBUG: convertElifList - procesando siguiente ElifList" << std::endl;
            auto tailBranches = convertElifList(elifList);
            elifBranches.insert(elifBranches.end(),
                                std::make_move_iterator(tailBranches.begin()),
                                std::make_move_iterator(tailBranches.end()));
        }
    }

    std::cout << "DEBUG: convertElifList - retornando " << elifBranches.size() << " elif branches" << std::endl;
    return elifBranches;
}

std::unique_ptr<IfExpr> CSTToASTConverter::convertElifBranch(DerivationNode *node)
{
    if (!node || node->getSymbol() != "ElifBranch")
    {
        std::cout << "DEBUG: convertElifBranch - nodo inválido" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertElifBranch - procesando con " << node->children.size() << " hijos" << std::endl;

    // ElifBranch → ELIF LPAREN Expr RPAREN IfBody
    if (node->children.size() >= 5)
    {
        // children[0] = ELIF (terminal)
        // children[1] = LPAREN (terminal)
        // children[2] = Expr (condition)
        // children[3] = RPAREN (terminal)
        // children[4] = IfBody

        auto condition = node->children[2].get();
        auto ifBody = node->children[4].get();

        if (!condition || !ifBody)
        {
            std::cout << "DEBUG: convertElifBranch - hijos nulos" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertElifBranch - convirtiendo condición" << std::endl;
        auto condExpr = convertExpr(condition);
        if (!condExpr)
        {
            std::cout << "DEBUG: convertElifBranch - error al convertir condición" << std::endl;
            return nullptr;
        }

        std::cout << "DEBUG: convertElifBranch - convirtiendo if body" << std::endl;
        auto bodyStmt = convertIfBody(ifBody);
        if (!bodyStmt)
        {
            std::cout << "DEBUG: convertElifBranch - error al convertir if body" << std::endl;
            return nullptr;
        }

        // Convertir Stmt a Expr
        auto bodyExprStmt = static_cast<ExprStmt *>(bodyStmt.get());
        auto bodyExpr = std::move(bodyExprStmt->expr);

        std::cout << "DEBUG: convertElifBranch - creando IfExpr" << std::endl;
        return std::make_unique<IfExpr>(
            std::move(condExpr),
            std::move(bodyExpr),
            nullptr); // No else branch for elif
    }

    std::cout << "DEBUG: convertElifBranch - estructura inválida" << std::endl;
    return nullptr;
}

// WhileExpr helper functions
std::unique_ptr<Stmt> CSTToASTConverter::convertWhileBody(DerivationNode *node)
{
    if (!node || node->getSymbol() != "WhileBody")
    {
        std::cout << "DEBUG: convertWhileBody - nodo inválido" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertWhileBody - procesando con " << node->children.size() << " hijos" << std::endl;

    // WhileBody → BlockStmt | Expr
    if (!node->children.empty())
    {
        auto child = node->children[0].get();
        if (!child)
        {
            std::cout << "DEBUG: convertWhileBody - hijo nulo" << std::endl;
            return nullptr;
        }

        std::string childSymbol = child->getSymbol();
        std::cout << "DEBUG: convertWhileBody - hijo es " << childSymbol << std::endl;

        if (childSymbol == "BlockStmt")
        {
            auto blockExpr = convertBlockStmt(child);
            if (blockExpr)
            {
                return std::make_unique<ExprStmt>(std::move(blockExpr));
            }
        }
        else if (childSymbol == "Expr")
        {
            auto expr = convertExpr(child);
            if (expr)
            {
                return std::make_unique<ExprStmt>(std::move(expr));
            }
        }
    }

    std::cout << "DEBUG: convertWhileBody - no se pudo convertir" << std::endl;
    return nullptr;
}

// ForExpr helper functions
std::unique_ptr<Stmt> CSTToASTConverter::convertForBody(DerivationNode *node)
{
    if (!node || node->getSymbol() != "ForBody")
    {
        std::cout << "DEBUG: convertForBody - nodo inválido" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertForBody - procesando con " << node->children.size() << " hijos" << std::endl;

    // ForBody → BlockStmt | Expr
    if (!node->children.empty())
    {
        auto child = node->children[0].get();
        if (!child)
        {
            std::cout << "DEBUG: convertForBody - hijo nulo" << std::endl;
            return nullptr;
        }

        std::string childSymbol = child->getSymbol();
        std::cout << "DEBUG: convertForBody - hijo es " << childSymbol << std::endl;

        if (childSymbol == "BlockStmt")
        {
            auto blockExpr = convertBlockStmt(child);
            if (blockExpr)
            {
                return std::make_unique<ExprStmt>(std::move(blockExpr));
            }
        }
        else if (childSymbol == "Expr")
        {
            auto expr = convertExpr(child);
            if (expr)
            {
                return std::make_unique<ExprStmt>(std::move(expr));
            }
        }
    }

    std::cout << "DEBUG: convertForBody - no se pudo convertir" << std::endl;
    return nullptr;
}

// PrimaryTail conversion
std::unique_ptr<Expr> CSTToASTConverter::convertPrimaryTail(std::unique_ptr<Expr> base, DerivationNode *node)
{
    if (!node || node->getSymbol() != "PrimaryTail")
    {
        return base;
    }

    std::cout << "DEBUG: convertPrimaryTail - procesando con " << node->children.size() << " hijos" << std::endl;

    // PrimaryTail → LPAREN ArgList RPAREN PrimaryTail | DOT IDENT PrimaryTail | ASSIGN_DESTRUCT Expr | ASSIGN Expr | ε
    if (node->children.empty())
    {
        return base; // ε production
    }

    auto firstChild = node->children[0].get();
    if (!firstChild)
    {
        return base;
    }

    std::string childSymbol = firstChild->getSymbol();

    if (childSymbol == "LPAREN")
    {
        // LPAREN ArgList RPAREN PrimaryTail
        if (node->children.size() >= 4)
        {
            auto argList = node->children[1].get();
            auto primaryTail = node->children[3].get();

            std::vector<std::unique_ptr<Expr>> args;
            if (argList)
            {
                args = convertArgList(argList);
            }

            // Distinguir entre CallExpr y MethodCallExpr
            std::unique_ptr<Expr> result = nullptr;

            if (auto getAttrExpr = dynamic_cast<GetAttrExpr *>(base.get()))
            {
                // Es un método: a.b() → MethodCallExpr
                std::cout << "DEBUG: convertPrimaryTail - creando MethodCallExpr para " << getAttrExpr->attrName << std::endl;
                result = std::make_unique<MethodCallExpr>(
                    std::move(getAttrExpr->object),
                    getAttrExpr->attrName,
                    std::move(args));
            }
            else if (auto varExpr = dynamic_cast<VariableExpr *>(base.get()))
            {
                // Es una función: a() → CallExpr
                std::cout << "DEBUG: convertPrimaryTail - creando CallExpr para " << varExpr->name << std::endl;
                result = std::make_unique<CallExpr>(varExpr->name, std::move(args));
            }
            else
            {
                std::cout << "DEBUG: convertPrimaryTail - error: base no es VariableExpr ni GetAttrExpr para LPAREN" << std::endl;
                return base;
            }

            setLineColumn(result.get(), firstChild);

            // Procesar recursivamente el PrimaryTail
            if (primaryTail)
            {
                return convertPrimaryTail(std::move(result), primaryTail);
            }

            return result;
        }
    }

    if (childSymbol == "DOT")
    {
        // DOT IDENT PrimaryTail
        if (node->children.size() >= 3)
        {
            auto ident = node->children[1].get();
            auto primaryTail = node->children[2].get();

            if (!ident)
            {
                return base;
            }

            std::string attrName = getTokenValue(ident);
            auto getAttrExpr = std::make_unique<GetAttrExpr>(std::move(base), attrName);
            setLineColumn(getAttrExpr.get(), ident);

            // Procesar recursivamente el PrimaryTail
            if (primaryTail)
            {
                return convertPrimaryTail(std::move(getAttrExpr), primaryTail);
            }

            return getAttrExpr;
        }
    }

    if (childSymbol == "ASSIGN" || childSymbol == "ASSIGN_DESTRUCT")
    {
        // ASSIGN Expr | ASSIGN_DESTRUCT Expr
        if (node->children.size() >= 2)
        {
            auto expr = node->children[1].get();
            if (!expr)
            {
                return base;
            }

            auto valueExpr = convertExpr(expr);
            if (!valueExpr)
            {
                return base;
            }

            // Verificar si el base es un GetAttrExpr (acceso a atributo)
            if (auto getAttrExpr = dynamic_cast<GetAttrExpr *>(base.get()))
            {
                std::cout << "DEBUG: convertPrimaryTail - creando SetAttrExpr para " << getAttrExpr->attrName << std::endl;
                auto setAttrExpr = std::make_unique<SetAttrExpr>(
                    std::move(getAttrExpr->object),
                    getAttrExpr->attrName,
                    std::move(valueExpr));
                setLineColumn(setAttrExpr.get(), node->children[0].get());
                return setAttrExpr;
            }

            // Si es una variable simple, crear AssignExpr
            std::string varName;
            if (auto varExpr = dynamic_cast<VariableExpr *>(base.get()))
            {
                varName = varExpr->name;
                std::cout << "DEBUG: convertPrimaryTail - creando AssignExpr para " << varName << std::endl;
                auto assignExpr = std::make_unique<AssignExpr>(varName, std::move(valueExpr));
                setLineColumn(assignExpr.get(), node->children[0].get());
                return assignExpr;
            }
            else
            {
                std::cout << "DEBUG: convertPrimaryTail - error: base no es VariableExpr ni GetAttrExpr para ASSIGN" << std::endl;
                return base;
            }
        }
    }

    // Para otros casos, simplemente retornar el base
    return base;
}

std::vector<std::unique_ptr<Expr>> CSTToASTConverter::convertArgListTail(DerivationNode *node)
{
    std::vector<std::unique_ptr<Expr>> args;

    if (!node || node->getSymbol() != "ArgListTail")
    {
        return args;
    }

    // ArgListTail → COMMA Expr ArgListTail | ε
    if (node->children.empty())
    {
        return args; // ε production
    }

    // ArgListTail → COMMA Expr ArgListTail
    if (node->children.size() >= 3)
    {
        auto expr = node->children[1].get();
        auto argListTail = node->children[2].get();

        if (expr)
        {
            auto argExpr = convertExpr(expr);
            if (argExpr)
            {
                args.push_back(std::move(argExpr));
            }
        }

        // Procesar recursivamente el tail
        if (argListTail)
        {
            auto tailArgs = convertArgListTail(argListTail);
            args.insert(args.end(),
                        std::make_move_iterator(tailArgs.begin()),
                        std::make_move_iterator(tailArgs.end()));
        }
    }

    return args;
}

// FunctionDecl helper functions
std::pair<std::vector<std::string>, std::vector<std::shared_ptr<TypeInfo>>> CSTToASTConverter::convertArgIdListWithTypes(DerivationNode *node)
{
    std::vector<std::string> params;
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;

    if (!node || node->getSymbol() != "ArgIdList")
    {
        std::cout << "DEBUG: convertArgIdListWithTypes - nodo inválido" << std::endl;
        return {params, paramTypes};
    }

    std::cout << "DEBUG: convertArgIdListWithTypes - procesando con " << node->children.size() << " hijos" << std::endl;

    // ArgIdList → ArgId ArgIdListTail | ε
    if (node->children.empty())
    {
        return {params, paramTypes}; // ε production
    }

    // ArgIdList → ArgId ArgIdListTail
    if (node->children.size() >= 2)
    {
        auto argId = node->children[0].get();
        auto argIdListTail = node->children[1].get();

        if (argId)
        {
            auto argInfo = convertArgId(argId);
            params.push_back(std::move(argInfo.first));
            paramTypes.push_back(std::move(argInfo.second));
        }

        // Procesar recursivamente el tail
        if (argIdListTail)
        {
            auto tailInfo = convertArgIdListTailWithTypes(argIdListTail);
            params.insert(params.end(),
                          std::make_move_iterator(tailInfo.first.begin()),
                          std::make_move_iterator(tailInfo.first.end()));
            paramTypes.insert(paramTypes.end(),
                              std::make_move_iterator(tailInfo.second.begin()),
                              std::make_move_iterator(tailInfo.second.end()));
        }
    }

    std::cout << "DEBUG: convertArgIdListWithTypes - retornando " << params.size() << " parámetros" << std::endl;
    return {params, paramTypes};
}

std::pair<std::vector<std::string>, std::vector<std::shared_ptr<TypeInfo>>> CSTToASTConverter::convertArgIdListTailWithTypes(DerivationNode *node)
{
    std::vector<std::string> params;
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;

    if (!node || node->getSymbol() != "ArgIdListTail")
    {
        return {params, paramTypes};
    }

    // ArgIdListTail → COMMA ArgId ArgIdListTail | ε
    if (node->children.empty())
    {
        return {params, paramTypes}; // ε production
    }

    // ArgIdListTail → COMMA ArgId ArgIdListTail
    if (node->children.size() >= 3)
    {
        auto argId = node->children[1].get();
        auto argIdListTail = node->children[2].get();

        if (argId)
        {
            auto argInfo = convertArgId(argId);
            params.push_back(std::move(argInfo.first));
            paramTypes.push_back(std::move(argInfo.second));
        }

        // Procesar recursivamente el tail
        if (argIdListTail)
        {
            auto tailInfo = convertArgIdListTailWithTypes(argIdListTail);
            params.insert(params.end(),
                          std::make_move_iterator(tailInfo.first.begin()),
                          std::make_move_iterator(tailInfo.first.end()));
            paramTypes.insert(paramTypes.end(),
                              std::make_move_iterator(tailInfo.second.begin()),
                              std::make_move_iterator(tailInfo.second.end()));
        }
    }

    return {params, paramTypes};
}

std::pair<std::string, std::shared_ptr<TypeInfo>> CSTToASTConverter::convertArgId(DerivationNode *node)
{
    if (!node || node->getSymbol() != "ArgId")
    {
        std::cout << "DEBUG: convertArgId - nodo inválido" << std::endl;
        return {"", std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)};
    }

    std::cout << "DEBUG: convertArgId - procesando con " << node->children.size() << " hijos" << std::endl;

    // ArgId → IDENT TypeAnnotation
    if (node->children.size() >= 2)
    {
        auto ident = node->children[0].get();
        auto typeAnnotation = node->children[1].get();

        if (!ident)
        {
            std::cout << "DEBUG: convertArgId - IDENT nulo" << std::endl;
            return {"", std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)};
        }

        std::string name = getTokenValue(ident);
        std::cout << "DEBUG: convertArgId - nombre: " << name << std::endl;

        std::shared_ptr<TypeInfo> type = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
        if (typeAnnotation)
        {
            type = convertTypeAnnotation(typeAnnotation);
        }

        return {name, std::move(type)};
    }

    std::cout << "DEBUG: convertArgId - estructura inválida" << std::endl;
    return {"", std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)};
}

std::unique_ptr<Stmt> CSTToASTConverter::convertFunctionBody(DerivationNode *node)
{
    if (!node || node->getSymbol() != "FunctionBody")
    {
        std::cout << "DEBUG: convertFunctionBody - nodo inválido" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertFunctionBody - procesando con " << node->children.size() << " hijos" << std::endl;

    // FunctionBody → ARROW Expr | BlockStmt
    if (!node->children.empty())
    {
        auto child = node->children[0].get();
        if (!child)
        {
            std::cout << "DEBUG: convertFunctionBody - hijo nulo" << std::endl;
            return nullptr;
        }

        std::string childSymbol = child->getSymbol();
        std::cout << "DEBUG: convertFunctionBody - hijo es " << childSymbol << std::endl;

        if (childSymbol == "ARROW")
        {
            // ARROW Expr
            if (node->children.size() >= 2)
            {
                auto expr = node->children[1].get();
                if (expr)
                {
                    auto exprNode = convertExpr(expr);
                    if (exprNode)
                    {
                        return std::make_unique<ExprStmt>(std::move(exprNode));
                    }
                }
            }
        }
        else if (childSymbol == "BlockStmt")
        {
            auto blockExpr = convertBlockStmt(child);
            if (blockExpr)
            {
                return std::make_unique<ExprStmt>(std::move(blockExpr));
            }
        }
    }

    std::cout << "DEBUG: convertFunctionBody - no se pudo convertir" << std::endl;
    return nullptr;
}

// TypeDef helper functions
std::pair<std::string, std::vector<std::unique_ptr<Expr>>> CSTToASTConverter::convertTypeInheritance(DerivationNode *node)
{
    std::string baseType = "Object";
    std::vector<std::unique_ptr<Expr>> baseArgs;

    if (!node || node->getSymbol() != "TypeInheritance")
    {
        return {baseType, std::move(baseArgs)};
    }

    std::cout << "DEBUG: convertTypeInheritance - procesando con " << node->children.size() << " hijos" << std::endl;

    // TypeInheritance → INHERITS IDENT TypeBaseArgs | ε
    if (node->children.empty())
    {
        return {baseType, std::move(baseArgs)}; // ε production
    }

    // TypeInheritance → INHERITS IDENT TypeBaseArgs
    if (node->children.size() >= 3)
    {
        auto baseTypeNode = node->children[1].get();
        auto typeBaseArgs = node->children[2].get();

        if (baseTypeNode)
        {
            baseType = getTokenValue(baseTypeNode);
            std::cout << "DEBUG: convertTypeInheritance - tipo base: " << baseType << std::endl;
        }

        if (typeBaseArgs)
        {
            baseArgs = convertTypeBaseArgs(typeBaseArgs);
        }
    }

    return {baseType, std::move(baseArgs)};
}

std::vector<std::unique_ptr<Expr>> CSTToASTConverter::convertTypeBaseArgs(DerivationNode *node)
{
    std::vector<std::unique_ptr<Expr>> args;

    if (!node || node->getSymbol() != "TypeBaseArgs")
    {
        return args;
    }

    // TypeBaseArgs → LPAREN ArgList RPAREN | ε
    if (node->children.empty())
    {
        return args; // ε production
    }

    // TypeBaseArgs → LPAREN ArgList RPAREN
    if (node->children.size() >= 3)
    {
        auto argList = node->children[1].get();
        if (argList)
        {
            args = convertArgList(argList);
        }
    }

    return args;
}

std::pair<std::vector<std::unique_ptr<AttributeDecl>>, std::vector<std::unique_ptr<MethodDecl>>> CSTToASTConverter::convertTypeBody(DerivationNode *node)
{
    std::vector<std::unique_ptr<AttributeDecl>> attributes;
    std::vector<std::unique_ptr<MethodDecl>> methods;

    if (!node || node->getSymbol() != "TypeBody")
    {
        return {std::move(attributes), std::move(methods)};
    }

    std::cout << "DEBUG: convertTypeBody - procesando con " << node->children.size() << " hijos" << std::endl;

    // TypeBody → TypeMember TypeBody | ε
    if (node->children.empty())
    {
        return {std::move(attributes), std::move(methods)}; // ε production
    }

    // TypeBody → TypeMember TypeBody
    if (node->children.size() >= 2)
    {
        auto typeMember = node->children[0].get();
        auto typeBody = node->children[1].get();

        if (typeMember)
        {
            auto memberInfo = convertTypeMember(typeMember);
            if (memberInfo.first)
            {
                attributes.push_back(std::move(memberInfo.first));
            }
            if (memberInfo.second)
            {
                methods.push_back(std::move(memberInfo.second));
            }
        }

        // Procesar recursivamente el resto del cuerpo
        if (typeBody)
        {
            auto tailInfo = convertTypeBody(typeBody);
            attributes.insert(attributes.end(),
                              std::make_move_iterator(tailInfo.first.begin()),
                              std::make_move_iterator(tailInfo.first.end()));
            methods.insert(methods.end(),
                           std::make_move_iterator(tailInfo.second.begin()),
                           std::make_move_iterator(tailInfo.second.end()));
        }
    }

    return {std::move(attributes), std::move(methods)};
}

std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> CSTToASTConverter::convertTypeMember(DerivationNode *node)
{
    std::unique_ptr<AttributeDecl> attribute = nullptr;
    std::unique_ptr<MethodDecl> method = nullptr;

    if (!node || node->getSymbol() != "TypeMember")
    {
        return {std::move(attribute), std::move(method)};
    }

    std::cout << "DEBUG: convertTypeMember - procesando con " << node->children.size() << " hijos" << std::endl;

    // TypeMember → IDENT TypeMemberTail
    if (node->children.size() >= 2)
    {
        auto ident = node->children[0].get();
        auto typeMemberTail = node->children[1].get();

        if (!ident)
        {
            return {std::move(attribute), std::move(method)};
        }

        std::string memberName = getTokenValue(ident);
        std::cout << "DEBUG: convertTypeMember - nombre de miembro: " << memberName << std::endl;

        if (typeMemberTail)
        {
            auto tailInfo = convertTypeMemberTail(memberName, typeMemberTail);
            attribute = std::move(tailInfo.first);
            method = std::move(tailInfo.second);
        }
    }

    return {std::move(attribute), std::move(method)};
}

std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> CSTToASTConverter::convertTypeMemberTail(const std::string &memberName, DerivationNode *node)
{
    std::unique_ptr<AttributeDecl> attribute = nullptr;
    std::unique_ptr<MethodDecl> method = nullptr;

    if (!node || node->getSymbol() != "TypeMemberTail")
    {
        return {std::move(attribute), std::move(method)};
    }

    std::cout << "DEBUG: convertTypeMemberTail - procesando con " << node->children.size() << " hijos" << std::endl;

    // TypeMemberTail → TypeAnnotation TypeMemberTail' | LPAREN ArgIdList RPAREN TypeAnnotation FunctionBody SEMICOLON
    if (node->children.size() >= 2)
    {
        auto firstChild = node->children[0].get();
        if (!firstChild)
        {
            return {std::move(attribute), std::move(method)};
        }

        std::string firstSymbol = firstChild->getSymbol();

        if (firstSymbol == "TypeAnnotation")
        {
            // TypeAnnotation TypeMemberTail' (Attribute)
            auto typeAnnotation = node->children[0].get();
            auto typeMemberTailPrime = node->children[1].get();

            std::shared_ptr<TypeInfo> type = nullptr;
            if (typeAnnotation)
            {
                type = convertTypeAnnotation(typeAnnotation);
            }

            if (typeMemberTailPrime && typeMemberTailPrime->getSymbol() == "TypeMemberTail'")
            {
                // TypeMemberTail' → SEMICOLON | ASSIGN Expr SEMICOLON
                if (typeMemberTailPrime->children.size() >= 1)
                {
                    auto firstPrimeChild = typeMemberTailPrime->children[0].get();
                    if (firstPrimeChild && firstPrimeChild->getSymbol() == "SEMICOLON")
                    {
                        // SEMICOLON - atributo sin inicialización
                        attribute = std::make_unique<AttributeDecl>(memberName, nullptr, std::move(type));
                    }
                    else if (firstPrimeChild && firstPrimeChild->getSymbol() == "ASSIGN")
                    {
                        // ASSIGN Expr SEMICOLON - atributo con inicialización
                        if (typeMemberTailPrime->children.size() >= 3)
                        {
                            auto expr = typeMemberTailPrime->children[1].get();
                            if (expr)
                            {
                                auto exprNode = convertExpr(expr);
                                if (exprNode)
                                {
                                    attribute = std::make_unique<AttributeDecl>(memberName, std::move(exprNode), std::move(type));
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (firstSymbol == "LPAREN")
        {
            // LPAREN ArgIdList RPAREN TypeAnnotation FunctionBody SEMICOLON (Method)
            if (node->children.size() >= 6)
            {
                auto argIdList = node->children[1].get();
                auto typeAnnotation = node->children[3].get();
                auto functionBody = node->children[4].get();

                // Convertir argumentos
                std::vector<std::string> params;
                std::vector<std::shared_ptr<TypeInfo>> paramTypes;
                if (argIdList)
                {
                    auto argInfo = convertArgIdListWithTypes(argIdList);
                    params = std::move(argInfo.first);
                    paramTypes = std::move(argInfo.second);
                }

                // Convertir tipo de retorno
                std::shared_ptr<TypeInfo> returnType = std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown);
                if (typeAnnotation)
                {
                    returnType = convertTypeAnnotation(typeAnnotation);
                }

                // Convertir cuerpo de función
                std::unique_ptr<Stmt> body = nullptr;
                if (functionBody)
                {
                    body = convertFunctionBody(functionBody);
                }

                if (body)
                {
                    method = std::make_unique<MethodDecl>(
                        memberName,
                        std::move(params),
                        std::move(body),
                        std::move(paramTypes),
                        std::move(returnType));
                }
            }
        }
    }

    return {std::move(attribute), std::move(method)};
}

std::unique_ptr<AsExpr> CSTToASTConverter::convertAsExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "AsExpr")
    {
        std::cout << "DEBUG: convertAsExpr - nodo inválido o no es AsExpr" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertAsExpr - procesando con " << node->children.size() << " hijos" << std::endl;

    // AsExpr → AS IDENT AsExpr | ε
    if (node->children.empty())
    {
        std::cout << "DEBUG: convertAsExpr - ε production" << std::endl;
        return nullptr;
    }

    // AsExpr → AS IDENT AsExpr
    if (node->children.size() >= 3)
    {
        auto asToken = node->children[0].get();
        auto typeIdent = node->children[1].get();
        auto nextAsExpr = node->children[2].get();

        if (!asToken || !typeIdent)
        {
            std::cout << "DEBUG: convertAsExpr - tokens nulos" << std::endl;
            return nullptr;
        }

        std::string typeName = getTokenValue(typeIdent);
        std::cout << "DEBUG: convertAsExpr - AS " << typeName << std::endl;

        // Crear AsExpr con una expresión temporal (será reemplazada desde convertUnary)
        auto tempExpr = std::make_unique<NumberExpr>(0);
        auto asExpr = std::make_unique<AsExpr>(std::move(tempExpr), typeName);

        // Procesar el siguiente AsExpr si existe (encadenamiento: x as A as B)
        if (nextAsExpr)
        {
            auto nextExpr = convertAsExpr(nextAsExpr);
            if (nextExpr)
            {
                // Combinar los AsExpr anidados
                return std::make_unique<AsExpr>(std::move(asExpr), nextExpr->typeName);
            }
        }

        return asExpr;
    }

    return nullptr;
}

std::unique_ptr<BaseCallExpr> CSTToASTConverter::convertBaseCallExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "BASE")
    {
        std::cout << "DEBUG: convertBaseCallExpr - nodo inválido o no es BASE" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertBaseCallExpr - procesando con " << node->children.size() << " hijos" << std::endl;

    // BASE LPAREN ArgList RPAREN
    if (node->children.size() >= 4)
    {
        auto lparen = node->children[0].get();
        auto argList = node->children[1].get();
        auto rparen = node->children[2].get();

        if (!lparen || !rparen)
        {
            std::cout << "DEBUG: convertBaseCallExpr - paréntesis nulos" << std::endl;
            return nullptr;
        }

        std::vector<std::unique_ptr<Expr>> args;
        if (argList)
        {
            args = convertArgList(argList);
        }

        std::cout << "DEBUG: convertBaseCallExpr - " << args.size() << " argumentos" << std::endl;
        return std::make_unique<BaseCallExpr>(std::move(args));
    }

    return nullptr;
}

std::unique_ptr<AssignExpr> CSTToASTConverter::convertAssignExpr(DerivationNode *node)
{
    if (!node || node->getSymbol() != "AssignExpr")
    {
        std::cout << "DEBUG: convertAssignExpr - nodo inválido o no es AssignExpr" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertAssignExpr - procesando con " << node->children.size() << " hijos" << std::endl;

    // AssignExpr → IDENT ASSIGN Expr
    if (node->children.size() >= 3)
    {
        auto ident = node->children[0].get();
        auto assign = node->children[1].get();
        auto expr = node->children[2].get();

        if (!ident || !expr)
        {
            std::cout << "DEBUG: convertAssignExpr - hijos nulos" << std::endl;
            return nullptr;
        }

        std::string varName = getTokenValue(ident);
        std::cout << "DEBUG: convertAssignExpr - variable: " << varName << std::endl;

        auto exprNode = convertExpr(expr);
        if (!exprNode)
        {
            std::cout << "DEBUG: convertAssignExpr - error al convertir expresión" << std::endl;
            return nullptr;
        }

        return std::make_unique<AssignExpr>(varName, std::move(exprNode));
    }

    return nullptr;
}

std::unique_ptr<ExprBlock> CSTToASTConverter::convertExprBlock(DerivationNode *node)
{
    if (!node || node->getSymbol() != "ExprBlock")
    {
        std::cout << "DEBUG: convertExprBlock - nodo inválido o no es ExprBlock" << std::endl;
        return nullptr;
    }

    std::cout << "DEBUG: convertExprBlock - procesando con " << node->children.size() << " hijos" << std::endl;

    // ExprBlock → LBRACE StmtList RBRACE
    if (node->children.size() >= 3)
    {
        auto stmtList = node->children[1].get();
        if (!stmtList)
        {
            std::cout << "DEBUG: convertExprBlock - StmtList nulo" << std::endl;
            return nullptr;
        }

        std::vector<std::unique_ptr<Stmt>> stmts;
        convertStmtList(stmtList, stmts);

        std::cout << "DEBUG: convertExprBlock - " << stmts.size() << " statements" << std::endl;
        return std::make_unique<ExprBlock>(std::move(stmts));
    }

    return nullptr;
}

// Helper functions to get line and column information from CST nodes
std::pair<int, int> CSTToASTConverter::getLineColumn(DerivationNode *node)
{
    if (!node)
        return {0, 0};

    // If the node has a token, use its line/column info
    if (node->token.has_value())
    {
        return {node->token->line, node->token->column};
    }

    // Otherwise, try to find the first child with line/column info
    for (const auto &child : node->children)
    {
        if (child)
        {
            auto childInfo = getLineColumn(child.get());
            if (childInfo.first > 0 || childInfo.second > 0)
            {
                return childInfo;
            }
        }
    }

    return {node->line_number, node->column_number};
}

void CSTToASTConverter::setLineColumn(Expr *expr, DerivationNode *node)
{
    if (!expr || !node)
        return;

    auto [line, col] = getLineColumn(node);
    expr->line_number = line;
    expr->column_number = col;
}

void CSTToASTConverter::setLineColumn(Stmt *stmt, DerivationNode *node)
{
    if (!stmt || !node)
        return;

    auto [line, col] = getLineColumn(node);
    stmt->line_number = line;
    stmt->column_number = col;
}