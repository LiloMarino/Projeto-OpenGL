# 📦 Dependências

Para compilar e executar este projeto, você precisa das seguintes bibliotecas:

- **GLFW** (para gerenciamento de janelas e entrada)
- **Assimp** (para carregamento de modelos 3D)
- **OpenGL** (biblioteca gráfica)

## 🛠️ Instalação das Dependências  

### 🔹 **Linux (Ubuntu/Debian)**
Execute o seguinte comando no terminal:
```bash
sudo apt update
sudo apt install -y libglfw3-dev libassimp-dev libgl1-mesa-dev
```
ou
```bash
make install_deps
```

### 🍏 **macOS (Homebrew)**
Se você usa **Homebrew**, instale as dependências com:
```bash
brew install glfw assimp
```

### 🖥️ **Windows (vcpkg)**
Para Windows, usando **vcpkg**, instale com:
```powershell
vcpkg install glfw3 assimp
```

## 🚀 Compilação e Execução

Depois de instalar as dependências, compile e rode o projeto com:
```bash
make
make run
```