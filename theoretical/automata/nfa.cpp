#include "nfa.hpp"
#include <iostream>
#include <queue>
#include <algorithm>

NFA::NFA() : numStates(0), startState(0) {}

NFA::NFA(int states) : numStates(states), startState(0) {}

NFA::NFA(char symbol) : numStates(2), startState(0)
{
    addTransition(0, symbol, 1);
    setAccepting(1);
}

NFA::NFA(char from, char to) : numStates(2), startState(0)
{
    for (char c = from; c <= to; c++)
    {
        addTransition(0, c, 1);
    }
    setAccepting(1);
}

NFA::NFA(const std::set<char> &except) : numStates(2), startState(0)
{
    // Para cualquier carácter excepto los especificados
    // En la práctica, esto se implementaría con un rango completo
    // Por simplicidad, usamos un rango amplio
    for (char c = 32; c <= 126; c++)
    { // Caracteres imprimibles ASCII
        if (except.find(c) == except.end())
        {
            addTransition(0, c, 1);
        }
    }
    setAccepting(1);
}

void NFA::addTransition(int from, char symbol, int to)
{
    transitions[from][symbol].insert(to);
}

void NFA::addEpsilonTransition(int from, int to)
{
    epsilonTransitions[from].insert(to);
}

void NFA::setAccepting(int state, bool accepting)
{
    if (accepting)
    {
        acceptingStates.insert(state);
    }
    else
    {
        acceptingStates.erase(state);
    }
}

void NFA::setStartState(int state)
{
    startState = state;
}

void NFA::setTokenType(int state, int tokenType)
{
    tokenTypes[state] = tokenType;
}

NFA NFA::concatenate(const NFA &other) const
{
    NFA result(numStates + other.numStates);

    // Copiar transiciones del primer NFA
    for (const auto &stateTrans : transitions)
    {
        int from = stateTrans.first;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to);
            }
        }
    }

    // Copiar transiciones epsilon del primer NFA
    for (const auto &epsTrans : epsilonTransitions)
    {
        int from = epsTrans.first;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to);
        }
    }

    // Copiar transiciones del segundo NFA (con offset)
    for (const auto &stateTrans : other.transitions)
    {
        int from = stateTrans.first + numStates;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to + numStates);
            }
        }
    }

    // Copiar transiciones epsilon del segundo NFA
    for (const auto &epsTrans : other.epsilonTransitions)
    {
        int from = epsTrans.first + numStates;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to + numStates);
        }
    }

    // Conectar estados finales del primer NFA con el inicial del segundo
    for (int acceptingState : acceptingStates)
    {
        result.addEpsilonTransition(acceptingState, other.startState + numStates);
    }

    // Estados de aceptación del segundo NFA
    for (int acceptingState : other.acceptingStates)
    {
        result.setAccepting(acceptingState + numStates);
    }

    // Propagar tipos de token del primer NFA
    for (const auto &tokenType : tokenTypes)
    {
        result.setTokenType(tokenType.first, tokenType.second);
    }

    // Propagar tipos de token del segundo NFA (con offset)
    for (const auto &tokenType : other.tokenTypes)
    {
        result.setTokenType(tokenType.first + numStates, tokenType.second);
    }

    return result;
}

NFA NFA::union_(const NFA &other) const
{
    NFA result(numStates + other.numStates + 1);
    int newStart = 0;

    // Epsilon transiciones del nuevo estado inicial
    result.addEpsilonTransition(newStart, startState + 1);
    result.addEpsilonTransition(newStart, other.startState + numStates + 1);

    // Copiar transiciones del primer NFA (con offset +1)
    for (const auto &stateTrans : transitions)
    {
        int from = stateTrans.first + 1;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to + 1);
            }
        }
    }

    // Copiar transiciones epsilon del primer NFA
    for (const auto &epsTrans : epsilonTransitions)
    {
        int from = epsTrans.first + 1;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to + 1);
        }
    }

    // Copiar transiciones del segundo NFA (con offset)
    for (const auto &stateTrans : other.transitions)
    {
        int from = stateTrans.first + numStates + 1;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to + numStates + 1);
            }
        }
    }

    // Copiar transiciones epsilon del segundo NFA
    for (const auto &epsTrans : other.epsilonTransitions)
    {
        int from = epsTrans.first + numStates + 1;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to + numStates + 1);
        }
    }

    // Estados de aceptación
    for (int acceptingState : acceptingStates)
    {
        result.setAccepting(acceptingState + 1);
    }
    for (int acceptingState : other.acceptingStates)
    {
        result.setAccepting(acceptingState + numStates + 1);
    }

    // Propagar tipos de token del primer NFA (con offset +1)
    for (const auto &tokenType : tokenTypes)
    {
        result.setTokenType(tokenType.first + 1, tokenType.second);
    }

    // Propagar tipos de token del segundo NFA (con offset)
    for (const auto &tokenType : other.tokenTypes)
    {
        result.setTokenType(tokenType.first + numStates + 1, tokenType.second);
    }

    return result;
}

