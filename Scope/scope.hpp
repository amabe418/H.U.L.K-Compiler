#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

/// Información asociada a cada símbolo en el análisis semántico
struct SymbolInfo
{
    enum Kind
    {
        VARIABLE,
        FUNCTION,
        TYPE
    } kind;
};

template <typename Info>
class Scope
{
public:
    using Ptr = std::shared_ptr<Scope<Info>>;

    /// Crea un nuevo scope con padre opcional
    explicit Scope(Ptr parent = nullptr)
        : parent_{std::move(parent)} {}

    /// Declara un símbolo en el scope actual
    void declare(const std::string &name, const Info &info)
    {
        if (symbols_.count(name))
        {
            throw std::runtime_error("Símbolo ya declarado en este scope: " + name);
        }
        symbols_[name] = info;
    }

    /// Busca recursivamente en la cadena de scopes
    const Info &lookup(const std::string &name) const
    {
        auto it = symbols_.find(name);
        if (it != symbols_.end())
        {
            return it->second;
        }
        if (parent_)
        {
            return parent_->lookup(name);
        }
        throw std::runtime_error("Símbolo no definido: " + name);
    }

    /// ¿Existe en el scope actual?
    bool existsInCurrent(const std::string &name) const
    {
        return symbols_.count(name) > 0;
    }

    /// Acceso al padre
    Ptr parent() const { return parent_; }

private:
    std::unordered_map<std::string, Info> symbols_;
    Ptr parent_;
};
