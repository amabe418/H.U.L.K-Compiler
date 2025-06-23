# Informe:
### Autores:
1. Amalia Beatriz Valiente Hinojosa.
2. Jorge Alejandro Echevarría Brunet.
3. Noel Pérez Calvo.

---

El presente proyecto consiste en el desarrollo de un compilador e intérprete para un lenguaje de programación propio denominado "hulk". El objetivo principal es transformar programas escritos en este lenguaje en una representación que pueda ser ejecutada por una máquina, asegurando la corrección sintáctica y semántica del código fuente. El diseño modular y la implementación cuidadosa de cada etapa del proceso de compilación permiten garantizar la robustez, extensibilidad y mantenibilidad del sistema.

# Proceso General
El proceso de compilación e interpretación en este proyecto se compone de varias etapas fundamentales, cada una de las cuales cumple una función específica en la traducción y validación del código fuente:  
### Análisis Léxico
En esta primera etapa, el compilador lee el código fuente y lo divide en unidades mínimas de significado llamadas "tokens" (palabras clave, identificadores, operadores, etc.). Esta tarea es realizada por el analizador léxico, cuyas reglas están definidas en el archivo lexer.l.  
### Análisis Sintáctico
Los tokens generados son procesados por el analizador sintáctico, que verifica que la secuencia de tokens cumpla con la gramática del lenguaje. Si la estructura es válida, se construye un Árbol de Sintaxis Abstracta (AST), que representa jerárquicamente la estructura del programa. La gramática y las reglas sintácticas se encuentran en el archivo parser.y.  
### Chequeo Semántico
Una vez construido el AST, se realiza un análisis semántico para asegurar que el programa tenga sentido desde el punto de vista lógico. Por ejemplo, se verifica que las variables estén declaradas antes de ser utilizadas, que los tipos de datos sean compatibles en las operaciones, y que las funciones reciban los argumentos correctos. Esta etapa es gestionada por los módulos ubicados en SemanticCheck/.  
### Resolución de Nombres y Manejo de la Tabla de Símbolos
Durante el análisis semántico, se lleva a cabo la resolución de nombres, que consiste en asociar cada identificador con su declaración correspondiente. Para ello, se utiliza una tabla de símbolos, que almacena información relevante sobre variables, funciones y otros elementos del programa.   
### Generación de Código
Una vez validado el programa, el compilador traduce el AST a una representación intermedia o directamente a código ejecutable. En este proyecto, se genera código intermedio (por ejemplo, LLVM IR), que puede ser posteriormente ejecutado o compilado a código máquina. Esta funcionalidad se encuentra en el módulo Codegen/.  
### Ejecución o Evaluación
Finalmente, el código generado puede ser ejecutado directamente mediante un intérprete, o bien convertirse en un archivo ejecutable. El resultado de la ejecución se muestra al usuario, completando así el ciclo de compilación e interpretación.  
### Organización del Proyecto
El proyecto está organizado en módulos que reflejan cada una de las etapas mencionadas:  
- Lexer/: Análisis léxico.  
- Parser/: Análisis sintáctico.  
- AST/: Representación del árbol de sintaxis abstracta.  
- SemanticCheck/: Análisis semántico.  
- NameResolver/ y SymbolTable/: Resolución de nombres y manejo de la tabla de símbolos.  
- Codegen/: Generación de código.  
- Evaluator/: Ejecución o evaluación del código.  
- Types/ y Value/: Manejo de tipos de datos y valores.  
### Flujo de Trabajo  
El flujo de trabajo típico es el siguiente:  
1.  El usuario escribe un programa en "hulk".  
2.  El compilador realiza el análisis léxico y sintáctico, construyendo el AST.  
3.  Se verifica la corrección semántica del programa.  
4.  Se genera el código intermedio o ejecutable.  
5.  El programa es ejecutado y se muestran los resultados al usuario.  


# Análisis Léxico en el Proyecto "hulk"

El análisis léxico constituye la primera etapa fundamental en el proceso de compilación del lenguaje "hulk". Su objetivo es transformar el flujo de caracteres del código fuente en una secuencia de unidades significativas denominadas **tokens**, que serán posteriormente utilizadas por el analizador sintáctico. Este proceso es gestionado principalmente por el archivo `lexer.l`, que define las reglas y patrones para identificar los distintos elementos léxicos del lenguaje.

## Funcionamiento del Analizador Léxico

Cuando un usuario escribe un programa en "hulk", el analizador léxico comienza leyendo el archivo fuente carácter por carácter. Utilizando las expresiones regulares y reglas definidas en `lexer.l`, el lexer reconoce patrones específicos que corresponden a los distintos tipos de tokens del lenguaje:

