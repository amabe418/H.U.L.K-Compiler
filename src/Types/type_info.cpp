#include "type_info.hpp"
#include "../SymbolTable/symbol_table.hpp"
#include <algorithm>
#include <set>

bool TypeInfo::conformsTo(const TypeInfo &other) const
{
    // Regla 1: Todo tipo conforma a Object
    if (other.getKind() == Kind::Object && other.getTypeName().empty())
    {
        return true;
    }

    // Regla 2: Todo tipo conforma a sí mismo
    if (kind_ == other.kind_ && typeName_ == other.typeName_)
    {
        return true;
    }

    // Regla 3: Si T1 hereda T2 entonces T1 <= T2
    if (kind_ == Kind::Object && other.kind_ == Kind::Object)
    {
        return isSubtypeOf(other.getTypeName());
    }

    // Regla 4: Solo Number, String, Boolean conforman a sí mismos
    if (kind_ == Kind::Number && other.kind_ == Kind::Number)
        return true;
    if (kind_ == Kind::String && other.kind_ == Kind::String)
        return true;
    if (kind_ == Kind::Boolean && other.kind_ == Kind::Boolean)
        return true;

    // Regla 5: Unknown conforma a todo tipo
    if (kind_ == Kind::Unknown)
        return true;

    // Regla 6: Todo tipo conforma a Unknown
    if (other.kind_ == Kind::Unknown)
        return true;

    return false;
}

bool TypeInfo::isSubtypeOf(const std::string &baseTypeName) const
{
    if (typeName_ == baseTypeName)
        return true;

    // Si no tenemos acceso al symbol table, no podemos verificar herencia
    if (!symbolTable_)
        return false;

    // Buscar en la cadena de herencia
    auto currentType = symbolTable_->getTypeDeclaration(typeName_);
    while (currentType && currentType->baseType != "Object")
    {
        if (currentType->baseType == baseTypeName)
            return true;
        currentType = symbolTable_->getTypeDeclaration(currentType->baseType);
    }

    return false;
}

TypeInfo TypeInfo::getLowestCommonAncestor(const std::vector<TypeInfo> &types)
{
    if (types.empty())
        return TypeInfo(Kind::Unknown);
    if (types.size() == 1)
        return types[0];

    // Si no tenemos symbol table, no podemos calcular LCA
    if (!symbolTable_)
        return TypeInfo(Kind::Object);

    // Para tipos primitivos, solo pueden tener LCA si son del mismo tipo
    bool allSamePrimitive = true;
    auto firstKind = types[0].getKind();
    for (const auto &type : types)
    {
        if (type.getKind() != firstKind || type.getKind() == Kind::Object)
        {
            allSamePrimitive = false;
            break;
        }
    }
    if (allSamePrimitive && firstKind != Kind::Object)
    {
        return types[0];
    }

    // Para tipos Object, calcular el LCA en la jerarquía
    std::vector<std::string> typeNames;
    for (const auto &type : types)
    {
        if (type.getKind() == Kind::Object && !type.getTypeName().empty())
        {
            typeNames.push_back(type.getTypeName());
        }
    }

    if (typeNames.empty())
        return TypeInfo(Kind::Object);

    // Encontrar el ancestro común más bajo
    std::string lca = typeNames[0];
    for (size_t i = 1; i < typeNames.size(); ++i)
    {
        lca = findLowestCommonAncestor(lca, typeNames[i]);
    }

    return TypeInfo(Kind::Object, lca);
}

std::string TypeInfo::findLowestCommonAncestor(const std::string &type1, const std::string &type2)
{
    if (type1 == type2)
        return type1;

    // Obtener todas las superclases de type1
    std::set<std::string> type1Ancestors;
    auto current = symbolTable_->getTypeDeclaration(type1);
    while (current && current->baseType != "Object")
    {
        type1Ancestors.insert(current->baseType);
        current = symbolTable_->getTypeDeclaration(current->baseType);
    }

    // Buscar el ancestro común más bajo de type2
    current = symbolTable_->getTypeDeclaration(type2);
    while (current && current->baseType != "Object")
    {
        if (type1Ancestors.find(current->baseType) != type1Ancestors.end())
        {
            return current->baseType;
        }
        current = symbolTable_->getTypeDeclaration(current->baseType);
    }

    return "Object";
}