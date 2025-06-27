CXX = g++
CXXFLAGS = -std=c++17 -I./include -Wall -g

# Fuente y objetos
SRC = $(wildcard Lexer/*.cpp Parser/*.cpp Semantic/*.cpp CodeGen/*.cpp common/*.cpp)
OBJ = $(SRC:.cpp=.o)

# Salidas
BIN = hulk/hulk.exe
LLFILE = hulk/output.ll

all: compile

# Compilar el ejecutable
$(BIN): main.cpp $(OBJ)
	@mkdir -p hulk
	$(CXX) $(CXXFLAGS) main.cpp $(OBJ) -o $(BIN) `llvm-config --cxxflags --ldflags --libs core` -Wl,-subsystem,console

# Generar el archivo LLVM IR
compile: $(BIN)
	@echo "Generando archivo LLVM IR: $(LLFILE)"
	@mkdir -p hulk
	$(BIN)

# Ejecutar el IR
execute: $(LLFILE)
	@echo "Ejecutando LLVM IR: $(LLFILE)"
	lli $(LLFILE)

# Verifica que el IR exista antes de intentar ejecutarlo
$(LLFILE): $(BIN)
	@if [ -f $(LLFILE) ]; then \
		echo "Ejecutando LLVM IR: $(LLFILE)"; \
		lli $(LLFILE); \
	else \
		echo "ERROR: No se generó $(LLFILE). No se ejecutará nada."; \
		exit 1; \
	fi

# Limpiar
clean:
	rm -f Lexer/*.o Parser/*.o Semantic/*.o CodeGen/*.o common/*.o
	rm -f hulk/*.exe hulk/*.ll