- **Palabras clave**: como `let`, `if`, `else`, `while`, `function`, etc.
- **Identificadores**: nombres de variables, funciones y tipos definidos por el usuario.
- **Literales numéricos y de cadena**: por ejemplo, `123`, `45.67`, o `"texto"`.
- **Operadores**: aritméticos (`+`, `-`, `*`, `/`), lógicos (`&&`, `||`), de comparación (`==`, `!=`, `<`, `>`, etc.), y otros símbolos del lenguaje.
- **Signos de puntuación**: como paréntesis, llaves, comas, punto y coma, etc.

Por ejemplo, al encontrar una secuencia de dígitos como `123` o `45.67`, el lexer la identifica como un número y genera un token de tipo `NUMBER`. Si detecta una palabra como `let`, la reconoce como una palabra reservada y produce el token correspondiente. De igual manera, los operadores y signos de puntuación son identificados y convertidos en sus respectivos tokens.

## Manejo de Espacios en Blanco y Comentarios

El lexer también se encarga de ignorar los espacios en blanco y los comentarios, asegurando que solo los elementos relevantes del código sean procesados en etapas posteriores. Esto permite que el código fuente sea más legible para el programador sin afectar el análisis posterior.

## Seguimiento de Ubicación y Reporte de Tokens

Además, el lexer mantiene un seguimiento preciso de la ubicación de cada token (línea y columna), lo que resulta esencial para la generación de mensajes de error claros y útiles durante la compilación. Para facilitar la depuración y el análisis del flujo de tokens, el lexer incluye una función de impresión (`print_token`) que muestra información detallada sobre cada token reconocido: su tipo, valor y la posición exacta en el código fuente.

## Integración con la Representación del AST

Una característica relevante del lexer en este proyecto es su integración directa con la construcción de nodos del Árbol de Sintaxis Abstracta (AST), definidos en `ast.hpp`. Por ejemplo, al reconocer un literal numérico, el lexer no solo genera el token correspondiente, sino que también instancia un nodo `NumberExpr` del AST, encapsulando el valor numérico detectado. De manera similar, para literales de cadena y booleanos, se crean instancias de `StringExpr` y `BooleanExpr`, respectivamente.

Esta integración temprana facilita la transición entre el análisis léxico y el sintáctico, permitiendo que el parser trabaje directamente con nodos del AST en lugar de simples valores, lo que agiliza la construcción de la estructura jerárquica del programa.

## Manejo de Palabras Reservadas e Identificadores

El lexer distingue entre palabras reservadas del lenguaje (como `if`, `else`, `let`, `while`, `function`, etc.) e identificadores definidos por el usuario (nombres de variables, funciones, tipos). Para lograr esto, compara cada secuencia de caracteres alfabéticos con una lista de palabras clave. Si la secuencia coincide con una palabra reservada, se genera el token específico; de lo contrario, se trata como un identificador y se crea un token `IDENT`.

### Ejemplo de Tokenización

Supongamos que el usuario escribe el siguiente fragmento de código:

```hulk
let x = 42;
print(x + 1);
```

El lexer generará la siguiente secuencia de tokens:

- `LET` (palabra clave)
- `IDENT(x)` (identificador)
- `ASSIGN` (símbolo `=`)
- `NUMBER(42)` (literal numérico)
- `SEMICOLON` (símbolo `;`)
- `IDENT(print)` (identificador)
- `LPAREN` (símbolo `(`)
- `IDENT(x)` (identificador)
- `PLUS` (símbolo `+`)
- `NUMBER(1)` (literal numérico)
- `RPAREN` (símbolo `)`)
- `SEMICOLON` (símbolo `;`)

Cada token incluye información sobre su tipo, valor y posición en el código fuente.

## Relación con el Visitor Pattern

Aunque el análisis léxico no utiliza directamente el patrón visitor definido en `visitor.hpp`, la construcción de nodos del AST en esta etapa prepara el terreno para que, en fases posteriores, los visitantes puedan recorrer y procesar el árbol sintáctico de manera eficiente y extensible.

---

# Análisis Sintáctico en el Proyecto "hulk"

El análisis sintáctico es la segunda gran etapa del proceso de compilación. Su objetivo es transformar la secuencia de tokens producida por el análisis léxico en una estructura jerárquica que refleje la lógica y organización del programa: el **Árbol de Sintaxis Abstracta (AST)**. Esta etapa es fundamental para validar la estructura del código fuente y preparar la información necesaria para el análisis semántico y la generación de código.

