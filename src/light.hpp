#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <GL/glut.h>

class Light 
{
private:
    const GLfloat MIN_BRIGHT = 0.5;
    const GLfloat MAX_BRIGHT = 4.0;
    const GLfloat BRIGHT_INTERVAL = 0.1;

    GLfloat kd = MIN_BRIGHT;
    GLfloat diffuse[4];
    
public:
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