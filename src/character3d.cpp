#include "character3d.hpp"
#include "stb_image.h"
#include <iostream>
#include <cmath>

aiMatrix4x4 createRotationMatrix(float angle, float x, float y, float z)
{
    // Converter ângulo de graus para radianos e calcular cosseno e seno
    float rad = angle * 3.14159265f / 180.0f;
    float c = cos(rad);
    float s = sin(rad);

    // Calcular o comprimento do vetor de rotação e normalizá-lo
    float len = sqrt(x * x + y * y + z * z);
    if (len == 0)
        len = 1.0f;
    x /= len;
    y /= len;
    z /= len;

    // Calcular os elementos da matriz de rotação utilizando a fórmula de Rodrigues
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

    // Definir a última linha para coordenadas homogêneas
    rot.d1 = 0.0f;
    rot.d2 = 0.0f;
    rot.d3 = 0.0f;
    rot.d4 = 1.0f;

    return rot;
}

Character3D::Character3D()
{
    // Inicializa os containers, garantindo que não haja resíduos de dados anteriores
    submeshes.clear();
    textureMap.clear();
    boneMapping.clear();
    boneInfo.clear();
}

GLuint Character3D::loadTexture(const std::string &path)
{
    // Carrega os dados da imagem a partir do arquivo utilizando stb_image
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data)
    {
        std::cerr << "Erro ao carregar textura: " << path << std::endl;
        return 0;
    }

    // Gera e configura a textura no OpenGL
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Libera os dados da imagem e retorna o ID da textura
    stbi_image_free(data);
    return textureID;
}

bool Character3D::loadModel(const std::string &path, const std::string &textureDir)
{
    // Carrega a cena do modelo utilizando Assimp com triangulação e ajuste de UVs
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Erro ao carregar modelo: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Limpa os dados anteriores
    submeshes.clear();
    boneMapping.clear();
    boneInfo.clear();

    // Processa cada mesh presente na cena
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;
        GLuint texID = 0;

        // Se o material possuir textura difusa, carrega-a e atualiza o cache
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string fullTexturePath = textureDir + "/" + texturePath.C_Str();
            if (textureMap.find(fullTexturePath) == textureMap.end())
                textureMap[fullTexturePath] = loadTexture(fullTexturePath);
            texID = textureMap[fullTexturePath];
            std::cout << "Carregando textura: " << fullTexturePath << std::endl;
        }

        // Inicializa o submesh com a textura carregada
        SubMesh submesh;
        submesh.textureID = texID;

        // Processa os vértices do mesh
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vert;
            vert.x = mesh->mVertices[v].x;
            vert.y = mesh->mVertices[v].y;
            vert.z = mesh->mVertices[v].z;

            // Verifica se há coordenadas de textura; caso contrário, define como zero
            if (mesh->mTextureCoords[0])
            {
                vert.u = mesh->mTextureCoords[0][v].x;
                vert.v = mesh->mTextureCoords[0][v].y;
            }
            else
            {
                vert.u = vert.v = 0.0f;
            }

            // Inicializa os dados de skinning (bones) para o vértice
            for (int j = 0; j < 4; j++)
            {
                vert.boneIDs[j] = 0;
                vert.weights[j] = 0.0f;
            }
            submesh.vertices.push_back(vert);
        }

        // Se o mesh contiver bones, processa os dados de cada bone
        if (mesh->HasBones())
        {
            for (unsigned int b = 0; b < mesh->mNumBones; b++)
            {
                aiBone *bone = mesh->mBones[b];
                std::string boneName(bone->mName.C_Str());
                int boneIndex = 0;

                // Verifica se o bone já foi mapeado; caso contrário, adiciona-o
                if (boneMapping.find(boneName) == boneMapping.end())
                {
                    boneIndex = boneInfo.size();
                    boneMapping[boneName] = boneIndex;

                    BoneInfo info;
                    info.offsetMatrix = bone->mOffsetMatrix;
                    info.defaultLocalTransform = aiMatrix4x4(); // identidade por padrão
                    info.manualRotation = aiMatrix4x4();        // identidade
                    info.finalTransformation = aiMatrix4x4();   // identidade
                    info.parentIndex = -1;
                    boneInfo.push_back(info);
                }
                else
                {
                    boneIndex = boneMapping[boneName];
                }

                // Associa cada peso do bone ao vértice correspondente
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

        // Adiciona o submesh processado à lista de submeshes
        submeshes.push_back(submesh);
    }

    // Após processar os meshes, percorre a hierarquia de nós para definir as transformações locais (bind pose) e
    // estabelecer o relacionamento pai-filho dos bones.
    readHierarchy(scene->mRootNode, aiMatrix4x4(), -1);

    return true;
}

