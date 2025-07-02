#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "value.hpp"

class RangeIterator
{
   public:
    // Construye el iterador a partir de la secuencia de enteros [min, max)
    // Recibe internamente un vector de Value, pero en la práctica este vector
    // vendrá de range(min,max).
    explicit RangeIterator(const std::vector<Value>& seq) : data(seq), index(0) {}

    // Avanza al siguiente elemento de la secuencia.
    // Devuelve true si tras avanzar hay un elemento válido.
    bool
    next()
    {
        if (index < data.size())
        {
            index++;
            return (index <= data.size());
        }
        return false;
    }

    // Devuelve el elemento actual. Lanza excepción si nunca se llamó a next()
    // o si ya se pasó del final.
    Value
    current() const
    {
        if (index == 0 || index > data.size())
        {
            throw std::runtime_error("RangeIterator::current() fuera de rango");
        }
        return data[index - 1];
    }

   private:
    std::vector<Value> data;  // Los valores precomputados
    std::size_t index;        // 0 = antes de la primera llamada a next()
};