## Definición de la Gramática y Parsing

El parser está implementado utilizando Bison, y la gramática formal del lenguaje se define en el archivo `parser.y`. En este archivo se especifican:

- **Tokens y Tipos**: Se declaran los tokens que el lexer puede producir (palabras clave, operadores, identificadores, literales, etc.), así como los tipos de datos asociados a cada uno.
- **Reglas de Producción**: Cada regla describe una construcción válida del lenguaje. Por ejemplo, cómo se define una función, cómo se declara una variable, cómo se estructura un bloque de código, etc. Estas reglas permiten reconocer expresiones, declaraciones, bloques, tipos y más.
- **Precedencia y Asociatividad**: Se establecen reglas para resolver ambigüedades en expresiones, como la precedencia de operadores aritméticos y lógicos.

### Ejemplo de Regla Gramatical

Por ejemplo, una regla para una función puede verse así:

```bison
stmt:
    FUNCTION IDENT LPAREN param_list RPAREN LBRACE stmt_list RBRACE {
        // Construcción del nodo FunctionDecl
    }
```

Esto indica que una función debe comenzar con la palabra clave `function`, seguida de un identificador (nombre), una lista de parámetros entre paréntesis y un bloque de instrucciones entre llaves.

## Construcción del Árbol de Sintaxis Abstracta (AST)

Cada vez que una regla gramatical se satisface, el parser crea un nodo correspondiente en el AST. El archivo `ast.hpp` define todas las clases de nodos posibles, como:

- **Program**: Nodo raíz que contiene todas las declaraciones y sentencias del programa.
- **FunctionDecl**: Representa la declaración de una función, incluyendo su nombre, parámetros, tipo de retorno y cuerpo.
- **LetExpr**: Para declaraciones de variables locales.
- **BinaryExpr** y **UnaryExpr**: Para operaciones aritméticas y lógicas.
- **IfExpr**, **WhileExpr**, **ForExpr**: Para estructuras de control de flujo.
- **ExprBlock**: Para bloques de instrucciones.
- **TypeDecl**, **AttributeDecl**, **MethodDecl**: Para la definición de tipos, atributos y métodos.

Cada nodo del AST puede contener otros nodos como hijos, reflejando la estructura jerárquica y anidada del código fuente. Esta representación es esencial para el análisis semántico posterior y la generación de código.

## Manejo de Errores Sintácticos

El parser está equipado para detectar y reportar errores sintácticos de manera precisa. Si la secuencia de tokens no se ajusta a ninguna regla de la gramática, se invoca una función de manejo de errores que informa al usuario sobre la naturaleza del error, el token problemático y su ubicación exacta (línea y columna). Esto es posible gracias a la integración de la información de ubicación proporcionada por el lexer y gestionada en las reglas de Bison.

## Integración y Estado Global

Al finalizar el análisis sintáctico, el AST completo se almacena en una variable global llamada `rootAST`, definida en `parser_globals.cpp`. Esto permite que las etapas posteriores del compilador (análisis semántico, generación de código, etc.) accedan fácilmente a la representación estructurada del programa.

## Ejemplo de Flujo Sintáctico

Supongamos que el usuario escribe el siguiente código en "hulk":

```hulk
function suma(a: Number, b: Number): Number {
    let resultado = a + b in
    resultado;
}
```

El proceso sería el siguiente:

1. El lexer convierte el texto en tokens: `FUNCTION`, `IDENT(suma)`, `LPAREN`, `IDENT(a)`, `COLON`, `IDENT(Number)`, etc.
2. El parser aplica las reglas de la gramática y reconoce que se trata de una declaración de función.
3. Se construye un nodo `FunctionDecl` en el AST, con nodos hijos para los parámetros, el tipo de retorno y el cuerpo de la función.
4. El cuerpo contiene un nodo `LetExpr` para la declaración de la variable `resultado`, y un nodo `BinaryExpr` para la suma `a + b`.
5. El AST completo se almacena en `rootAST` para su posterior análisis.

## Relación con el Visitor Pattern

Aunque el parser se encarga de construir el AST, el recorrido y procesamiento posterior del árbol (por ejemplo, para el análisis semántico o la generación de código) se realiza mediante el patrón visitor, definido en `visitor.hpp`. Esto permite separar la lógica de procesamiento de la estructura de datos, facilitando la extensibilidad y el mantenimiento del compilador.

---

# Chequeo Semántico en "hulk": Proceso Detallado

