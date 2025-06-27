#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>

class NFA
{
private:
    int numStates;
    std::map<int, std::map<char, std::set<int>>> transitions;
    std::map<int, std::set<int>> epsilonTransitions;
    std::set<int> acceptingStates;
    int startState;
    std::map<int, int> tokenTypes; // estado -> tipo de token

public:
    NFA();
    NFA(int states);

    // Constructor para NFA básico (un símbolo)
    NFA(char symbol);

    // Constructor para NFA de rango [a-z]
    NFA(char from, char to);

    // Constructor para NFA de cualquier carácter excepto
    NFA(const std::set<char> &except);

    // Operaciones básicas
    void addTransition(int from, char symbol, int to);
    void addEpsilonTransition(int from, int to);
    void setAccepting(int state, bool accepting = true);
    void setStartState(int state);
    void setTokenType(int state, int tokenType);

    // Operaciones de composición
    NFA concatenate(const NFA &other) const;
    NFA union_(const NFA &other) const;
    NFA kleeneStar() const;
    NFA kleenePlus() const;
    NFA optional() const;

    // Operaciones de utilidad
    std::set<int> epsilonClosure(const std::set<int> &states) const;
    std::set<int> move(const std::set<int> &states, char symbol) const;
    bool isAccepting(const std::set<int> &states) const;

    // Getters
    int getNumStates() const { return numStates; }
    int getStartState() const { return startState; }
    const std::set<int> &getAcceptingStates() const { return acceptingStates; }
    const std::map<int, int> &getTokenTypes() const { return tokenTypes; }

    // Debug
    void print() const;
};