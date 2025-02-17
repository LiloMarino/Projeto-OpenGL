#include "camera3d.hpp"

// A implementação dos métodos já está incluída no cabeçalho.

Camera3D::Camera3D(double eyeX, double eyeY, double eyeZ, double rotX, double rotY, double rotZ, double focalLength, double sensorSize, double aspectRatio, double nearPlane, double farPlane)
    : eyeX(eyeX), eyeY(eyeY), eyeZ(eyeZ), rotX(rotX), rotY(rotY), rotZ(rotZ), focalLength(focalLength), sensorSize(sensorSize), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
{
    calculateFOV();
    calculateDirection();
}

void Camera3D::applyCamera() const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void Camera3D::calculateFOV()
{
    fov = 2.0 * atan((sensorSize / 2.0) / focalLength) * (180.0 / M_PI);
}

void Camera3D::calculateDirection()
{
    double yaw = rotY * (M_PI / 180.0);
    double pitch = rotX * (M_PI / 180.0);

    centerX = eyeX + cos(yaw) * cos(pitch);
    centerY = eyeY + sin(pitch);
    centerZ = eyeZ + sin(yaw) * cos(pitch);

    upX = 0.0;
    upY = 0.0;
    upZ = 1.0;
}
