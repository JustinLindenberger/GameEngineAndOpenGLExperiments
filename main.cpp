#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSourceOrange = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

const char* fragmentShaderSourceBlue = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
"}\0";

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void initShaders(unsigned int shaderProgram, unsigned int color)
{
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates a shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //places the source code from vertexShaderSource inside of vertexShader
    glCompileShader(vertexShader);

    //Check if shader compilation was successful
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (color == 0) {
        glShaderSource(fragmentShader, 1, &fragmentShaderSourceOrange, NULL);
    } else {
        glShaderSource(fragmentShader, 1, &fragmentShaderSourceBlue, NULL);
    }
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
    }
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

    unsigned int shaderProgramOrange, shaderProgramBlue;
    shaderProgramOrange = glCreateProgram();
    shaderProgramBlue = glCreateProgram();
    initShaders(shaderProgramOrange, 0);
    initShaders(shaderProgramBlue, 1);

    float vertices[] = {
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
         0.25f,  0.5f, 0.0f,
         0.75f,  0.5f, 0.0f,
         0.75f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    unsigned int indices1[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        3, 4, 5
    };
    unsigned int indices2[] = {  // note that we start from 0!
        0, 1, 5,   // first triangle
        3, 4, 2
    };
    unsigned int VAO1crazy, VAO1proper;
    glGenVertexArrays(1, &VAO1crazy);
    glGenVertexArrays(1, &VAO1proper);
    unsigned int VBO1;
	glGenBuffers(1, &VBO1); //Generates buffer objects and stores the IDs in VBO array
    unsigned int EBOcrazy, EBOproper;
    glGenBuffers(1, &EBOcrazy);
    glGenBuffers(1, &EBOproper);

    glBindVertexArray(VAO1crazy); //This keeps track of all the following configuration on VBO1
    glBindBuffer(GL_ARRAY_BUFFER, VBO1); //Binds the buffer object to the GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copies data into the buffer on the GPU
    //This tells the shader program how to read in the vertices contained in GL_ARRAY_BUFFER
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); //By default vertices are disabled, maybe this means that they don't get rendered? So they need to be enabled.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOcrazy);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    glBindVertexArray(VAO1proper);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOproper);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    /*
    float EBOvertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f,  // top left 
     0.4f,  0.4f, 0.0f,
    -0.4f, -0.4f, 0.0f
    };
    unsigned int EBOindices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3,    // second triangle
        4, 5, 1
    };
    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(EBOvertices), EBOvertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(EBOindices), EBOindices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    */

    int oscilator = 0;

    while (!glfwWindowShouldClose(window))
    {
		processInput(window);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (oscilator % 10000 < 5000) {
            glUseProgram(shaderProgramOrange);
            glBindVertexArray(VAO1proper);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } else {
            glBindVertexArray(VAO1crazy);
            glUseProgram(shaderProgramBlue);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        oscilator++;
    }

    glfwTerminate();
    return 0;
}

