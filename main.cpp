#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Shader.h"
#include "stb_image.h"

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glViewport(0, 0, 800, 600);

    Shader ourShader("vShader.txt", "fShader.txt");
    ourShader.use();

    // Configuration for how screen pixels interpolate between texture pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    stbi_set_flip_vertically_on_load(true);

    float vert[4][15] =
    {
            // positions            //texture coords
        {
             0.5f, -0.5f, -0.5f,    0.55f, 0.95f, // bottom right left
             0.5f, -0.5f,  0.5f,    0.55f, 0.35f, // bottom right right
             0.0f,  0.5f,  0.0f,    0.85f, 0.65f  // top 
        },
        {
             0.5f, -0.5f,  0.5f,    1.0f,  0.28f, // bottom right front 
            -0.5f, -0.5f,  0.5f,    0.6f,  0.28f, // bottom left front
             0.0f,  0.5f,  0.0f,    0.8f,  0.48f  // top 
        },
        {
            -0.5f, -0.5f,  0.5f,    0.4f,  0.4f,  // bottom left left 
            -0.5f, -0.5f, -0.5f,    0.0f,  0.25f, // bottom right left
             0.0f,  0.5f,  0.0f,    0.0f,  0.8f   // top 
        },
        {
             0.5f, -0.5f, -0.5f,    0.0f,  0.2f,  // bottom right back
            -0.5f, -0.5f, -0.5f,    1.0f,  0.2f,  // bottom left back
             0.0f,  0.5f,  0.0f,    0.5f,  0.9f   // top 
        }
    };

    unsigned int VAO[4];
    glGenVertexArrays(4, VAO);
    unsigned int VBO[4];
    glGenBuffers(4, VBO);
    unsigned int texture[4];
    glGenTextures(4, texture);
    char const *textureFiles[4] = {"E.jpg", "L.jpg", "M.jpg", "T.jpg" };
    int width[4], height[4], nrChannels[4];
    unsigned char* data[4];
    for (int i = 0; i < 4; i++)
    {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        data[i] = stbi_load(textureFiles[i], &width[i], &height[i], &nrChannels[i], 0);
        if (data[i]) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[i], height[i], 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture number " << i << std::endl;
        }
        stbi_image_free(data[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vert[i]), vert[i], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < 4; i++)
        {
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glBindVertexArray(VAO[i]);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

