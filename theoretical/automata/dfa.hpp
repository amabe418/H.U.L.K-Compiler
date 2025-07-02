#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>

class DFA
{
private:
    int numStates;
    std::map<int, std::map<char, int>> transitions;
    std::set<int> acceptingStates;
    int startState;
    std::map<int, int> tokenTypes; // Estado -> tipo de token

public:
    DFA();
    DFA(int states);

    // Operaciones básicas
    void addTransition(int from, char symbol, int to);
    void setAccepting(int state, bool accepting = true);
    void setStartState(int state);
    void setTokenType(int state, int tokenType);

    // Simulación
    int simulate(const std::string &input, int &lastAcceptingState) const;
    bool isAccepting(int state) const;

    // Getters
    int getNumStates() const { return numStates; }
    int getStartState() const { return startState; }
    const std::set<int> &getAcceptingStates() const { return acceptingStates; }
    const std::map<int, std::map<char, int>> &getTransitions() const { return transitions; }
    int getTokenType(int state) const;

    // Debug
    void print() const;
};