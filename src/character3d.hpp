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

/**
 * @brief Estrutura para armazenar vértices e coordenadas de textura.
 */
struct Vertex
{
    float x, y, z;
    float u, v;
};

/**
 * @brief Estrutura para armazenar um submesh com seus vértices e textura.
 */
struct SubMesh
{
    std::vector<Vertex> vertices;
    GLuint textureID;
};

class Character3D
{
private:
    std::vector<SubMesh> submeshes;           ///< Lista de submeshes do modelo.
    std::map<std::string, GLuint> textureMap; ///< Cache de texturas para evitar recarregamento.

public:
    /**
     * @brief Construtor da classe.
     */
    Character3D();

    /**
     * @brief Carrega um modelo 3D e suas texturas associadas.
     * @param path Caminho do modelo 3D.
     * @param textureDir Diretório onde as texturas estão armazenadas.
     * @return true se o modelo for carregado com sucesso, false caso contrário.
     */
    bool loadModel(const std::string &path, const std::string &textureDir);

    /**
     * @brief Renderiza o modelo na cena.
     */
    void draw() const;

private:
    /**
     * @brief Carrega uma textura e retorna seu ID no OpenGL.
     * @param path Caminho da textura.
     * @return GLuint ID da textura no OpenGL.
     */
    GLuint loadTexture(const std::string &path);
};

#endif
