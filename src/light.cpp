#include "light.hpp"

const GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
const GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
const GLfloat light0_pos[] = {5.0f, 10.0f, 2.0f, 1.0f};

void Light::apply()
{
    diffuse[0] = kd;
    diffuse[1] = kd;
    diffuse[2] = kd;
    diffuse[3] = 1.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0,GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0,GL_AMBIENT, black);
    glLightfv(GL_LIGHT0,GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0,GL_SPECULAR, white);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void Light::setMoreBrightness()
{
    if(kd < MAX_BRIGHT){
        kd += BRIGHT_INTERVAL;
    }
}

void Light::setLessBrightness()
{
    if(kd > MIN_BRIGHT){
        kd -= BRIGHT_INTERVAL;
    }
}