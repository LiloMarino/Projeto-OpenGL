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

struct Vertex
{
    float x, y, z;    ///< Posição do vértice.
    float u, v;       ///< Coordenadas de textura.
    int boneIDs[4];   ///< IDs dos bones que influenciam este vértice (suporta até 4).
    float weights[4]; ///< Pesos correspondentes a cada bone.
};

struct SubMesh
{
    std::vector<Vertex> vertices; ///< Lista de vértices do submesh.
    GLuint textureID;             ///< ID da textura associada ao submesh.
};

struct BoneInfo
{
    aiMatrix4x4 offsetMatrix;          ///< Matriz de transformação do bone para a pose inicial.
    aiMatrix4x4 defaultLocalTransform; ///< Transformação local do bone na bind pose.
    aiMatrix4x4 manualRotation;        ///< Rotação manual aplicada ao bone.
    aiMatrix4x4 finalTransformation;   ///< Transformação final aplicada aos vértices.
    int parentIndex;                   ///< Índice do bone pai (-1 se for raiz).
};

class Character3D
{
private:
    std::vector<SubMesh> submeshes;           ///< Lista de submeshes do modelo.
    std::map<std::string, GLuint> textureMap; ///< Cache de texturas carregadas.
    std::map<std::string, int> boneMapping;   ///< Mapeia o nome do bone para seu índice.
    std::vector<BoneInfo> boneInfo;           ///< Lista de informações de cada bone.

public:
    /**
     * @brief Construtor da classe Character3D.
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

    /**
     * @brief Rotaciona um bone utilizando um quaternion.
     * @param boneName Nome do bone a ser rotacionado.
     * @param rotation Rotação representada como um quaternion glm::quat.
     */
    void rotateBone(const std::string &boneName, const glm::quat &rotation);

private:
    /**
     * @brief Carrega uma textura e retorna seu ID no OpenGL.
     * @param path Caminho da textura.
     * @return GLuint ID da textura carregada.
     */
    GLuint loadTexture(const std::string &path);

    /**
     * @brief Atualiza as transformações dos bones com base na hierarquia.
     */
    void updateBoneTransforms();

    /**
     * @brief Lê a hierarquia de bones do modelo e armazena suas transformações.
     * @param node Nó da cena do Assimp a ser processado.
     * @param parentTransform Transformação do nó pai.
     * @param parentBoneIndex Índice do bone pai (-1 se não houver).
     */
    void readHierarchy(const aiNode *node, const aiMatrix4x4 &parentTransform, int parentBoneIndex);

    /**
     * @brief Computa a transformação global de um bone específico.
     * @param boneIndex Índice do bone.
     * @return aiMatrix4x4 Matriz de transformação global.
     */
    aiMatrix4x4 computeGlobalTransform(int boneIndex) const;
};

#endif
