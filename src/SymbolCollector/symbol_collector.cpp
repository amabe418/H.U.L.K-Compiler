#include "symbol_collector.hpp"

#include "../Symbols/symbol_table.hpp"

void
SymbolCollector::visit(Program* prog)
{
    // Asegurarnos de que estamos en el scope global
    while (symbolTable.getCurrentScope() > 0)
    {
        symbolTable.exitScope();
    }

    for (auto& stmt : prog->stmts) stmt->accept(this);
}

void
SymbolCollector::visit(FunctionDecl* func)
{
    // Verificar si la función ya está definida
    if (symbolTable.lookupInCurrentScope(func->name))
    {
        throw TypeError("Function " + func->name + " already defined");
    }

    // Registrar la función en la tabla de símbolos
    Symbol funcSymbol;
    funcSymbol.name = func->name;
    funcSymbol.kind = SymbolKind::Function;
    symbolTable.define(func->name, funcSymbol);

    // Entrar en un nuevo scope para los parámetros
    symbolTable.enterScope();

    // Registrar los parámetros
    for (const auto& param : func->params)
    {
        Symbol paramSymbol;
        paramSymbol.name = param;
        paramSymbol.kind = SymbolKind::Parameter;
        symbolTable.define(param, paramSymbol);
    }

    // Visitar el cuerpo de la función
    pushCaller(func->name);  // Agregar la función actual al stack
    func->body->accept(this);
    popCaller();  // Remover la función del stack

    // Salir del scope de la función
    symbolTable.exitScope();
}

void
SymbolCollector::visit(TypeDecl* typeDecl)
{
    // Verificar si el tipo ya está definido
    if (symbolTable.lookupInCurrentScope(typeDecl->name))
    {
        throw TypeError("Type " + typeDecl->name + " already defined");
    }

    // Registrar el tipo en la tabla de símbolos
    Symbol typeSymbol;
    typeSymbol.name = typeDecl->name;
    typeSymbol.kind = SymbolKind::Type;
    symbolTable.define(typeDecl->name, typeSymbol);

    // Entrar en un nuevo scope para los parámetros, atributos y métodos
    symbolTable.enterScope();

    // Registrar los parámetros del tipo
    for (const auto& param : typeDecl->params)
    {
        Symbol paramSymbol;
        paramSymbol.name = param;
        paramSymbol.kind = SymbolKind::TypeParameter;
        symbolTable.define(param, paramSymbol);
    }

    // Registrar los atributos con sus tipos
    for (const auto& attr : typeDecl->attributes)
    {
        Symbol attrSymbol;
        attrSymbol.name = attr->name;
        attrSymbol.kind = SymbolKind::Attribute;
        // Si el atributo tiene un tipo inferido, lo guardamos
        if (attr->initializer && attr->initializer->inferredType)
        {
            attrSymbol.type = attr->initializer->inferredType;
        }
        symbolTable.define(attr->name, attrSymbol);
    }

    // Registrar los métodos
    for (const auto& method : typeDecl->methods)
    {
        Symbol methodSymbol;
        methodSymbol.name = method->name;
        methodSymbol.kind = SymbolKind::Method;
        // Si el método tiene un tipo inferido, lo guardamos
        if (method->body && method->body->inferredType)
        {
            methodSymbol.type = method->body->inferredType;
        }
        symbolTable.define(method->name, methodSymbol);
    }

    // Salir del scope del tipo
    symbolTable.exitScope();
}

void
SymbolCollector::visit(MethodDecl* method)
{
    // Entrar en un nuevo scope para los parámetros del método
    symbolTable.enterScope();

    // Registrar los parámetros
    for (const auto& param : method->params)
    {
        Symbol paramSymbol;
        paramSymbol.name = param;
        paramSymbol.kind = SymbolKind::Parameter;
        symbolTable.define(param, paramSymbol);
    }

    // Visitar el cuerpo del método
    method->body->accept(this);

    // Salir del scope del método
    symbolTable.exitScope();
}

void
SymbolCollector::visit(ExprStmt* stmt)
{
    stmt->expr->accept(this);
}

void
SymbolCollector::visit(ExprBlock* block)
{
    // Entrar en un nuevo scope
    symbolTable.enterScope();

    // Visitar cada statement en el bloque
    for (auto& stmt : block->stmts) stmt->accept(this);

    // Salir del scope
    symbolTable.exitScope();
}

void
SymbolCollector::visit(LetExpr* let)
{
    // Evaluar el inicializador
    let->initializer->accept(this);

    // Entrar en un nuevo scope
    symbolTable.enterScope();

    // Registrar la variable
    Symbol varSymbol;
    varSymbol.name = let->name;
    varSymbol.kind = SymbolKind::Variable;

    std::cout << "nombre de la variable: " << let->name << std::endl;
    std::cout << "nombre de la variable: " << varSymbol.name << std::endl;
    std::cout << "numero de scope: " << symbolTable.getCurrentScope() << std::endl;
    // Debug print
    symbolTable.define(let->name, varSymbol);

    // Visitar el cuerpo
    let->body->accept(this);

    // Salir del scope
    symbolTable.exitScope();
}

// Implementaciones de los métodos de ExprVisitor
void
SymbolCollector::visit(NumberExpr*)
{
}
void
SymbolCollector::visit(StringExpr*)
{
}
void
SymbolCollector::visit(BooleanExpr*)
{
}

void
SymbolCollector::visit(UnaryExpr* expr)
{
    expr->operand->accept(this);
}

void
SymbolCollector::visit(BinaryExpr* expr)
{
    expr->left->accept(this);
    expr->right->accept(this);
}

void
SymbolCollector::visit(CallExpr* expr)
{
    // Visitar los argumentos
    for (auto& arg : expr->args)
    {
        arg->accept(this);
    }

    // Obtener el nombre de la función llamada
    std::string callee = expr->callee;
    std::string caller = getCurrentCaller();

    std::cout << "Function " << callee << " called from " << caller << std::endl;
}

void
SymbolCollector::visit(VariableExpr*)
{
}

void
SymbolCollector::visit(AssignExpr* expr)
{
    expr->value->accept(this);
}

void
SymbolCollector::visit(IfExpr* expr)
{
    expr->condition->accept(this);
    expr->thenBranch->accept(this);
    if (expr->elseBranch)
    {
        expr->elseBranch->accept(this);
    }
}

void
SymbolCollector::visit(WhileExpr* expr)
{
    expr->condition->accept(this);
    expr->body->accept(this);
}

void
SymbolCollector::visit(NewExpr* expr)
{
    for (auto& arg : expr->args)
    {
        arg->accept(this);
    }
}

void
SymbolCollector::visit(GetAttrExpr* expr)
{
    expr->object->accept(this);
}

void
SymbolCollector::visit(SetAttrExpr* expr)
{
    expr->object->accept(this);
    expr->value->accept(this);
}

void
SymbolCollector::visit(MethodCallExpr* expr)
{
    expr->object->accept(this);
    for (auto& arg : expr->args)
    {
        arg->accept(this);
    }
}

void
SymbolCollector::visit(SelfExpr*)
{
}

void
SymbolCollector::visit(BaseCallExpr* expr)
{
    for (auto& arg : expr->args)
    {
        arg->accept(this);
    }
}
