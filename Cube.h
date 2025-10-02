#ifndef CUBE_H
#define CUBE_H

#include "glm/glm.hpp"
#include <glad/glad.h>
#include "Shader.h"
#include <iostream>
#include <cmath>

// Vertices and normal vectors for the triangles that compose the cube, each block of 6 corresponds to one face.
const float cubeVertices[] = {
        // Back
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        // Front
        -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        // Left
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        // Right
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        // Bottom
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        // Top
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

/*
// Vertices for the triangles that compose the cube, each block of 6 corresponds to one face.
const float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
};
*/

class Cube
{
private:
    // Shader that will be applied to this cube, we need to know the shader class so that we can modify its uniform variables.
    Shader* shader;

    void checkCollision() {

    }

public:
    unsigned int VBO, VAO;
    glm::vec3 Position;
    // Is the player looking at this cube? true if yes (cube colored red), false if no (cube colored white).
    bool targeted;
    bool isMoving;
    bool isHeld;
    bool movable;
    glm::vec3 Velocity;
    const char* Name;

    // This checks whether the line of sight (the line of the front vector) of the player intersects with any cube faces.
    bool isCubeTargeted(glm::vec3 cameraPos, glm::vec3 cameraFront) {
        for (int axis = 0; axis < 3; ++axis) {
            float dir = cameraFront[axis];
            if (dir == 0) continue;                         // Front is prallel in to this plane, skip to prevent Divide by Zero.

            float faceOffset = (dir > 0.0f) ? -0.5f : 0.5f; // Check whether nearer or farther cube face is closer to camera so we only check one plane.
            float planePos = Position[axis] + faceOffset;
            float t = (planePos - cameraPos[axis]) / dir;   // Determine scalar at which the front vector intersects the cube plane.

            if (t < 0.0f) continue;                         // Scalar is negative, so cube is behind camera.

            // Compute intersection point coordinates on the other two axes.
            int a1 = (axis + 1) % 3;
            int a2 = (axis + 2) % 3;
            float interA1 = cameraPos[a1] + t * cameraFront[a1];
            float interA2 = cameraPos[a2] + t * cameraFront[a2];

            // Check whether the intersections are within the cube face.
            if (interA1 > (Position[a1] - 0.5f) && interA1 < (Position[a1] + 0.5) &&
                interA2 >(Position[a2] - 0.5f) && interA2 < (Position[a2] + 0.5)) {
                targeted = true;
                return true;
            }
        }
        targeted = false;
        return false;
    }

    // Upon construction load all the vertex data into a buffer for quick retrival later on.
    Cube(float x, float y, float z, Shader& sha, const char* name, bool mov) : shader(&sha) {
        Position = glm::vec3(x, y, z);
        targeted = false;
        isMoving = false;
        isHeld = false;
        movable = mov;
        Name = name;
        Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3* sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void drawCube() {
        // Load the buffer containing the cube vertex data. (Actually not necessary at the moment, since all objects use identical vertex data.
        glBindVertexArray(VAO);

        // This matrix translates the cubes vertex coordinates to its actual position.
        glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
        shader->setMatrix4fv("model", model);

        // Sets the "color" uniform bool variable in the shader program, which changes to color to red.
        if (targeted) {
            shader->setInt("color", 1);
        }
        else {
            shader->setInt("color", 0);
        }
        if (isMoving) {
            shader->setInt("color", 2);
            // std::cout << this << " is moving" << std::endl;
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    bool processMovement(float dTime) {
        Velocity += glm::vec3(0.0, -dTime * 0.5, 0.0);
        Position += Velocity;
        if (Position.y < 0.5) {
            Position.y = 0.5;
            Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            isMoving = false;
            return false;
        }
        return true;
    }
};

#endif