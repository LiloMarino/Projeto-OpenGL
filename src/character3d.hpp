#ifndef CHARACTER3D_HPP
#define CHARACTER3D_HPP

#include <vector>
#include <map>
#include <string>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/**
 * @brief Estrutura para armazenar vértices, coordenadas de textura e informações de skinning.
 */
struct Vertex
{
    float x, y, z;
    float u, v;
    // IDs dos bones que influenciam este vértice (suporta até 4)
    int boneIDs[4];
    // Pesos correspondentes a cada bone
    float weights[4];
};

/**
 * @brief Estrutura para armazenar um submesh com seus vértices e textura.
 */
struct SubMesh
{
    std::vector<Vertex> vertices;
    GLuint textureID;
};

/**
 * @brief Estrutura para armazenar os dados de um bone.
 *
 * Foram adicionados:
 * - defaultLocalTransform: a transformação local (bind pose) do nó correspondente;
 * - manualRotation: rotação manual aplicada (inicialmente identidade);
 * - parentIndex: índice do osso pai (-1 se não houver);
 * - finalTransformation: transformação final que será aplicada aos vértices.
 */
struct BoneInfo
{
    aiMatrix4x4 offsetMatrix;          
    aiMatrix4x4 defaultLocalTransform; // transform local (bind pose) extraída do nó
    aiMatrix4x4 manualRotation;        // rotação manual (inicialmente identidade)
    aiMatrix4x4 finalTransformation;   // transformação final: (globalTransform * offsetMatrix)
    int parentIndex;                   // índice do osso pai (-1 se for raiz)
};

class Character3D
{
private:
    std::vector<SubMesh> submeshes;           ///< Lista de submeshes do modelo.
    std::map<std::string, GLuint> textureMap; ///< Cache de texturas.

    // Dados dos bones
    std::map<std::string, int> boneMapping;   ///< Mapeia o nome do bone para seu índice.
    std::vector<BoneInfo> boneInfo;             ///< Lista de informações de cada bone.

public:
    /**
     * @brief Construtor da classe.
     */
    Character3D();

    /**
     * @brief Carrega um modelo 3D e suas texturas associadas, além de dados de bones.
     * @param path Caminho do modelo 3D.
     * @param textureDir Diretório onde as texturas estão armazenadas.
     * @return true se o modelo for carregado com sucesso, false caso contrário.
     */
    bool loadModel(const std::string &path, const std::string &textureDir);

    /**
     * @brief Renderiza o modelo na cena aplicando as transformações dos bones.
     */
    void draw() const;

    /**
     * @brief Rotaciona um bone especificado.
     * @param boneName Nome do bone a ser rotacionado.
     * @param angle Ângulo (em graus) da rotação.
     * @param axisX Componente X do eixo.
     * @param axisY Componente Y do eixo.
     * @param axisZ Componente Z do eixo.
     */
    void rotateBone(const std::string &boneName, float angle, float axisX, float axisY, float axisZ);

    void rotateBone(const std::string& boneName, const glm::quat& rotation);

private:
    /**
     * @brief Carrega uma textura e retorna seu ID no OpenGL.
     * @param path Caminho da textura.
     * @return GLuint ID da textura.
     */
    GLuint loadTexture(const std::string &path);

    // Funções auxiliares para atualização da hierarquia dos bones
    void updateBoneTransforms();
    void readHierarchy(const aiNode* node, const aiMatrix4x4 &parentTransform, int parentBoneIndex);
    aiMatrix4x4 computeGlobalTransform(int boneIndex) const;
};

#endif
