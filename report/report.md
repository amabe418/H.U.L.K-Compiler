# Análisis del Lexer basado en Autómatas (carpeta theoretical/)

## Introducción
El subdirectorio `theoretical/` contiene una implementación de un analizador léxico (lexer) basado en autómatas finitos construidos manualmente, en lugar de utilizar Flex. Esta implementación demuestra los fundamentos teóricos de cómo funciona un lexer desde la perspectiva de la teoría de autómatas y lenguajes formales.

## Estructura de la Implementación

La implementación se divide en tres componentes principales:

### 1. Modelos de Autómatas (`theoretical/automata/`)
- **NFA (Autómata Finito No Determinista)**: Implementado en `nfa.hpp` y `nfa.cpp`
- **DFA (Autómata Finito Determinista)**: Implementado en `dfa.hpp` y `dfa.cpp`
- **Conversión de NFA a DFA**: Implementado en `nfa_to_dfa.hpp` y `nfa_to_dfa.cpp`

### 2. Definición de Tokens (`theoretical/token.hpp` y `token.cpp`)
Define los tipos de tokens que el lexer puede reconocer (palabras clave, identificadores, literales, etc.).

### 3. El Lexer Teórico (`theoretical/Lexer/theoretical_lexer.hpp` y `theoretical_lexer.cpp`)
Utiliza los autómatas para reconocer y clasificar tokens en el código fuente.

## Funcionamiento del Lexer

### Construcción de Autómatas

El lexer se basa en el siguiente flujo de trabajo:

1. **Construcción de NFAs individuales**: Para cada patrón léxico (números, strings, identificadores, palabras clave, etc.) se construye un NFA específico utilizando operaciones como concatenación, unión y cierre de Kleene.

```cpp
NFA TheoreticalLexer::buildNumberNFA()
{
    // Número: [0-9]+(\.[0-9]+)?
    NFA digits = NFA('0', '9').kleenePlus();
    NFA decimal = NFA('.').concatenate(NFA('0', '9').kleenePlus());
    NFA number = digits.concatenate(decimal.optional());
    return number;
}
```

2. **Combinación de NFAs**: Todos los NFAs individuales se combinan en un único NFA usando operaciones de unión, asignando tipos de token a los estados de aceptación.

```cpp
NFA TheoreticalLexer::buildCombinedNFA()
{
    NFA combined;
    // Unión de NFAs para palabras clave, operadores, números, etc.
    // ...
    return combined;
}
```

3. **Conversión de NFA a DFA**: El NFA combinado se convierte a un DFA utilizando el algoritmo de construcción por subconjuntos, lo que mejora la eficiencia del proceso de reconocimiento.

```cpp
void TheoreticalLexer::buildDFA()
{
    NFA combinedNFA = buildCombinedNFA();
    dfa = convertNFAtoDFA(combinedNFA, combinedNFA.getTokenTypes());
}
```

### Proceso de Análisis Léxico

Una vez construido el DFA, el análisis léxico se realiza en estos pasos:

1. **Inicialización**: Se configura el input y se reinician los contadores de posición, línea y columna.

2. **Tokenización**: Se leen caracteres y se simulan en el DFA hasta encontrar un estado de aceptación o error.

```cpp
Token TheoreticalLexer::getNextToken()
{
    // Saltar espacios en blanco y comentarios
    // ...
    
    // Simular el DFA paso a paso
    int currentState = dfa.getStartState();
    int lastAcceptingState = -1;
    int lastAcceptingPos = -1;

    for (size_t i = 0; i < remainingInput.length(); i++)
    {
        char c = remainingInput[i];
        // Buscar la transición en el DFA
        // ...
        if (dfa.isAccepting(nextState))
        {
            lastAcceptingState = nextState;
            lastAcceptingPos = i;
        }
        currentState = nextState;
    }

    // Crear y devolver el token
    // ...
}
```

3. **Generación de tokens**: Para cada secuencia reconocida, se genera un token con su tipo, lexema y posición (línea y columna).

## Diferencias con el Lexer basado en Flex

Esta implementación "teórica" difiere del lexer generado por Flex en varias formas:

1. **Construcción explícita de autómatas**: En lugar de que Flex genere automáticamente los autómatas, aquí se construyen programáticamente.

2. **Mayor control sobre el proceso**: La implementación manual permite entender y controlar cada aspecto del análisis léxico.

3. **Propósito educativo**: Esta implementación está diseñada para ilustrar los conceptos teóricos detrás de un analizador léxico.

## Conclusión

El lexer teórico implementado en la carpeta `theoretical/` demuestra la aplicación práctica de la teoría de autómatas en la construcción de analizadores léxicos. Al construir explícitamente los NFAs y DFAs para cada patrón y simular su ejecución, proporciona una visión clara de cómo funcionan internamente los analizadores léxicos generados por herramientas como Flex.

Esta implementación es una valiosa herramienta pedagógica que complementa el compilador H.U.L.K, permitiendo comparar la versión práctica (basada en Flex) con la versión teórica (basada en autómatas construidos manualmente).