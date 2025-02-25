#include <iostream>
#include <stb_image.h>
#include "background.hpp"

Background::Background(){}

Background::~Background()
{
    glDeleteTextures(1, &textureID);
}

bool Background::loadTexture(const char *caminho)
{
    int largura, altura, canais;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(caminho, &largura, &altura, &canais, 4);
    
    if(!data) 
    {
        std::cerr << "Erro ao carregar fundo: " <<  caminho  << "\n";
        return false;
    }

    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);    // Bind
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, 0);    // Unbind

    stbi_image_free(data);

    std::cout << "Fundo carregado: " << caminho << "\n";
    return true;
}

void Background::draw() {
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Adiciona a textura em um plano
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-10, 2, 10);  // Inferior esquerdo
        glTexCoord2f(1, 0); glVertex3f(20, 2, 10);   // Inferior direito
        glTexCoord2f(1, 1); glVertex3f(20, 2, 20);    // Superior direito
        glTexCoord2f(0, 1); glVertex3f(-10, 2, 20);   // Superior esquerdo
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}