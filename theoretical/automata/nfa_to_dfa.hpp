#pragma once
#include "nfa.hpp"
#include "dfa.hpp"
#include <map>
#include <vector>

// Función para convertir NFA a DFA usando el algoritmo de subconjuntos
DFA convertNFAtoDFA(const NFA &nfa, const std::map<int, int> &tokenTypes = {});

// Función auxiliar para obtener el alfabeto del NFA
std::set<char> getAlphabet(const NFA &nfa);