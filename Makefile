.PHONY: all compile execute clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -Itheoretical/Lexer -Itheoretical/Parser -IPrintVisitor -IAST -Itheoretical/automata -IValue -ISymbolTable -IEvaluator -ISemanticCheck -IScope -ITypes -ICodegen
LDFLAGS =

# Archivos fuente
SOURCES = main_ll1.cpp theoretical/Lexer/theoretical_lexer.cpp theoretical/token.cpp theoretical/automata/dfa.cpp theoretical/automata/nfa.cpp theoretical/automata/nfa_to_dfa.cpp Types/type_info.cpp SemanticCheck/semantic_checker.cpp Codegen/codegen.cpp Codegen/builtin.cpp Parser/ll1_parser.cpp Parser/cst_to_ast.cpp
TARGET = main_ll1

# Gramática
GRAMMAR = Parser/grammar.ll1

FILE ?= script.hulk

all: compile

compile:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

execute: compile
	@./$(TARGET) $(FILE)

clean:
	rm -f $(TARGET) *.o theoretical/Lexer/*.o theoretical/automata/*.o Codegen/*.o Parser/*.o 