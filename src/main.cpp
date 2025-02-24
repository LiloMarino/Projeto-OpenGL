#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "camera3d.hpp"
#include "character3d.hpp"

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow *window, const Camera3D &camera, const Character3D &character)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.applyCamera();
    character.draw();
    glfwSwapBuffers(window);
}

void keyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch(key)
        {
        case GLFW_KEY_ESCAPE:
            exit(0);
            break;
        
        default:
            break;
        }
    }
}

void rotateHeadToMouse(GLFWwindow *window, Character3D &character)
{
    // Captura a posição atual do mouse na janela
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Obtém o tamanho da janela
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Define os limites máximos de rotação da cabeça (60 graus em cada direção)
    float maxRotationX = glm::radians(60.0f); // Limite de rotação no eixo X (vertical)
    float maxRotationY = glm::radians(60.0f); // Limite de rotação no eixo Y (horizontal)

    // Normaliza a posição do mouse dentro da janela
    float normalizedMouseX = (float)mouseX / (float)windowWidth;  // Proporção horizontal do mouse
    float normalizedMouseY = (float)mouseY / (float)windowHeight; // Proporção vertical do mouse

    // Calcula o ângulo de rotação baseado na posição normalizada do mouse
    float rotationX = (normalizedMouseY - 0.5f) * maxRotationX * 2.0f; // Rotação no eixo X (vertical)
    float rotationY = (normalizedMouseX - 0.5f) * maxRotationY * 2.0f; // Rotação no eixo Y (horizontal)

    // Usando quaternions para aplicar as rotações
    glm::quat rotationXQuat = glm::angleAxis(rotationX, glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotação no eixo X
    glm::quat rotationYQuat = glm::angleAxis(rotationY, glm::vec3(0.0f, 0.0f, 1.0f));  // Rotação no eixo Y

    // Combina as rotações no eixo X e Y
    glm::quat combinedRotation = rotationYQuat * rotationXQuat;

    // Aplica a rotação ao bone "Head"
    character.rotateBone("Head", combinedRotation);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(800, 600, "Modelo com Textura", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyboardEvents);

    Camera3D camera(0.0, -9.0, 16.0, 90.0, 0.0, 0.0, 50.0, 36.0, 800.0 / 600.0, 0.1, 1000.0);
    Character3D character;

    if (!character.loadModel("Mita/Mita (orig).fbx", "Mita"))
    {
        return -1;
    }

    init();

    while (!glfwWindowShouldClose(window))
    {
        // Chama a função para rotacionar o bone "Head" para olhar para o mouse
        rotateHeadToMouse(window, character);

        display(window, camera, character);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
