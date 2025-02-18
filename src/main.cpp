#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <GLFW/glfw3.h>
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

    Camera3D camera(0.0, -9.0, 16.0, 90.0, 0.0, 0.0, 50.0, 36.0, 800.0 / 600.0, 0.1, 1000.0);
    Character3D character;

    if (!character.loadModel("Mita/Mita (orig).fbx", "Mita"))
    {
        return -1;
    }

    init();

    // Ângulo de rotação para o bone (poderia ser controlado pelo tempo ou input do usuário)
    float angle = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // Exemplo: se a tecla R for pressionada, rotaciona o bone (altere "BoneName" para o nome do seu bone)
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            angle += 1.0f; // aumenta o ângulo
            character.rotateBone("Head", angle, 0.0f, 0.0f, 1.0f);
        }

        display(window, camera, character);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
