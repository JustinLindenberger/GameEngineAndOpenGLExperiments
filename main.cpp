#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
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
             0.5f, -0.5f, -0.5f,    0.85f, 0.45f, // bottom right left
             0.5f, -0.5f,  0.5f,    0.25f, 0.45f, // bottom right right
             0.0f,  0.5f,  0.0f,    0.55f, 0.90f  // top 
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
    float valentexe[] = {
        -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, //links oben
        -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, //links unten
        0.5f, 1.0f, 0.0f, 1.0f, 1.0f, //rechts oben
        0.5f, -1.0f, 0.0f, 1.0f, 0.0f //rechts unten
    };
    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    unsigned int EBO, VVAO, VVBO, Vtexture;
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VVBO);
    glGenVertexArrays(1, &VVAO);
    glGenTextures(1, &Vtexture);
    int Vwidth, Vheight, VnrChannels;
    unsigned char* Vdata;

    glBindVertexArray(VVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindTexture(GL_TEXTURE_2D, Vtexture);
    
    Vdata = stbi_load("V.jpg", &Vwidth, &Vheight, &VnrChannels, 0);
    if (Vdata) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Vwidth, Vheight, 0, GL_RGB, GL_UNSIGNED_BYTE, Vdata);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(Vdata);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(valentexe), valentexe, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vert[i]), vert[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    glm::mat4 model;
    glm::mat4 proj;
    glm::mat4 view;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        view = camera.GetViewMatrix();
        ourShader.setMatrix4fv("view", view);
        proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMatrix4fv("projection", proj);


        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5));
        model = glm::scale(model, glm::vec3(2.0f, 1.0f, 3.0f));
        ourShader.setMatrix4fv("model", model);

        glBindTexture(GL_TEXTURE_2D, Vtexture);
        glBindVertexArray(VVAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        for (int i = 0; i < 4; i++)
        {    
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            ourShader.setMatrix4fv("model", model);

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

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}