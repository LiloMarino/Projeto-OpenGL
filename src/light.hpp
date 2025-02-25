#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <GL/glut.h>

#define LUZ_DIFUSA 0
#define LUZ_PONTUAL 1

class Light 
{
private:
    const GLfloat MIN_BRIGHT = 0.2;
    const GLfloat MAX_BRIGHT = 5.0;
    const GLfloat BRIGHT_INTERVAL = 0.1;

    GLfloat kd = MIN_BRIGHT;
    GLfloat diffuse[4];
    GLfloat light0_pos[4];
    
public:
    /**
     * @brief Construtor
     * @param x A posição x da luz
     * @param y A posição y da luz
     * @param z A posição z da luz
     * @param tipo O tipo de luz
     */
    Light(GLfloat x, GLfloat y, GLfloat z, GLfloat tipo);
    
    /**
     * @brief Destrutor
     */
    ~Light();

    /**
     * @brief Aplica as configurações de iluminação na cena
     */
    void apply();

    /**
     * @brief Aumenta o brilho da cena
     */
    void setMoreBrightness();

    /**
     * @brief Diminui o brilho da cena
     */
    void setLessBrightness();

};

#endif 