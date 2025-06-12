# Makefile para o projeto C++ com SQLite

# Compiladores
CC = gcc
CXX = g++

# Flags de compilação
CFLAGS = -Wall -DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_JSON1
CXXFLAGS = -Wall -std=c++11
LDFLAGS = 

# Diretórios
SQLITE_SRC = sqlite/src
SQLITE_LIB = sqlite/lib
BUILD_DIR = build

# Arquivos
SQLITE_OBJ = $(BUILD_DIR)/sqlite3.o
ALUNO_OBJ = $(BUILD_DIR)/Aluno.o
MAIN_OBJ = $(BUILD_DIR)/main.o

# Executável final
TARGET = sistema

# Regra padrão
all: $(TARGET)

# Criar diretório de build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compilar SQLite (como C)
$(SQLITE_OBJ): $(SQLITE_SRC)/sqlite3.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar Aluno.cpp
$(ALUNO_OBJ): Aluno.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SQLITE_LIB) -c $< -o $@

# Compilar main.cpp (se existir)
$(MAIN_OBJ): main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SQLITE_LIB) -c $< -o $@

# Linkar tudo
$(TARGET): $(SQLITE_OBJ) $(ALUNO_OBJ) $(MAIN_OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

# Compilar apenas o SQLite
sqlite: $(SQLITE_OBJ)

# Compilar apenas Aluno.cpp
aluno: $(ALUNO_OBJ)

# Limpar arquivos de build
clean:
	rm -rf $(BUILD_DIR) $(TARGET).exe

# Executar o programa
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run sqlite aluno 