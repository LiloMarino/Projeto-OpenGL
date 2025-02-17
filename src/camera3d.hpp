#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <GL/glu.h>
#include <cmath>

class Camera3D
{
private:
    double eyeX, eyeY, eyeZ;
    double rotX, rotY, rotZ;
    double focalLength;
    double sensorSize;
    double aspectRatio;
    double nearPlane;
    double farPlane;
    double fov;
    double centerX, centerY, centerZ;
    double upX, upY, upZ;

public:
    Camera3D(double eyeX, double eyeY, double eyeZ, double rotX, double rotY, double rotZ, double focalLength, double sensorSize, double aspectRatio, double nearPlane, double farPlane);

    void applyCamera() const;

private:
    void calculateFOV();
    
    void calculateDirection();
};

#endif
