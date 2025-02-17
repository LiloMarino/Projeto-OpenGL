#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

// Estrutura para armazenar vértices e texturas
struct Vertex {
    float x, y, z;
    float u, v;
};

// Lista de vértices e texturas
std::vector<Vertex> vertices;
std::map<std::string, GLuint> textureMap; // Armazena as texturas carregadas

// Função para carregar uma textura e criar um ID no OpenGL
GLuint loadTexture(const std::string& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    
    if (!data) {
        std::cerr << "Erro ao carregar textura: " << path << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

// Função para carregar modelo e texturas
bool loadModel(const std::string& path, const std::string& textureDir) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Erro ao carregar modelo: " << importer.GetErrorString() << std::endl;
        return false;
    }

    aiMesh* mesh = scene->mMeshes[0];
    std::cout << "Modelo carregado com " << mesh->mNumVertices << " vértices." << std::endl;

    // Verificar textura associada ao material
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString texturePath;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
        std::string fullTexturePath = textureDir + "/" + texturePath.C_Str();
        std::cout << "Carregando textura: " << fullTexturePath << std::endl;
        textureMap[fullTexturePath] = loadTexture(fullTexturePath);
    }

    // Processar vértices e coordenadas UV
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;

        if (mesh->mTextureCoords[0]) { // Se há coordenadas de textura
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;
        } else {
            v.u = v.v = 0.0f; // Valor padrão caso não haja textura
        }

        vertices.push_back(v);
    }

    return true;
}

// Inicializar OpenGL
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

// Renderizar modelo
void drawModel() {
    if (!vertices.empty()) {
        glBindTexture(GL_TEXTURE_2D, textureMap.begin()->second); // Usa a primeira textura carregada

        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < vertices.size(); i++) {
            glTexCoord2f(vertices[i].u, vertices[i].v);
            glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
        }
        glEnd();
    }
}

// Função de renderização
void display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0/600.0, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    drawModel();

    glfwSwapBuffers(window);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Modelo com Textura", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!loadModel("Mita/Mita (orig).fbx", "Mita")) {
        return -1;
    }

    init();

    while (!glfwWindowShouldClose(window)) {
        display(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
