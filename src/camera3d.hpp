#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <GL/glu.h>
#include <cmath>

class Camera3D
{
private:
    double eyeX, eyeY, eyeZ;   ///< Posição da câmera no espaço 3D.
    double dirX, dirY, dirZ;   ///< Direção da câmera.
    double fov;                ///< Campo de visão vertical.
    double aspectRatio;        ///< Proporção da tela.
    double nearPlane;          ///< Plano próximo de corte da câmera.
    double farPlane;           ///< Plano distante de corte da câmera.

public:
    /**
     * @brief Construtor da câmera 3D baseado em parâmetros do Blender.
     * 
     * @param eyeX Posição X da câmera.
     * @param eyeY Posição Y da câmera.
     * @param eyeZ Posição Z da câmera.
     * @param rotX Rotação em torno do eixo X (ângulo do Blender).
     * @param rotY Rotação em torno do eixo Y (ângulo do Blender).
     * @param rotZ Rotação em torno do eixo Z (ângulo do Blender).
     * @param focalLength Distância focal da câmera.
     * @param sensorSize Tamanho do sensor da câmera.
     * @param aspectRatio Proporção da tela.
     * @param nearPlane Distância do plano próximo de renderização.
     * @param farPlane Distância do plano distante de renderização.
     */
    Camera3D(double eyeX, double eyeY, double eyeZ, double rotX, double rotY, double rotZ, 
             double focalLength, double sensorSize, double aspectRatio, double nearPlane, double farPlane);

    /**
     * @brief Aplica as configurações da câmera no OpenGL.
     */
    void applyCamera() const;

private:
    /**
     * @brief Calcula o campo de visão (FOV) com base na distância focal e no tamanho do sensor.
     */
    double calculateFOV(double focalLength, double sensorSize) const;

    /**
     * @brief Calcula a direção para onde a câmera está olhando com base nas rotações.
     */
    void calculateDirection(double rotX, double rotY, double rotZ);
};

#endif
