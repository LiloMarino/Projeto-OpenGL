#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <GL/glu.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera3D
{
private:
    double eyeX, eyeY, eyeZ; ///< Posição da câmera no espaço 3D.
    double dirX, dirY, dirZ; ///< Direção da câmera.
    double fov;              ///< Campo de visão vertical.
    double aspectRatio;      ///< Proporção da tela.
    double nearPlane;        ///< Plano próximo de corte da câmera.
    double farPlane;         ///< Plano distante de corte da câmera.

public:
    Camera3D(double eyeX, double eyeY, double eyeZ, double rotX, double rotY, double rotZ,
             double focalLength, double sensorSize, double aspectRatio, double nearPlane, double farPlane);

    /**
     * @brief Aplica as configurações da câmera no OpenGL.
     */
    void applyCamera() const;

    /**
     * @brief Retorna a matriz de visão.
     */
    glm::mat4 getViewMatrix() const;

private:
    double calculateFOV(double focalLength, double sensorSize) const;
    void calculateDirection(double rotX, double rotY, double rotZ);
};

#endif
