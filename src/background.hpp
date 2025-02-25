#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <GL/glew.h>

class Background
{
private:
    GLuint textureID;
public:
    /**
     * @brief Construtor
     */
    Background();
    
    /**
     * @brief Destrutor
     */
    ~Background();

    /**
     * @brief Carrega a textura para o fundo
     * @param caminho O caminho da textura do fundo
     * @return True se concluiu com sucesso, false para caso contrário
     */
    bool loadTexture(const char *caminho);

    /**
     * @brief Desenha o background
     */
    void draw();
};

#endif