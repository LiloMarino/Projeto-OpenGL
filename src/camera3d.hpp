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
    double dirX, dirY, dirZ; ///< Vetor direção da câmera.
    double fov;              ///< Campo de visão vertical (em graus).
    double aspectRatio;      ///< Proporção da tela (largura/altura).
    double nearPlane;        ///< Distância do plano de recorte próximo.
    double farPlane;         ///< Distância do plano de recorte distante.

public:
    /**
     * @brief Construtor da câmera, baseado em parâmetros do Blender.
     *
     * @param eyeX Posição X da câmera.
     * @param eyeY Posição Y da câmera.
     * @param eyeZ Posição Z da câmera.
     * @param rotX Rotação em torno do eixo X (ângulo do Blender).
     * @param rotY Rotação em torno do eixo Y (ângulo do Blender).
     * @param rotZ Rotação em torno do eixo Z (ângulo do Blender).
     * @param focalLength Distância focal da lente.
     * @param sensorSize Tamanho do sensor usado para calcular o FOV.
     * @param aspectRatio Proporção da tela (largura/altura).
     * @param nearPlane Distância do plano de recorte próximo.
     * @param farPlane Distância do plano de recorte distante.
     */
    Camera3D(double eyeX, double eyeY, double eyeZ, double rotX, double rotY, double rotZ,
             double focalLength, double sensorSize, double aspectRatio, double nearPlane, double farPlane);

    /**
     * @brief Aplica a configuração da câmera no OpenGL, definindo projeção e posição.
     */
    void applyCamera() const;

private:
    /**
     * @brief Calcula o campo de visão (FOV) com base na distância focal e no tamanho do sensor.
     * @param focalLength Distância focal da lente.
     * @param sensorSize Tamanho do sensor.
     * @return O valor do campo de visão vertical em graus.
     */
    double calculateFOV(double focalLength, double sensorSize) const;

    /**
     * @brief Calcula o vetor direção da câmera baseado nas rotações em torno dos eixos X e Y.
     * @param rotX Rotação em torno do eixo X (pitch).
     * @param rotY Rotação em torno do eixo Y (yaw).
     * @param rotZ Rotação em torno do eixo Z (não utilizado diretamente).
     */
    void calculateDirection(double rotX, double rotY, double rotZ);
};

#endif