void Character3D::draw() const
{
    // Atualiza as transformações finais dos bones, respeitando a hierarquia.
    // Como draw() é const, usamos const_cast para chamar a função não-const.
    const_cast<Character3D *>(this)->updateBoneTransforms();

    // Para cada submesh, vincula a textura e desenha os triângulos
    for (const auto &sub : submeshes)
    {
        glBindTexture(GL_TEXTURE_2D, sub.textureID);
        glBegin(GL_TRIANGLES);
        for (const auto &vert : sub.vertices)
        {
            // Calcula a posição final do vértice considerando a influência dos bones
            aiVector3D pos(vert.x, vert.y, vert.z);
            aiVector3D finalPos(0, 0, 0);
            float totalWeight = 0.0f;

            // Aplica a transformação de cada bone que influencia o vértice
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

            // Se nenhum bone influenciar o vértice, utiliza a posição original
            if (totalWeight == 0.0f)
                finalPos = pos;

            glTexCoord2f(vert.u, vert.v);
            glVertex3f(finalPos.x, finalPos.y, finalPos.z);
        }
        glEnd();
    }
}

void Character3D::rotateBone(const std::string &boneName, float angle, float axisX, float axisY, float axisZ)
{
    // Verifica se o bone existe no mapeamento
    if (boneMapping.find(boneName) == boneMapping.end())
    {
        std::cerr << "Bone '" << boneName << "' não encontrada!" << std::endl;
        return;
    }

    // Atualiza a rotação manual do bone utilizando a matriz de rotação criada
    int index = boneMapping[boneName];
    boneInfo[index].manualRotation = createRotationMatrix(angle, axisX, axisY, axisZ);
}

void Character3D::rotateBone(const std::string &boneName, const glm::quat &rotation)
{
    // Procura o bone no mapa de bones
    auto it = boneMapping.find(boneName);
    if (it != boneMapping.end())
    {
        int boneIndex = it->second; // Obtém o índice do bone
        BoneInfo &bone = boneInfo[boneIndex];

        // Converte o quaternion para uma matriz 4x4 usando glm
        glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        // Atualiza a rotação manual do bone combinando com a rotação já existente
        bone.manualRotation = aiMatrix4x4(rotationMatrix[0][0], rotationMatrix[0][1], rotationMatrix[0][2], rotationMatrix[0][3],
                                          rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2], rotationMatrix[1][3],
                                          rotationMatrix[2][0], rotationMatrix[2][1], rotationMatrix[2][2], rotationMatrix[2][3],
                                          rotationMatrix[3][0], rotationMatrix[3][1], rotationMatrix[3][2], rotationMatrix[3][3]);
    }
    else
    {
        std::cerr << "Bone " << boneName << " não encontrado!" << std::endl;
    }
}

// ----- Funções auxiliares para hierarquia de bones -----

void Character3D::readHierarchy(const aiNode *node, const aiMatrix4x4 &parentTransform, int parentBoneIndex)
{
    // Calcula a transformação acumulada atual multiplicando a transformação do pai com a transformação do nó atual
    aiMatrix4x4 currentTransform = parentTransform * node->mTransformation;
    int currentBoneIndex = parentBoneIndex;
    std::string nodeName(node->mName.C_Str());

    // Se o nó corresponde a um bone, atualiza o índice do bone e salva a transformação local (bind pose)
    if (boneMapping.find(nodeName) != boneMapping.end())
    {
        currentBoneIndex = boneMapping[nodeName];
        boneInfo[currentBoneIndex].parentIndex = parentBoneIndex;
        boneInfo[currentBoneIndex].defaultLocalTransform = node->mTransformation;
    }

    // Processa recursivamente os nós filhos
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        readHierarchy(node->mChildren[i], currentTransform, currentBoneIndex);
}

aiMatrix4x4 Character3D::computeGlobalTransform(int boneIndex) const
{
    // Se o bone é raiz, retorna sua transformação local combinada com a rotação manual
    if (boneInfo[boneIndex].parentIndex == -1)
        return boneInfo[boneIndex].defaultLocalTransform * boneInfo[boneIndex].manualRotation;
    else
        // Caso contrário, multiplica recursivamente a transformação global do pai pela transformação local do bone
        return computeGlobalTransform(boneInfo[boneIndex].parentIndex) *
               (boneInfo[boneIndex].defaultLocalTransform * boneInfo[boneIndex].manualRotation);
}

void Character3D::updateBoneTransforms()
{
    // Para cada bone, calcula a transformação global e atualiza sua transformação final para skinning
    for (unsigned int i = 0; i < boneInfo.size(); i++)
    {
        aiMatrix4x4 global = computeGlobalTransform(i);
        boneInfo[i].finalTransformation = global * boneInfo[i].offsetMatrix;
    }
}
