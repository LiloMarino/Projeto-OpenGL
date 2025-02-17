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
#include <cmath>
#include "camera3d.hpp"

// Estrutura para armazenar vértices e texturas
struct Vertex
{
    float x, y, z;
    float u, v;
};

// Lista de vértices e texturas
std::vector<Vertex> vertices;
std::map<std::string, GLuint> textureMap; // Armazena as texturas carregadas

struct SubMesh
{
    std::vector<Vertex> vertices;
    GLuint textureID;
};

// Vetor para armazenar todos os submeshes
std::vector<SubMesh> submeshes;


// Função para carregar uma textura e criar um ID no OpenGL
GLuint loadTexture(const std::string &path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (!data)
    {
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
bool loadModel(const std::string &path, const std::string &textureDir)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, 
        aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Erro ao carregar modelo: " 
                  << importer.GetErrorString() << std::endl;
        return false;
    }

    // Limpa nosso vector global (se estiver usando um global)
    vertices.clear();
    submeshes.clear();

    // Percorrer TODAS as meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];

        // Pegar o material associado a esta mesh
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        // Verificar se existe textura difusa
        aiString texturePath;
        GLuint texID = 0; // caso não tenha textura
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string fullTexturePath = textureDir + "/" + texturePath.C_Str();
            // Se já carregamos essa textura antes, não carregar de novo
            if (textureMap.find(fullTexturePath) == textureMap.end())
            {
                textureMap[fullTexturePath] = loadTexture(fullTexturePath);
            }
            texID = textureMap[fullTexturePath];
            std::cout << "Carregando textura: " << fullTexturePath << std::endl;
        }

        // Criar SubMesh
        SubMesh submesh;
        submesh.textureID = texID;

        // Processar vértices desta mesh
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vert;
            vert.x = mesh->mVertices[v].x;
            vert.y = mesh->mVertices[v].y;
            vert.z = mesh->mVertices[v].z;

            if (mesh->mTextureCoords[0]) // Se há coords de textura
            {
                vert.u = mesh->mTextureCoords[0][v].x;
                vert.v = mesh->mTextureCoords[0][v].y;
            }
            else
            {
                vert.u = vert.v = 0.0f;
            }

            submesh.vertices.push_back(vert);
        }

        // Adicionar esse submesh ao nosso vector principal
        submeshes.push_back(submesh);
    }

    return true;
}

// Inicializar OpenGL
void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

// Renderizar modelo
void drawModel()
{
    for (const auto &sub : submeshes)
    {
        glBindTexture(GL_TEXTURE_2D, sub.textureID);

        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < sub.vertices.size(); i++)
        {
            glTexCoord2f(sub.vertices[i].u, sub.vertices[i].v);
            glVertex3f(sub.vertices[i].x, sub.vertices[i].y, sub.vertices[i].z);
        }
        glEnd();
    }
}

// Função de renderização
void display(GLFWwindow *window, const Camera3D &camera)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Aplica a câmera
    camera.applyCamera(); // Agora isso cuida de gluPerspective e gluLookAt

    drawModel();

    glfwSwapBuffers(window);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(800, 600, "Modelo com Textura", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializa a câmera
    Camera3D camera(0.0, -9.0, 16.0, 90.0, 0.0, 0.0, 50.0, 36.0, 800.0 / 600.0, 0.1, 1000.0);

    if (!loadModel("Mita/Mita (orig).fbx", "Mita"))
    {
        return -1;
    }

    init();

    while (!glfwWindowShouldClose(window))
    {
        display(window, camera);  // Passe a câmera para o display
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
