#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

float radius = 3.0f;  // Distância da câmera ao centro
glm::quat cameraRotation = glm::quat(1, 0, 0, 0); // Quaternion inicial (sem rotação)

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
}

void updateCamera()
{
    glm::vec3 position = cameraRotation * glm::vec3(0.0f, 0.0f, radius);

    glm::mat4 view = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), cameraRotation * glm::vec3(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&view[0][0]); // Envia a matriz de visão corrigida para OpenGL
}

void rotateCamera(float angle, glm::vec3 axis)
{
    glm::quat rotation = glm::angleAxis(glm::radians(angle), axis);
    cameraRotation = glm::normalize(rotation * cameraRotation); // Acumula a rotação no quaternion
}

void drawCube()
{
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glEnd();

    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glEnd();

    glColor3f(0.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 4.0);

    updateCamera();
    drawCube();
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_UP:
            rotateCamera(2.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotação em torno do eixo X
            break;
        case GLFW_KEY_DOWN:
            rotateCamera(-2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        case GLFW_KEY_LEFT:
            rotateCamera(2.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotação em torno do eixo Y
            break;
        case GLFW_KEY_RIGHT:
            rotateCamera(-2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case GLFW_KEY_J:
            radius += 0.5f; // Zoom In
            break;
        case GLFW_KEY_K:
            radius -= 0.5f; // Zoom Out
            if (radius < 1.0f) radius = 1.0f; // Evita zoom negativo
            break;
        }
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(600, 600, "Viewing 3D", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    init();

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
