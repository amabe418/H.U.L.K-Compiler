.PHONY: all compile execute clean

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -I. -Itheoretical/Lexer -Itheoretical/Parser -IPrintVisitor -IAST -Itheoretical/automata -IValue -ISymbolTable -IEvaluator -ISemanticCheck -IScope -ITypes -ICodegen $(shell llvm-config --cxxflags) -fexceptions
LDFLAGS = $(shell llvm-config --ldflags --libs)

# Archivos fuente
SOURCES = main_ll1.cpp theoretical/Lexer/theoretical_lexer.cpp theoretical/token.cpp theoretical/automata/dfa.cpp theoretical/automata/nfa.cpp theoretical/automata/nfa_to_dfa.cpp Types/type_info.cpp SemanticCheck/semantic_checker.cpp Codegen/llvm_codegen.cpp Codegen/llvm_aux.cpp Parser/ll1_parser.cpp Parser/cst_to_ast.cpp
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