# Nome do executável
EXEC_NAME = program

# Nome do zip
ZIP_NAME = Teste

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
THIRD_PARTY_DIR = third_party

# Arquivos do projeto principal
SRC_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
SRC_OBJECTS = $(SRC_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Arquivo do GLAD
GLAD_SOURCE = $(THIRD_PARTY_DIR)/glad.c
GLAD_OBJECT = $(BUILD_DIR)/glad.o

# Definir o compilador
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -I$(THIRD_PARTY_DIR) -I$(INCLUDE_DIR)

# Flags para OpenGL e GLFW
LDFLAGS = -lGL -lGLU -lglfw

# Alvo padrão
all: $(EXEC_NAME)

# Regra de compilação do executável
$(EXEC_NAME): $(LIB_OBJECTS) $(SRC_OBJECTS) $(GLAD_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regras para compilar arquivos .cpp em .o
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para compilar glad.c
$(BUILD_DIR)/glad.o: $(GLAD_SOURCE)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Instalar dependências automaticamente
install_deps:
	sudo apt update
	sudo apt install -y libglfw3-dev libassimp-dev libgl1-mesa-dev

# Alvo para limpar os arquivos gerados
clean:
	rm -rf $(BUILD_DIR) $(EXEC_NAME)

# Alvo para executar o programa
run: all
	./$(EXEC_NAME)

# Alvo para criar um zip com os arquivos fonte
zip:
	zip -r $(ZIP_NAME).zip $(SRC_DIR) $(LIB_DIR) $(THIRD_PARTY_DIR) $(INCLUDE_DIR) Makefile README.md
	mkdir -p $(ZIP_NAME)
	unzip -q $(ZIP_NAME).zip -d $(ZIP_NAME)

.PHONY: all clean run zip
