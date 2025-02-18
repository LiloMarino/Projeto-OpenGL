#include "character3d.hpp"
#include "stb_image.h"
#include <iostream>
#include <cmath>

// Função auxiliar para criar uma matriz de rotação em torno de um eixo (fórmula de Rodrigues)
aiMatrix4x4 createRotationMatrix(float angle, float x, float y, float z)
{
    float rad = angle * 3.14159265f / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    float len = sqrt(x * x + y * y + z * z);
    if (len == 0) len = 1.0f;
    x /= len; y /= len; z /= len;

    aiMatrix4x4 rot;
    rot.a1 = c + (1 - c) * x * x;
    rot.a2 = (1 - c) * x * y - s * z;
    rot.a3 = (1 - c) * x * z + s * y;
    rot.a4 = 0.0f;

    rot.b1 = (1 - c) * y * x + s * z;
    rot.b2 = c + (1 - c) * y * y;
    rot.b3 = (1 - c) * y * z - s * x;
    rot.b4 = 0.0f;

    rot.c1 = (1 - c) * z * x - s * y;
    rot.c2 = (1 - c) * z * y + s * x;
    rot.c3 = c + (1 - c) * z * z;
    rot.c4 = 0.0f;

    rot.d1 = 0.0f;
    rot.d2 = 0.0f;
    rot.d3 = 0.0f;
    rot.d4 = 1.0f;

    return rot;
}

Character3D::Character3D()
{
    submeshes.clear();
    textureMap.clear();
    boneMapping.clear();
    boneInfo.clear();
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
    boneMapping.clear();
    boneInfo.clear();

    // Processa cada mesh do modelo
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

        // Processa vértices
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

            // Inicializa os dados de bone para o vértice
            for (int j = 0; j < 4; j++)
            {
                vert.boneIDs[j] = 0;
                vert.weights[j] = 0.0f;
            }
            submesh.vertices.push_back(vert);
        }

        // Se o mesh possuir bones, processa-os
        if (mesh->HasBones())
        {
            for (unsigned int b = 0; b < mesh->mNumBones; b++)
            {
                aiBone *bone = mesh->mBones[b];
                std::string boneName(bone->mName.C_Str());
                int boneIndex = 0;

                if (boneMapping.find(boneName) == boneMapping.end())
                {
                    boneIndex = boneInfo.size();
                    boneMapping[boneName] = boneIndex;

                    BoneInfo info;
                    info.offsetMatrix = bone->mOffsetMatrix;
                    info.finalTransformation = aiMatrix4x4(); // identidade
                    boneInfo.push_back(info);
                }
                else
                {
                    boneIndex = boneMapping[boneName];
                }

                // Para cada peso deste bone, associa ao vértice correspondente
                for (unsigned int w = 0; w < bone->mNumWeights; w++)
                {
                    unsigned int vertexID = bone->mWeights[w].mVertexId;
                    float weight = bone->mWeights[w].mWeight;
                    Vertex &vert = submesh.vertices[vertexID];
                    for (int j = 0; j < 4; j++)
                    {
                        if (vert.weights[j] == 0.0f)
                        {
                            vert.boneIDs[j] = boneIndex;
                            vert.weights[j] = weight;
                            break;
                        }
                    }
                }
            }
        }

        submeshes.push_back(submesh);
    }

    return true;
}

void Character3D::draw() const
{
    // Para cada submesh, aplica a textura e desenha os triângulos
    for (const auto &sub : submeshes)
    {
        glBindTexture(GL_TEXTURE_2D, sub.textureID);
        glBegin(GL_TRIANGLES);
        for (const auto &vert : sub.vertices)
        {
            // Aplica a transformação dos bones se houver influência
            aiVector3D pos(vert.x, vert.y, vert.z);
            aiVector3D finalPos(0, 0, 0);
            float totalWeight = 0.0f;

            for (int i = 0; i < 4; i++)
            {
                if (vert.weights[i] > 0.0f)
                {
                    int boneIndex = vert.boneIDs[i];
                    aiMatrix4x4 transform = boneInfo[boneIndex].finalTransformation;
                    aiVector3D transformed;
                    transformed.x = transform.a1 * pos.x + transform.a2 * pos.y + transform.a3 * pos.z + transform.a4;
                    transformed.y = transform.b1 * pos.x + transform.b2 * pos.y + transform.b3 * pos.z + transform.b4;
                    transformed.z = transform.c1 * pos.x + transform.c2 * pos.y + transform.c3 * pos.z + transform.c4;

                    finalPos.x += vert.weights[i] * transformed.x;
                    finalPos.y += vert.weights[i] * transformed.y;
                    finalPos.z += vert.weights[i] * transformed.z;
                    totalWeight += vert.weights[i];
                }
            }

            // Se nenhum bone influenciar, usa a posição original
            if (totalWeight == 0.0f)
            {
                finalPos = pos;
            }

            glTexCoord2f(vert.u, vert.v);
            glVertex3f(finalPos.x, finalPos.y, finalPos.z);
        }
        glEnd();
    }
}

void Character3D::rotateBone(const std::string &boneName, float angle, float axisX, float axisY, float axisZ)
{
    if (boneMapping.find(boneName) == boneMapping.end())
    {
        std::cerr << "Bone '" << boneName << "' não encontrada!" << std::endl;
        return;
    }
    int index = boneMapping[boneName];
    // Cria uma matriz de rotação e atualiza a transformação final do bone.
    boneInfo[index].finalTransformation = createRotationMatrix(angle, axisX, axisY, axisZ);
}
