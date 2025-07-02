#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>

#include "../Value/value.hpp"

struct EnvFrame
{
    // mapa local de nombre→valor
    std::unordered_map<std::string, Value> locals;

    // puntero shared a un frame padre (nullptr si es el global)
    std::shared_ptr<EnvFrame> parent;

    // Constructor: recibe el frame padre
    explicit EnvFrame(std::shared_ptr<EnvFrame> p = nullptr) : parent(std::move(p)) {}

    // Buscar recursivamente un nombre en esta cadena de frames.
    // Si no se halla en ningún nivel, lanza excepción.
    Value
    get(const std::string &name) const
    {
        std::cout << "[DEBUG] EnvFrame::get - looking for '" << name << "' in frame with " << locals.size() << " locals" << std::endl;

        auto it = locals.find(name);
        if (it != locals.end())
        {
            std::cout << "[DEBUG] EnvFrame::get - found '" << name << "' in current frame" << std::endl;
            return it->second;
        }
        if (parent)
        {
            std::cout << "[DEBUG] EnvFrame::get - not found in current frame, searching parent" << std::endl;
            return parent->get(name);
        }
        std::cout << "[DEBUG] EnvFrame::get - variable '" << name << "' not found in any frame" << std::endl;
        throw std::runtime_error("Variable no definida: " + name);
    }

    // Asignar un valor a un nombre en el frame adecuado:
    // - Si el nombre existe en el mapa local, sobrescribir aquí.
    // - Si no existe localmente, pero existe en algún padre, propagar la asignación al padre donde
    // esté originalmente.
    // - Si no existe en ningún lado, crearla en este frame (nivel actual).
    void
    set(const std::string &name, const Value &v)
    {
        if (locals.count(name))
        {
            locals[name] = v;
            return;
        }

        if (parent && parent->existsInChain(name))
        {
            parent->set(name, v);
            return;
        }

        locals[name] = v;
    }

    // Verificar si un nombre existe en esta cadena de frames (local o ancestros).
    bool
    existsInChain(const std::string &name) const
    {
        if (locals.count(name))
            return true;
        if (parent)
            return parent->existsInChain(name);
        return false;
    }
};