El chequeo semántico es la etapa del compilador que valida que el programa, además de ser sintácticamente correcto, tenga sentido lógico y de tipos. Aquí se detectan errores como el uso de variables no declaradas, incompatibilidades de tipos, redefiniciones indebidas, errores en la herencia, y más. Es el "supervisor" que asegura que el programa pueda ejecutarse sin contradicciones internas.

## Estructuras y Componentes Clave

- **Árbol de Sintaxis Abstracta (AST) (`ast.hpp`)**: El AST es la representación jerárquica del programa. Cada nodo representa una construcción del lenguaje (expresión, declaración, bloque, etc.). El chequeo semántico recorre este árbol para analizar cada parte del programa.
- **Tabla de Símbolos (`symbol_table.hpp`)**: Estructura que almacena información sobre variables, funciones y tipos, junto con su ámbito (scope). Permite declarar, buscar y actualizar símbolos, y es esencial para validar el uso correcto de nombres y tipos.
- **Manejo de Ámbitos (`scope.hpp`)**: Permite que variables y funciones tengan visibilidad limitada a ciertas regiones del código, evitando conflictos y permitiendo la reutilización de nombres en diferentes contextos.
- **Sistema de Tipos y Herencia (`type_info.hpp`, `type_info.cpp`)**: Define los tipos primitivos y de usuario, y gestiona las relaciones de herencia y conformidad entre ellos. Permite determinar si un tipo es subtipo de otro, si dos tipos son compatibles, y realizar inferencia de tipos.
- **Manejo de Errores Semánticos (`error.hpp`)**: El sistema de errores semánticos recopila y reporta todos los problemas detectados durante el análisis, proporcionando mensajes claros y precisos con información de contexto, línea y columna.
- **Analizador Semántico (`semantic_checker.hpp`, `semantic_checker.cpp`)**: Implementa el recorrido del AST y la lógica de chequeo, utilizando el patrón visitor para procesar cada tipo de nodo.

## Proceso Paso a Paso

### 1. Inicialización y Registro de Built-ins
Antes de analizar el código del usuario, el compilador registra funciones y tipos incorporados (por ejemplo, `print`, `sin`, `cos`, `Number`, `String`, etc.) en la tabla de símbolos. Esto asegura que estén disponibles para su uso y evita errores por ausencia de definiciones básicas.

### 2. Primera Pasada: Declaración de Funciones y Tipos
El analizador recorre el AST y registra todas las funciones y tipos definidos por el usuario. Esto permite que puedan ser referenciados incluso antes de su definición textual en el código, soportando la recursividad y referencias cruzadas.

### 3. Segunda Pasada: Análisis Profundo y Chequeo de Tipos
En esta fase, el analizador recorre nuevamente el AST, esta vez validando en detalle cada expresión, declaración y bloque:
- **Variables**: Verifica que toda variable utilizada haya sido declarada en el scope correspondiente. Si no es así, se reporta un error de variable no definida.
- **Funciones**: Comprueba que las llamadas a funciones correspondan a funciones declaradas, que el número y tipo de argumentos coincidan con la firma, y que el valor de retorno sea compatible con lo esperado.
- **Tipos**: Valida que los tipos utilizados existan, que las asignaciones sean compatibles, y que las operaciones entre tipos sean válidas.
- **Herencia**: Verifica que las relaciones de herencia sean correctas (sin ciclos, sin heredar de tipos primitivos, etc.) y que los métodos redefinidos sean compatibles en firma y tipo de retorno.
- **Ámbitos**: Gestiona la entrada y salida de scopes para asegurar la correcta visibilidad y vida útil de variables y funciones.

### 4. Inferencia y Propagación de Tipos
El sistema es capaz de inferir tipos en muchos contextos. Por ejemplo, si un parámetro de función no tiene tipo declarado pero se utiliza en una operación aritmética, el analizador puede inferir que debe ser de tipo `Number` y actualizar la información en la tabla de símbolos. Esta inferencia se propaga a lo largo del AST, permitiendo una mayor flexibilidad y robustez en la escritura de código.

### 5. Reporte de Errores
Si se detecta un error semántico, se registra un error detallado, incluyendo el tipo de error, mensaje descriptivo, línea, columna y contexto. El sistema puede limitar la cantidad de errores reportados para evitar saturar al usuario con mensajes redundantes.

---

## Ejemplo Narrativo: Análisis de la Función `suma`

Supongamos que el usuario ha escrito la siguiente función en "hulk":

```hulk
function suma(a: Number, b: Number): Number {
    let resultado = a + b in
    resultado;
}
```

