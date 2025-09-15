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
#include <set>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, std::set<Cube*>* movingCubes);
glm::vec3 calculateAngularVelocity(glm::vec3 prevFront, glm::vec3 front, float mouseMovDelay);

// screen settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.5f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame.
float lastFrame = 0.0f;
float lastMouseMov = 0.0f; // Timing of how long the last mouse rotation was.

// Cube that the camera is currently looking at (nullptr if no cube is looked at)
Cube* targetedCube = nullptr;
// Information about the previously held cube and its direction.
Cube* prevTargetedCube = nullptr;
bool prevHeld = false;
glm::vec3 prevFront = glm::vec3(0.0f);

// Comparision function to sort cubes by distance to the camera, smallest distance first
bool sortCubes(Cube* a, Cube *b) {
    return (glm::distance(camera.Position, a->Position) < glm::distance(camera.Position, b->Position));
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
    //glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    Shader plainShader("vShader.txt", "fShader.txt");
    Shader lightShader("vLightShader.txt", "fLightShader.txt");

    // Vertex data for the crosshair.
    const float crossHairVert[] = {
        -0.02f,  0.005f, 0.0f,
        -0.02f, -0.005f, 0.0f,
         0.02f,  0.005f, 0.0f,
         0.02f,  0.005f, 0.0f,
        -0.02f, -0.005f, 0.0f,
         0.02f, -0.005f, 0.0f,

        -0.005f,  0.02f, 0.0f,
        -0.005f, -0.02f, 0.0f,
         0.005f,  0.02f, 0.0f,
         0.005f,  0.02f, 0.0f,
        -0.005f, -0.02f, 0.0f,
         0.005f, -0.02f, 0.0f,
    };
    unsigned int crossHairVBO, crossHairVAO;
    glGenVertexArrays(1, &crossHairVAO);
    glGenBuffers(1, &crossHairVBO);

    glBindVertexArray(crossHairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crossHairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crossHairVert), crossHairVert, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Initally places 9 cubes in a 3x3 grid.
    Cube cube0(0.0f, 0.5f, 0.0f, lightShader, "cube0");
    Cube cube1(1.5f, 0.5f, 1.5f, lightShader, "cube1");
    Cube cube2(1.5f, 0.5f, 0.0f, lightShader, "cube2");
    Cube cube3(1.5f, 0.5f, -1.5f, lightShader, "cube3");
    Cube cube4(0.0f, 0.5f, -1.5f, lightShader, "cube4");
    Cube cube5(-1.5f, 0.5f, -1.5f, lightShader, "cube5");
    Cube cube6(-1.5f, 0.5f, 0.0f, lightShader, "cube6");
    Cube cube7(-1.5f, 0.5f, 1.5f, lightShader, "cube7");
    Cube cube8(0.0f, 0.5f, 1.5f, lightShader, "cube8");

    Cube lightCube(0.0f, 4.0f, 1.5f, plainShader, "lightCube");

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    plainShader.use();
    plainShader.setVec3("lightColor", lightColor);
    lightShader.use();
    lightShader.setVec3("lightColor", lightColor);
    lightShader.setVec3("lightPos", lightCube.Position);

    // Matrix that projects all geometry to normalized device coordinates, which are required by openGL to draw shapes.
    glm::mat4 proj;
    glm::mat4 view;

    std::vector<Cube*> cubes = { &cube0, &cube1, &cube2, &cube3, &cube4, &cube5, &cube6, &cube7, &cube8 };
    std::set<Cube*> movingCubes;

    /* This loop first calculates the time passed between frames (needed to scale camera movement), 
    * sorts cubes by distance to the camera, checks in order whether the camera is looking at (targeting)
    * a cube. If the left mouse button is held the cube will be tied to the camera movement and move 
    * and turn with the camera. All cubes are drawn, targeted cube is red, all other cubes are white.
    */
    while (!glfwWindowShouldClose(window))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::sort(cubes.begin(), cubes.end(), sortCubes);
        // Targeted flag also determins cube color, so it needs to be reset so that cubes aren't all painted red over time. 
        for (int i = 0; i < cubes.size(); i++) {
            cubes[i]->targeted = false;
        }

        // Targeted Cubes are checked in order because the line of sight might intersect multiple cubes but only the closest should be targeted.
        for (int i = 0; i < cubes.size(); i++) {
            if (cubes[i]->isCubeTargeted(camera.Position, camera.Front)) {
                // if (targetedCube != cubes[i] && targetedCube) {}
                prevTargetedCube = targetedCube;
                targetedCube = cubes[i];
                break;
            }
            if (i == cubes.size() - 1) {
                targetedCube = nullptr;
            }
        }
        if (!prevHeld) {
            prevTargetedCube = nullptr;
        }

        processInput(window, &movingCubes);

        // Process each of the currently moving cubes. Once a cube hits the ground processMovement returns false, so it wont be processed in the next frame. 
        for (auto it = movingCubes.begin(); it != movingCubes.end(); ) {
            Cube* c = *it;
            if (!c->processMovement(deltaTime)) {
                it = movingCubes.erase(it);
            }
            else {
                ++it;
            }
        }

        // Retrieve the matrix that enforces the cameras viewing angle of the game world.
        view = camera.GetViewMatrix();
        proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        lightShader.use();
        lightShader.setMatrix4fv("projection", proj);
        lightShader.setMatrix4fv("view", view);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < cubes.size(); i++) {
            cubes[i]->drawCube();
        }
        
        plainShader.use();
        glBindVertexArray(lightCube.VAO);
        plainShader.setMatrix4fv("model", glm::translate(glm::mat4(1.0f), lightCube.Position));
        plainShader.setMatrix4fv("projection", proj);
        plainShader.setMatrix4fv("view", view);
        plainShader.setInt("lightOrCrossHair", 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draws the corsshair, need to reset all the matrices first so that we can draw over everything in the 2D plane of the screen.
        plainShader.setMatrix4fv("model", glm::mat4(1.0f));
        plainShader.setMatrix4fv("projection", glm::mat4(1.0f));
        plainShader.setMatrix4fv("view", glm::mat4(1.0f));
        plainShader.setInt("lightOrCrossHair", 1);
        glBindVertexArray(crossHairVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// A, W, S, and D and are used to steer the camera Left, Forward, Backwards and Right.
void processInput(GLFWwindow* window, std::set<Cube*>* movingCubes) {
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

    /* If the mouse button is held the cube is displayed by the same movement as the camera.
    *  When the mouse button is no longer held the previously held cube will retrain the movement of the player or camer rotation.
    *  When a cube is moving it is inserted into the movingCubes list, where its movement will be processed every frame until it hits
    *  the ground. */
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (prevHeld && prevTargetedCube && prevTargetedCube != targetedCube) {
            prevTargetedCube->isMoving = true;
            prevTargetedCube->Velocity += calculateAngularVelocity(prevFront, camera.Front, (float)glfwGetTime() - lastMouseMov);
            movingCubes->insert(prevTargetedCube);
        }
        if (targetedCube) {
            targetedCube->isMoving = false;
            movingCubes->erase(targetedCube);

            targetedCube->Position += (camera.Position - previousPos);
            targetedCube->Velocity = (camera.Position - previousPos);
        }
        prevHeld = true;
    } else {
        if (prevHeld && prevTargetedCube) {
            prevTargetedCube->isMoving = true;
            prevTargetedCube->Velocity += calculateAngularVelocity(prevFront, camera.Front, (float)glfwGetTime() - lastMouseMov);
            movingCubes->insert(prevTargetedCube);
        }
        prevHeld = false;
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
    prevFront = camera.Front;
    // Pass on the mouse movement offsets which are then translated into camera rotations.
    camera.ProcessMouseMovement(xoffset, yoffset);
    // Saves the time when the last mouse movment ended, so that I can calculate a time span how long the most 
    // recent mouse movement lasted, by which I can scale the angular velocity.
    lastMouseMov = (float)glfwGetTime();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (targetedCube) {
            // Acquire vector pointing from camera to targeted cube
            glm::vec3 cameraToCube = targetedCube->Position - camera.Position;

            float dYaw = camera.Yaw - prevYaw;
            float dPitch = camera.Pitch - prevPitch;

            // Rotate cameraToCube vector by change in player direction.
            // dYaw is negated because a camera rotation the right results in a positive dYaw
            // but a positive dYaw means a rotation to the left. Rotating the camera downards 
            // makes dPitch positive, and positive dPatch also corresponds to a downwards rotation.
            glm::mat4 RAroundY = glm::rotate(glm::mat4(1.0f), -glm::radians(dYaw), glm::vec3(0, 1, 0));
            glm::mat4 RAroundRight = glm::rotate(glm::mat4(1.0f), glm::radians(dPitch), camera.Right);
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

// Arcane ChatGPT-written function that figures out the angular velocity the cube should be launched at after rotation. Understand and rewrite later.
glm::vec3 calculateAngularVelocity(glm::vec3 prevFront, glm::vec3 front, float mouseMovDelay) {
    // --- compute exact angular velocity (world-space) from cameraToCube -> rotated ---
    const float EPS = 1e-6f;
    const float dt = static_cast<float>(deltaTime); // make sure this is > 0

    // guard: if dt is zero, we can't compute omega
    if (dt <= 0.0f) {
        // handle appropriately (skip or set zero)
    }
    else {
        // normalize directions for robust axis/angle extraction
        glm::vec3 v = glm::normalize(prevFront);
        glm::vec3 v2 = glm::normalize(front);

        // cross and dot
        glm::vec3 c = glm::cross(v, v2);
        float s = glm::length(c);            // sin(theta)
        float d = glm::clamp(glm::dot(v, v2), -1.0f, 1.0f); // cos(theta), clamped for safety

        // angle in radians (robust for all ranges)
        float angle = std::atan2(s, d); // angle ∈ [0, π]

        glm::vec3 axis(0.0f);
        if (s > EPS) {
            // normal case: well-defined axis
            axis = c / s; // normalized cross
        }
        else {
            // s ~ 0 -> either no rotation or 180-degree rotation
            if (d > 0.0f) {
                // vectors are nearly identical => no rotation
                axis = glm::vec3(0.0f); // omega will be zero below
                angle = 0.0f;
            }
            else {
                // vectors are opposite (angle ≈ π). Need any axis orthogonal to v.
                // try camera.Right (already available) unless it's parallel to v.
                axis = glm::cross(v, camera.Right);
                if (glm::dot(axis, axis) < EPS) {
                    // camera.Right was colinear with v; fall back to world up
                    axis = glm::cross(v, glm::vec3(0.0f, 1.0f, 0.0f));
                    if (glm::dot(axis, axis) < EPS) {
                        // edge case: v is vertical; pick arbitrary orthogonal axis
                        axis = glm::cross(v, glm::vec3(1.0f, 0.0f, 0.0f));
                    }
                }
                axis = glm::normalize(axis);
                // angle already ≈ π from atan2(s,d) (s ≈ 0, d ≈ -1 -> atan2(0,-1) -> π)
                angle = glm::pi<float>();
            }
        }

        // angular velocity vector (rad/s)
        glm::vec3 omega = (angle == 0.0f) ? glm::vec3(0.0f) : (axis * (angle / mouseMovDelay));

        // r: vector from player's rotation center to object (world-space)
        glm::vec3 r = prevTargetedCube->Position - camera.Position; // same as 'rotated' but keep for clarity

        // compute release velocity
        return glm::cross(omega*0.005f, r);
    }
}