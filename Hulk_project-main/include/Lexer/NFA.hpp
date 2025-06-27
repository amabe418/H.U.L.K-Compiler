#ifndef NFA_HPP
#define NFA_HPP

#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <iostream>
#include "DFA.hpp"
#include "Token.hpp"

class NFA {
public:
    int m_start_state;
    int m_total_states;
    std::set<char> m_alphabet;
    std::map<int, std::vector<Transition>> m_transitions;
    std::vector<int> m_final_states;
    std::map<int, TokenType> m_final_token_types;

public:
    // Constructor
    NFA(int start_state, int total_states, std::vector<int> final_states,
        std::set<char> alphabet, std::map<int, std::vector<Transition>> transitions)
        {
            m_total_states=total_states;
            m_start_state=start_state;
            m_final_states=final_states;
            m_alphabet=alphabet;
            m_transitions=transitions;
        }

    // Evaluación
    bool evaluate(std::string input);

    // Validación de símbolo
    bool is_valid(const char symbol);

    // Obtener siguiente conjunto de estados
    std::pair<std::vector<int>, bool> nextStates(char c, int state, bool useAny) const;

    // Verificar estado final
    bool is_final_state(int current_state);

    // Conversión a DFA
    DFA convertToDFA();

    // Constructores estáticos para REs
    static NFA emptyRE();
    static NFA epsilonRE();
    static NFA anyRE();
    static NFA symbolRE(char symbol);
    static NFA RangeRE(char from, char to);
    static NFA UnionRE(NFA a1, NFA a2);
    static NFA ConcatenationRE(NFA a1, NFA a2);
    static NFA ClousureRE(NFA a1);

private:
    // Cierre epsilon para un estado
    std::pair<std::set<int>, bool> eClousure(int state, bool initial);

    // Cierre epsilon para un conjunto
    std::pair<std::set<int>, bool> eClousure_set(std::set<int> set);

    // Movimiento con símbolo
    std::pair<std::set<int>, bool> move(std::set<int> set, char symbol);

    // Transiciones epsilon desde un estado
    std::pair<std::vector<int>, bool> epsilonTransitions(int state) const;
};

#endif // NFA_HPP
