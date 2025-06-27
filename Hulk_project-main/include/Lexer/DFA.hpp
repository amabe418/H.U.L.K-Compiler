#ifndef DFA_HPP
#define DFA_HPP

#include <vector>
#include <map>
#include <set>
#include <string>
#include <filesystem>
#include "Token.hpp"

struct Transition
{
    char character;
    std::vector<int> to;

    std::string type;

    Transition(char c,std::vector<int> to,std::string t)
    :character(c),to(to),type(t){}

};

class DFA {
public:
    int m_start_state;
    int m_total_states;
    std::vector<int> m_final_states;
    std::set<char> m_alphabet;
    std::map<int, std::vector<Transition>> m_transitions;
    std::map<int, TokenType> m_final_token_types;

public:
    // Constructores
    DFA(int start_state, int total_states, std::vector<int> final_states,
        std::set<char> alphabet, std::map<int, std::vector<Transition>> transitions)
        {
            m_start_state=start_state;
            m_total_states=total_states;
            m_final_states=final_states;
            m_alphabet=alphabet;
            m_transitions=transitions;
        }
    DFA(){}; // Constructor por defecto

    // Evaluar cadena de entrada
    bool evualuate(std::string input);

    // Obtener estado siguiente
    int nextState(char c, int state);

    // Verifica si símbolo es válido
    bool is_valid(const char symbol);

    // Verifica si estado es final
    bool is_final_state(int current_state);

    // Verifica si existe archivo binario de DFA
    static bool existsDfa();

    // Serialización (opcional - descomentable)
    /*
    static void save_binary(const DFA& dfa, const std::string& filename = "dfa.bin");
    static DFA load_binary(const std::string& filename = "dfa.bin");
    */
};

#endif // DFA_HPP