NFA NFA::kleeneStar() const
{
    NFA result(numStates + 1);
    int newStart = 0;

    // Epsilon transición del nuevo estado inicial al estado inicial original
    result.addEpsilonTransition(newStart, startState + 1);

    // Copiar todas las transiciones (con offset +1)
    for (const auto &stateTrans : transitions)
    {
        int from = stateTrans.first + 1;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to + 1);
            }
        }
    }

    // Copiar transiciones epsilon (con offset +1)
    for (const auto &epsTrans : epsilonTransitions)
    {
        int from = epsTrans.first + 1;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to + 1);
        }
    }

    // Epsilon transiciones de estados de aceptación al estado inicial original
    for (int acceptingState : acceptingStates)
    {
        result.addEpsilonTransition(acceptingState + 1, startState + 1);
    }

    // Epsilon transición del nuevo estado inicial al nuevo estado de aceptación
    result.addEpsilonTransition(newStart, numStates);

    // Estados de aceptación: el nuevo estado inicial y todos los originales
    result.setAccepting(newStart);
    for (int acceptingState : acceptingStates)
    {
        result.setAccepting(acceptingState + 1);
    }

    // Propagar tipos de token (con offset +1)
    for (const auto &tokenType : tokenTypes)
    {
        result.setTokenType(tokenType.first + 1, tokenType.second);
    }

    return result;
}

NFA NFA::kleenePlus() const
{
    NFA result(numStates + 1);
    int newStart = 0;

    // Epsilon transición del nuevo estado inicial al estado inicial original
    result.addEpsilonTransition(newStart, startState + 1);

    // Copiar todas las transiciones (con offset +1)
    for (const auto &stateTrans : transitions)
    {
        int from = stateTrans.first + 1;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to + 1);
            }
        }
    }

    // Copiar transiciones epsilon (con offset +1)
    for (const auto &epsTrans : epsilonTransitions)
    {
        int from = epsTrans.first + 1;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to + 1);
        }
    }

    // Epsilon transiciones de estados de aceptación al estado inicial original
    for (int acceptingState : acceptingStates)
    {
        result.addEpsilonTransition(acceptingState + 1, startState + 1);
    }

    // Estados de aceptación: solo los originales (no el nuevo estado inicial)
    for (int acceptingState : acceptingStates)
    {
        result.setAccepting(acceptingState + 1);
    }

    // Propagar tipos de token (con offset +1)
    for (const auto &tokenType : tokenTypes)
    {
        result.setTokenType(tokenType.first + 1, tokenType.second);
    }

    return result;
}

NFA NFA::optional() const
{
    NFA result(numStates + 1);
    int newStart = 0;

    // Epsilon transición del nuevo estado inicial al estado inicial original
    result.addEpsilonTransition(newStart, startState + 1);

    // Copiar todas las transiciones (con offset +1)
    for (const auto &stateTrans : transitions)
    {
        int from = stateTrans.first + 1;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                result.addTransition(from, symbol, to + 1);
            }
        }
    }

    // Copiar transiciones epsilon (con offset +1)
    for (const auto &epsTrans : epsilonTransitions)
    {
        int from = epsTrans.first + 1;
        for (int to : epsTrans.second)
        {
            result.addEpsilonTransition(from, to + 1);
        }
    }

    // Estados de aceptación: el nuevo estado inicial y todos los originales
    result.setAccepting(newStart);
    for (int acceptingState : acceptingStates)
    {
        result.setAccepting(acceptingState + 1);
    }

    // Propagar tipos de token (con offset +1)
    for (const auto &tokenType : tokenTypes)
    {
        result.setTokenType(tokenType.first + 1, tokenType.second);
    }

    return result;
}

std::set<int> NFA::epsilonClosure(const std::set<int> &states) const
{
    std::set<int> closure = states;
    std::queue<int> queue;

    for (int state : states)
    {
        queue.push(state);
    }

    while (!queue.empty())
    {
        int current = queue.front();
        queue.pop();

        auto it = epsilonTransitions.find(current);
        if (it != epsilonTransitions.end())
        {
            for (int next : it->second)
            {
                if (closure.insert(next).second)
                {
                    queue.push(next);
                }
            }
        }
    }

    return closure;
}

std::set<int> NFA::move(const std::set<int> &states, char symbol) const
{
    std::set<int> result;

    for (int state : states)
    {
        auto stateIt = transitions.find(state);
        if (stateIt != transitions.end())
        {
            auto symbolIt = stateIt->second.find(symbol);
            if (symbolIt != stateIt->second.end())
            {
                for (int next : symbolIt->second)
                {
                    result.insert(next);
                }
            }
        }
    }

    return result;
}

bool NFA::isAccepting(const std::set<int> &states) const
{
    for (int state : states)
    {
        if (acceptingStates.find(state) != acceptingStates.end())
        {
            return true;
        }
    }
    return false;
}

void NFA::print() const
{
    std::cout << "NFA with " << numStates << " states" << std::endl;
    std::cout << "Start state: " << startState << std::endl;
    std::cout << "Accepting states: ";
    for (int state : acceptingStates)
    {
        std::cout << state << " ";
    }
    std::cout << std::endl;

    std::cout << "Transitions:" << std::endl;
    for (const auto &stateTrans : transitions)
    {
        int from = stateTrans.first;
        for (const auto &symbolTrans : stateTrans.second)
        {
            char symbol = symbolTrans.first;
            for (int to : symbolTrans.second)
            {
                std::cout << "  " << from << " --" << symbol << "--> " << to << std::endl;
            }
        }
    }

    std::cout << "Epsilon transitions:" << std::endl;
    for (const auto &epsTrans : epsilonTransitions)
    {
        int from = epsTrans.first;
        for (int to : epsTrans.second)
        {
            std::cout << "  " << from << " --ε--> " << to << std::endl;
        }
    }
}