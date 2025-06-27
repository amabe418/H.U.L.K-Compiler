#include "nfa_to_dfa.hpp"
#include <queue>
#include <iostream>

std::set<char> getAlphabet(const NFA &nfa)
{
    std::set<char> alphabet;

    // Obtener todos los símbolos de las transiciones
    // Esto es una simplificación - en la práctica necesitarías
    // acceder a las transiciones del NFA
    for (char c = 32; c <= 126; c++)
    { // Caracteres imprimibles ASCII
        alphabet.insert(c);
    }

    return alphabet;
}

DFA convertNFAtoDFA(const NFA &nfa, const std::map<int, int> &tokenTypes)
{
    DFA dfa;

    // Obtener el alfabeto
    std::set<char> alphabet = getAlphabet(nfa);

    // Estado inicial del DFA: epsilon-closure del estado inicial del NFA
    std::set<int> initialStates = {nfa.getStartState()};
    std::set<int> initialClosure = nfa.epsilonClosure(initialStates);

    // Mapeo de conjuntos de estados NFA a estados DFA
    std::map<std::set<int>, int> stateMapping;
    std::vector<std::set<int>> dfaStates;

    // Cola para procesar estados DFA
    std::queue<int> unprocessedStates;

    // Agregar estado inicial
    stateMapping[initialClosure] = 0;
    dfaStates.push_back(initialClosure);
    unprocessedStates.push(0);
    dfa = DFA(1);

    int nextStateId = 1;

    while (!unprocessedStates.empty())
    {
        int currentDfaState = unprocessedStates.front();
        unprocessedStates.pop();

        std::set<int> currentNfaStates = dfaStates[currentDfaState];

        // Para cada símbolo del alfabeto
        for (char symbol : alphabet)
        {
            // Calcular move y epsilon-closure
            std::set<int> nextNfaStates = nfa.move(currentNfaStates, symbol);
            if (!nextNfaStates.empty())
            {
                std::set<int> nextClosure = nfa.epsilonClosure(nextNfaStates);

                // Verificar si este conjunto de estados ya existe
                auto it = stateMapping.find(nextClosure);
                int nextDfaState;

                if (it == stateMapping.end())
                {
                    // Nuevo estado DFA
                    nextDfaState = nextStateId++;
                    stateMapping[nextClosure] = nextDfaState;
                    dfaStates.push_back(nextClosure);
                    unprocessedStates.push(nextDfaState);
                }
                else
                {
                    nextDfaState = it->second;
                }

                // Agregar transición
                dfa.addTransition(currentDfaState, symbol, nextDfaState);
            }
        }
    }

    // Configurar estados de aceptación y tipos de token
    for (size_t i = 0; i < dfaStates.size(); i++)
    {
        if (nfa.isAccepting(dfaStates[i]))
        {
            dfa.setAccepting(i, true);

            // Buscar el tipo de token de mayor prioridad (menor valor en el enum)
            int bestTokenType = -1;
            for (int s : dfaStates[i])
            {
                auto it = tokenTypes.find(s);
                if (it != tokenTypes.end())
                {
                    int type = it->second;
                    if (bestTokenType == -1 || type < bestTokenType)
                    {
                        bestTokenType = type;
                    }
                }
            }
            if (bestTokenType != -1)
            {
                dfa.setTokenType(i, bestTokenType);
            }
        }
    }

    return dfa;
}