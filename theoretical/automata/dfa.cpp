#include "dfa.hpp"
#include <iostream>

DFA::DFA() : numStates(0), startState(0) {}

DFA::DFA(int states) : numStates(states), startState(0) {}

void DFA::addTransition(int from, char symbol, int to)
{
    transitions[from][symbol] = to;
}

void DFA::setAccepting(int state, bool accepting)
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

void DFA::setStartState(int state)
{
    startState = state;
}

void DFA::setTokenType(int state, int tokenType)
{
    tokenTypes[state] = tokenType;
}

int DFA::simulate(const std::string &input, int &lastAcceptingState) const
{
    int currentState = startState;
    lastAcceptingState = -1;
    int lastAcceptingPos = -1;

    for (size_t i = 0; i < input.length(); i++)
    {
        char symbol = input[i];

        // Verificar si el estado actual es de aceptación
        if (isAccepting(currentState))
        {
            lastAcceptingState = currentState;
            lastAcceptingPos = i;
        }

        // Buscar transición
        auto stateIt = transitions.find(currentState);
        if (stateIt == transitions.end())
        {
            break; // No hay transiciones desde este estado
        }

        auto symbolIt = stateIt->second.find(symbol);
        if (symbolIt == stateIt->second.end())
        {
            break; // No hay transición para este símbolo
        }

        currentState = symbolIt->second;
    }

    // Verificar el estado final
    if (isAccepting(currentState))
    {
        lastAcceptingState = currentState;
        lastAcceptingPos = input.length();
    }

    return lastAcceptingPos;
}

bool DFA::isAccepting(int state) const
{
    return acceptingStates.find(state) != acceptingStates.end();
}

int DFA::getTokenType(int state) const
{
    auto it = tokenTypes.find(state);
    return (it != tokenTypes.end()) ? it->second : -1;
}

void DFA::print() const
{
    std::cout << "DFA with " << numStates << " states" << std::endl;
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
            int to = symbolTrans.second;
            std::cout << "  " << from << " --" << symbol << "--> " << to;
            if (isAccepting(to))
            {
                std::cout << " (accepting)";
            }
            std::cout << std::endl;
        }
    }

    std::cout << "Token types:" << std::endl;
    for (const auto &tokenType : tokenTypes)
    {
        std::cout << "  State " << tokenType.first << " -> Token " << tokenType.second << std::endl;
    }
}