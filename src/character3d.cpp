#include "character3d.hpp"
#include "stb_image.h"
#include <iostream>

Character3D::Character3D()
{
    submeshes.clear();
    textureMap.clear();
}

GLuint Character3D::loadTexture(const std::string &path)
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

bool Character3D::loadModel(const std::string &path, const std::string &textureDir)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Erro ao carregar modelo: " << importer.GetErrorString() << std::endl;
        return false;
    }

    submeshes.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;
        GLuint texID = 0;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string fullTexturePath = textureDir + "/" + texturePath.C_Str();
            if (textureMap.find(fullTexturePath) == textureMap.end())
            {
                textureMap[fullTexturePath] = loadTexture(fullTexturePath);
            }
            texID = textureMap[fullTexturePath];
            std::cout << "Carregando textura: " << fullTexturePath << std::endl;
        }

        SubMesh submesh;
        submesh.textureID = texID;

        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vert;
            vert.x = mesh->mVertices[v].x;
            vert.y = mesh->mVertices[v].y;
            vert.z = mesh->mVertices[v].z;

            if (mesh->mTextureCoords[0])
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

        submeshes.push_back(submesh);
    }

    return true;
}

void Character3D::draw() const
{
    for (const auto &sub : submeshes)
    {
        glBindTexture(GL_TEXTURE_2D, sub.textureID);

        glBegin(GL_TRIANGLES);
        for (const auto &vert : sub.vertices)
        {
            glTexCoord2f(vert.u, vert.v);
            glVertex3f(vert.x, vert.y, vert.z);
        }
        glEnd();
    }
}