**Paso a paso del chequeo semántico:**

1. **Declaración de la función:**
   El analizador detecta la declaración de la función `suma`, registra su nombre, parámetros y tipo de retorno en la tabla de símbolos.
2. **Entrada al ámbito de la función:**
   Se crea un nuevo scope para los parámetros y variables locales de la función.
3. **Declaración de parámetros:**
   Los parámetros `a` y `b` se declaran en el scope actual, ambos con tipo `Number`.
4. **Análisis de la expresión `let resultado = a + b in resultado;`:**
   - Se analiza la expresión `a + b`. El analizador verifica que ambos operandos (`a` y `b`) existen y son de tipo `Number`.
   - Se crea un nodo `LetExpr` para la variable `resultado`, que se inicializa con el resultado de la suma.
   - El tipo de `resultado` se infiere como `Number` y se declara en el scope local.
   - Se verifica que la expresión final del bloque (`resultado`) también es de tipo `Number`.
5. **Verificación del tipo de retorno:**
   El analizador comprueba que el valor retornado por la función coincide con el tipo declarado (`Number`). Si todo es correcto, la función pasa el chequeo semántico.
6. **Salida del ámbito:**
   Al finalizar el análisis de la función, se elimina el scope local, preservando solo la información relevante en la tabla de símbolos global.

---

## Ejemplo de Detección de Errores

### a) Variable no declarada

Si el usuario hubiera escrito:

```hulk
function suma(a: Number, b: Number): Number {
    let resultado = a + c in
    resultado;
}
```

El analizador semántico detectaría que la variable `c` no está declarada en el ámbito de la función, y reportaría un error del tipo `UNDEFINED_VARIABLE`, indicando la línea, columna y contexto del error.

### b) Incompatibilidad de tipos

Si el usuario intenta sumar un número y una cadena:

```hulk
function suma(a: Number, b: String): Number {
    let resultado = a + b in
    resultado;
}
```

El analizador detectaría que la operación `a + b` no es válida entre un `Number` y un `String`, y reportaría un error de tipo (`TYPE_ERROR`).

### c) Redefinición de variable

Si dentro de un mismo scope se declara dos veces la misma variable:

```hulk
let x = 5 in
let x = 10 in
x;
```

El sistema reportaría un error de redefinición de variable (`REDEFINED_VARIABLE`).

### d) Herencia inválida

Si se intenta heredar de un tipo primitivo:

```hulk
type MiTipo inherits Number { }
```

El analizador reportaría un error de herencia inválida (`INVALID_BASE_TYPE`).

---

## Inferencia y Propagación de Tipos: Ejemplo Detallado

Supón que el usuario escribe:

```hulk
function identidad(x) {
    x;
}
```

Aquí, el parámetro `x` no tiene tipo declarado. El analizador semántico observa cómo se utiliza `x` en el cuerpo de la función. Si más adelante se llama a `identidad(5)`, el sistema puede inferir que `x` debe ser de tipo `Number`. Esta inferencia se propaga hacia atrás, actualizando la información de tipo de `x` en la tabla de símbolos y en el AST.

Si la función se utiliza con diferentes tipos en distintos contextos, el sistema puede inferir el tipo más general posible (por ejemplo, `Object`), o reportar un error si la sobrecarga no es permitida.

---

## Herencia y Polimorfismo: Ejemplo Detallado

Supón que el usuario define dos tipos:

```hulk
type Animal {
    nombre: String = "";
    function hablar() => "";
}

type Perro inherits Animal {
    function hablar() => "Guau";
}
```

El analizador semántico valida que:
- `Perro` hereda correctamente de `Animal`.
- El método `hablar` en `Perro` tiene la misma firma que en `Animal` (polimorfismo).
- No existen redefiniciones incompatibles ni ciclos de herencia.

Si el usuario intentara redefinir `hablar` con una firma diferente, el sistema reportaría un error de redefinición de método (`REDEFINED_METHOD`).

---

## Manejo de Ámbitos (Scopes)

Cada vez que se entra en una función, bloque, o tipo, se crea un nuevo scope. Esto permite que variables locales no interfieran con variables globales o de otros bloques. Al salir del contexto, el scope se elimina, asegurando la correcta visibilidad y vida útil de las variables.

---

## Reporte de Errores

Todos los errores semánticos se recopilan y se reportan al usuario con mensajes claros, indicando el tipo de error, la línea y columna donde ocurrió, y el contexto. Esto facilita la depuración y mejora la experiencia de desarrollo.
