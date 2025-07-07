#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "iterable.hpp"
#include "value.hpp"

class RangeValue
{
   public:
    // Constructor de tipo builtin: recibe min y max (ambos números).
    RangeValue(double min_, double max_) : min(min_), max(max_)
    {
        if (min > max)
        {
            throw std::runtime_error("RangeValue: min > max inválido");
        }
    }

    // Método "iter()" que crea una nueva instancia de RangeIterator,
    // con la secuencia [min, min+1, ..., max-1]. Cada llamada a iter()
    // genera un iterador independiente, capaz de recorrer la misma secuencia:
    std::shared_ptr<RangeIterator>
    iter() const
    {
        // Construir el vector de valores [min, min+1, ..., max-1]:
        std::vector<Value> seq;
        for (double v = min; v < max; v += 1.0)
        {
            seq.emplace_back(Value(v));
        }
        // Devolver un iterador que recorra `seq`
        return std::make_shared<RangeIterator>(seq);
    }

   private:
    double min, max;
};