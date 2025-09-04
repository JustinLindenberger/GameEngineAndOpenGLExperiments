#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <chrono>
#include <thread>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// screen settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.5f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Cube that the camera is currently looking at (nullptr if no cube is looked at)
Cube* targetedCube = nullptr;

// Comparision function to sort cubes by distance to the camera, smallest distance first
bool sortCubes(Cube a, Cube b) {
    return (glm::distance(camera.Position, a.Position) < glm::distance(camera.Position, b.Position));
};

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    Shader ourShader("vShader.txt", "fShader.txt");
    ourShader.use();

    // Initally places 9 cubes in a 3x3 grid.
    Cube cube0(0.0f, 0.0f, 0.0f, ourShader);
    Cube cube1(1.5f, 0.0f, 1.5f, ourShader);
    Cube cube2(1.5f, 0.0f, 0.0f, ourShader);
    Cube cube3(1.5f, 0.0f, -1.5f, ourShader);
    Cube cube4(0.0f, 0.0f, -1.5f, ourShader);
    Cube cube5(-1.5f, 0.0f, -1.5f, ourShader);
    Cube cube6(-1.5f, 0.0f, 0.0f, ourShader);
    Cube cube7(-1.5f, 0.0f, 1.5f, ourShader);
    Cube cube8(0.0f, 0.0f, 1.5f, ourShader);

    // Matrix that projects all geometry to normalized device coordinates, which are required by openGL to draw shapes.
    glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMatrix4fv("projection", proj);
    glm::mat4 view;

    std::vector<Cube> cubes = { cube0, cube1, cube2, cube3, cube4, cube5, cube6, cube7, cube8 };

    /* This loop first calculates the time passed between frames (needed to scale camera movement), 
    * sorts cubes by distance to the camera, checks in order whether the camera is looking at (targeting)
    * a cube. If the left mouse button is held the cube will be tied to the camera movement and move 
    * and turn with the camera. All cubes are drawn, targeted cube is red, all other cubes are white.
    */
    while (!glfwWindowShouldClose(window))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Retrieve the matrix that enforces the cameras viewing angle of the game world.
        view = camera.GetViewMatrix();
        ourShader.setMatrix4fv("view", view);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::sort(cubes.begin(), cubes.end(), sortCubes);

        // Targeted flag also determins cube color, so it needs to be reset so that cubes aren't all painted red over time. 
        for (int i = 0; i < cubes.size(); i++) {
            cubes[i].targeted = false;
        }
        targetedCube = nullptr;
        for (int i = 0; i < cubes.size(); i++) {
            if (cubes[i].isCubeTargeted(camera.Position, camera.Front)) {
                targetedCube = &cubes[i];
                break;
            }
        }
        for (int i = 0; i < cubes.size(); i++) {
            cubes[i].drawCube();
        }

        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// A, W, S, and D and are used to steer the camera Left, Forward, Backwards and Right.
void processInput(GLFWwindow* window)
{
    // ESC stops the rendering loop and terminates the program.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 previousPos = camera.Position;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // If the mouse button is held the cube is displayed by the same movement as the camera.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (targetedCube)
            targetedCube->Position += (camera.Position - previousPos);
    }
}

// Rescales the viewport when the window is resized.
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

    // Save previous orientation so that we can calculate the change in direction to the previus frame.
    float prevYaw = camera.Yaw;
    float prevPitch = camera.Pitch;
    // Pass on the mouse movement offsets which are then translated into camera rotations.
    camera.ProcessMouseMovement(xoffset, yoffset);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (targetedCube) {
            // acquire vector pointing from camera to targeted cube
            glm::vec3 cameraToCube = targetedCube->Position - camera.Position;
            
            float dYaw = glm::radians(camera.Yaw - prevYaw);
            float dPitch = glm::radians(camera.Pitch - prevPitch);

            // Rotate cameraToCube vector by change in player direction.
            // dYaw is negated because a camera rotation the right results in a positive dYaw
            // but a positive dYaw means a rotation to the left. Rotating the camera downards 
            // makes dPitch positive, and positive dPatch also corresponds to a downwards rotation.
            glm::mat4 RAroundY = glm::rotate(glm::mat4(1.0f), -dYaw, glm::vec3(0, 1, 0));
            glm::mat4 RAroundRight = glm::rotate(glm::mat4(1.0f), dPitch, camera.Right);
            glm::mat4 R = RAroundRight * RAroundY;
            glm::vec3 rotated = glm::mat3(R) * cameraToCube;

            targetedCube->Position = camera.Position + rotated;
        }
    }
}

// Scrolling the mouse changes the FOV.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}