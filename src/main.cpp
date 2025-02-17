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

#define DEG2RAD 3.14159265358979323846 / 180.0

void calculateDirection(double rotX, double rotY, double rotZ, double &dirX, double &dirY, double &dirZ)
{
    // Converter para radianos
    double yaw = rotY * DEG2RAD;   // Rotação ao redor do eixo Y (horizontal)
    double pitch = rotX * DEG2RAD; // Rotação ao redor do eixo X (vertical)

    // Cálculo da direção a partir dos ângulos
    dirX = cos(yaw) * cos(pitch);
    dirY = sin(pitch);
    dirZ = sin(yaw) * cos(pitch);
}

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
void display(GLFWwindow *window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Alterar o FOV para 39.6° (calculado anteriormente)
    gluPerspective(39.6, 800.0 / 600.0, 0.1, 1000.0);  // FOV alterado de 50.0 para 39.6

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posição da câmera no Blender
    double Blender_X = 0, Blender_Y = -9.0, Blender_Z = 16.0;

    // Rotação da câmera no Blender (em graus)
    double rotX = 90.0, rotY = 0.0, rotZ = 0.0;

    // Converter coordenadas do Blender para OpenGL
    double eyeX = Blender_X;
    double eyeY = Blender_Y;  // Z do Blender vira Y no OpenGL
    double eyeZ = Blender_Z; // Y do Blender vira -Z no OpenGL

    // Calcular direção da câmera
    double dirX, dirY, dirZ;
    calculateDirection(rotX, rotY, rotZ, dirX, dirY, dirZ);

    // Definir ponto para onde a câmera olha
    double centerX = 0;
    double centerY = eyeZ + dirY;
    double centerZ = -eyeY + dirZ;

    // Vetor "para cima" (normalmente fixo)
    double upX = 0.0, upY = 1.0, upZ = 0.0;

    // Aplicar no OpenGL
    gluLookAt(0, -9, 16, centerX, centerY, centerZ, upX, upY, upZ);

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

    if (!loadModel("Mita/Mita (orig).fbx", "Mita"))
    {
        return -1;
    }

    init();

    while (!glfwWindowShouldClose(window))
    {
        display(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
