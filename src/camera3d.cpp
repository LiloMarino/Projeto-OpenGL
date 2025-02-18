#include "camera3d.hpp"

Camera3D::Camera3D(double eyeX, double eyeY, double eyeZ, double rotX, double rotY, double rotZ,
                   double focalLength, double sensorSize, double aspectRatio, double nearPlane, double farPlane)
    : eyeX(eyeX), eyeY(eyeY), eyeZ(eyeZ),
      aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
{
    // Calcula o campo de visão (FOV) baseado na lente e no sensor do Blender
    fov = calculateFOV(focalLength, sensorSize);

    // Calcula a direção da câmera baseada na rotação
    calculateDirection(rotX, rotY, rotZ);
}

void Camera3D::applyCamera() const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
              eyeX + dirX, eyeY + dirY, eyeZ + dirZ, // Centro da câmera (para onde está olhando)
              0.0, 0.0, 1.0);                        // Vetor "para cima" fixo no eixo Z (Por causa do Blender)
}

glm::mat4 Camera3D::getViewMatrix() const
{
    glm::vec3 eye(eyeX, eyeY, eyeZ);
    glm::vec3 center(eyeX + dirX, eyeY + dirY, eyeZ + dirZ);
    glm::vec3 up(0.0f, 0.0f, 1.0f);  // Vetor "para cima"
    return glm::lookAt(eye, center, up);
}


double Camera3D::calculateFOV(double focalLength, double sensorSize) const
{
    return 2.0 * atan((sensorSize / 2.0) / focalLength) * (180.0 / M_PI);
}

void Camera3D::calculateDirection(double rotX, double rotY, double rotZ)
{
    double yaw = rotY * (M_PI / 180.0);
    double pitch = rotX * (M_PI / 180.0);

    dirX = cos(yaw) * cos(pitch);
    dirY = sin(pitch);
    dirZ = sin(yaw) * cos(pitch);
}
