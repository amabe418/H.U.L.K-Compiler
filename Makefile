.PHONY: all compile execute clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. -Itheoretical/Lexer -Itheoretical/Parser -IPrintVisitor -IAST -Itheoretical/automata -IValue -ISymbolTable -IEvaluator -ISemanticCheck -IScope -ITypes -ICodegen
LDFLAGS =

# Archivos fuente
SOURCES = main_ll1.cpp theoretical/Parser/ll1_parser.cpp theoretical/Lexer/theoretical_lexer.cpp theoretical/token.cpp theoretical/automata/dfa.cpp theoretical/automata/nfa.cpp theoretical/automata/nfa_to_dfa.cpp Types/type_info.cpp SemanticCheck/semantic_checker.cpp Codegen/codegen.cpp Codegen/builtin.cpp
TARGET = main_ll1

# Generador y gramática
GENERATOR = Parser/ll1_generator
GENERATOR_SRC = Parser/ll1_generator.cpp
GRAMMAR = Parser/grammar.ll1
GENERATED = Parser/ll1_parser_generated.cpp

FILE ?= script.hulk

all: compile

$(GENERATOR): $(GENERATOR_SRC)
	$(CXX) $(CXXFLAGS) $(GENERATOR_SRC) -o $(GENERATOR)

$(GENERATED): $(GENERATOR) $(GRAMMAR)
	$(GENERATOR) $(GRAMMAR)
	@if [ -f ll1_parser_generated.cpp ]; then mv ll1_parser_generated.cpp $(GENERATED); fi
	@if [ -f ll1_parser_generated.hpp ]; then mv ll1_parser_generated.hpp Parser/ll1_parser_generated.hpp; fi

compile: $(GENERATED)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(GENERATED) -o $(TARGET) $(LDFLAGS)

execute: compile
	@./$(TARGET) $(FILE)

clean:
	rm -f $(TARGET) $(GENERATOR) $(GENERATED) *.o theoretical/Parser/*.o theoretical/Lexer/*.o theoretical/automata/*.o Codegen/*.o ll1_parser_generated.cpp ll1_parser_generated.hpp 