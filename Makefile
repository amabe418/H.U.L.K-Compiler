# Existing variables
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = hulk

# LLVM Configuration
LLVM_VERSION = 19
LLVM_CONFIG = llvm-config
LLVM_CXXFLAGS = $(shell $(LLVM_CONFIG) --cflags)
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags)
LLVM_LIBS = $(shell $(LLVM_CONFIG) --libs core support irreader ipo bitwriter passes)

# Archivos fuente del parser y lexer
PARSER_Y = $(SRC_DIR)/Parser/parser.y
PARSER_CPP = $(SRC_DIR)/Parser/parser.tab.cpp
PARSER_HPP = $(SRC_DIR)/Parser/parser.tab.hpp

LEXER_L = $(SRC_DIR)/Lexer/lexer.l
LEXER_CPP = $(SRC_DIR)/Lexer/lex.yy.cpp

# Generados por Bison y Flex
$(PARSER_CPP): $(PARSER_Y)
	bison -d -o $(PARSER_CPP) $(PARSER_Y)

$(LEXER_CPP): $(LEXER_L)
	flex -o $(LEXER_CPP) $(LEXER_L)


# Otros archivos fuente
OTHER_SRCS = $(wildcard $(SRC_DIR)/AST/*.cpp) \
             $(wildcard $(SRC_DIR)/NameResolver/*.cpp) \
             $(wildcard $(SRC_DIR)/Scope/*.cpp) \
             $(wildcard $(SRC_DIR)/SymbolTable/*.cpp) \
             $(wildcard $(SRC_DIR)/Types/*.cpp) \
             $(wildcard $(SRC_DIR)/SemanticCheck/*.cpp) \
             $(wildcard $(SRC_DIR)/Codegen/*.cpp) \
             $(wildcard $(SRC_DIR)/Runtime/*.cpp) \
             $(SRC_DIR)/Parser/parser_globals.cpp \
             $(SRC_DIR)/main.cpp

# Todos los archivos fuente
SRCS = $(PARSER_CPP) $(LEXER_CPP) $(OTHER_SRCS)

# Objetos
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I$(SRC_DIR) -DYYDEBUG=1 -fexceptions $(LLVM_CXXFLAGS)

TARGET = $(BIN_DIR)/hulk_executable
INPUT_FILE ?= script.hulk

.PHONY: all clean compile execute debug llvm-check test-llvm test-llvm-basic

# Check LLVM installation
llvm-check:
	@echo "=== LLVM Configuration ==="
	@echo "LLVM Version: $(shell $(LLVM_CONFIG) --version)"
	@echo "LLVM CXXFLAGS: $(LLVM_CXXFLAGS)"
	@echo "LLVM LDFLAGS: $(LLVM_LDFLAGS)"
	@echo "LLVM LIBS: $(LLVM_LIBS)"
	@echo "LLVM Components: $(shell $(LLVM_CONFIG) --components | head -c 100)..."
	@echo "=== End LLVM Configuration ==="

all: $(TARGET)

# Compilar binario final
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LLVM_LDFLAGS) $(LLVM_LIBS)

# Compilar archivos .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile: $(PARSER_CPP) $(LEXER_CPP) $(TARGET)

execute: compile
	./$(TARGET) $(INPUT_FILE)

debug: compile
	@echo "=== DEBUGGING PARSER ==="
	./$(TARGET) $(INPUT_FILE)

# Test LLVM compilation
test-llvm: test_llvm.cpp
	$(CXX) $(CXXFLAGS) $< -o test_llvm $(LLVM_LDFLAGS) $(LLVM_LIBS)
	@echo "LLVM test compiled successfully!"
	./test_llvm

# Test LLVM basic expressions
test-llvm-basic: test_llvm_basic.cpp src/Codegen/llvm_codegen.cpp
	$(CXX) $(CXXFLAGS) $^ -o test_llvm_basic $(LLVM_LDFLAGS) $(LLVM_LIBS)
	@echo "LLVM basic expressions test compiled successfully!"
	./test_llvm_basic

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) \
	       $(PARSER_CPP) $(PARSER_HPP) \
	       $(LEXER_CPP) \
		   script.hulk.ll \
		   

