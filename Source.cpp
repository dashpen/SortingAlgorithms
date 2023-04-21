#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "Shader.h"

void setArray(float* array[]);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//// Callback function for mouse button events
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
//{
//    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//    {
//        double xpos, ypos;
//        glfwGetCursorPos(window, &xpos, &ypos);
//        std::cout << "Left mouse button pressed at (" << xpos << ", " << ypos << ")" << std::endl;
//    }
//}
//
//// Callback function for mouse movement events
int arrayLength = 300;
float array[300];
int randIter = 0;
int bubbleIter = 0;
int bubbleJter = 0;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

float posx, posy;

float offsetAgain;
float offsetAgainAgain;

int vheight = 600;
int vwidth = 600;

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(vheight, vwidth, "AMONG US!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, vheight, vwidth);

    Shader newShader("shader.vert", "shader.frag");
    float vertices[] = {
        // positions          // colors           
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   // top right
         1.0f,  -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,  // bottom right
         -1.0f,  -1.0f, 0.0f,   1.0f, 1.0f, 1.0f, // bottom left
         -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set the callback functions for mouse events
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetCursorPosCallback(window, cursor_position_callback);

    // wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    newShader.use();


    //glm::mat4 trans = glm::mat4(1.0f);
    //trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0));

    //unsigned int transformLocation = glGetUniformLocation(newShader.ID, "trans");
    //glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));

    //float array[] = { 1.0f, 2.0f, 3.0f, 4.0f };


    srand(time(NULL));

    for (int i = 0; i < arrayLength; i++) {
        array[i] = static_cast<float>(i);
    }

    float len = sizeof(array) / sizeof(array[0]);
    float max = -1.0;

    for (int i = 0; i < len; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    float maxHeight = 0.8f;

    for (int i = 0; i < len; i++) {
        array[i] = static_cast<float>(array[i]);
    }


    for (int i = arrayLength - 1; i > 0; i--) {
        int j = rand() % i + 1;
        float temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
     
    randIter = static_cast<int>(len);

    double prevTime = glfwGetTime();
    int numFrames = 0;

    float lenSize = 1 / len;
    float inverseMax = 1 / max;

    // rendering
    while (!glfwWindowShouldClose(window))
    {
        // fps calculator
        double curTime = glfwGetTime();
        double deltaTime = curTime - prevTime;
        numFrames++;
        if (deltaTime >= 1) {
            double milli = deltaTime * 1000 / static_cast<double>(numFrames);
            std::string ms = std::to_string(milli);
            std::string fps = std::to_string(1000.0 / milli);
            std::string newTitle = "AMONG US! || Milliseconds: " + ms + " Frames Per Second: " + fps;
            glfwSetWindowTitle(window, newTitle.c_str());
            numFrames = 0;
            prevTime = curTime;
        }


        // input
        processInput(window);

        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(shaderProgram);

        newShader.setFloat("offsetAgain", offsetAgain);
        newShader.setFloat("offsetAgainAgain", offsetAgainAgain);

        glBindVertexArray(VAO);

        if (randIter < (len - 1)) {
            int j = rand() % (randIter + 1);
            float temp = array[randIter];
            array[randIter] = array[j];
            array[j] = temp;
            randIter++;
            Sleep(5);
        }

        if (bubbleIter < len) {
            if (bubbleJter < len - 1 - bubbleIter) {
                if (array[bubbleJter] > array[bubbleJter + 1]) {
                    float temp = array[bubbleJter];
                    array[bubbleJter] = array[bubbleJter + 1];
                    array[bubbleJter + 1] = temp;
                }
                bubbleJter++;
            }
            else {
                bubbleJter = 0;
                bubbleIter++;
            }
        }

        float maxh = 1.0f;
        for (int i = 0; i < len; i++) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(-1.0f + 1 * lenSize + 2 * i * lenSize, -1.0f, 0));
            trans = glm::scale(trans, glm::vec3(lenSize, 2 * array[i] * inverseMax * maxh, 0));

            unsigned int transformLocation = glGetUniformLocation(newShader.ID, "trans");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void setArray(float* array[]) {
    int arrayLength = sizeof(*array) / sizeof(*array[0]);
    for (int i = 0; i < arrayLength; i++) {
        *array[i] = static_cast<float>(i);
    }
}

void resetArray() {
    int arrayLength = sizeof(array) / sizeof(array[0]);
    for (int i = 0; i < arrayLength; i++) {
        array[i] = static_cast<float>(i);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        offsetAgain += 0.0001f;
        if (offsetAgain > 1) {
            offsetAgain = 1;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        offsetAgain -= 0.0001f;
        if (offsetAgain < 0) {
            offsetAgain = 0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        offsetAgainAgain += 0.0001f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        offsetAgainAgain -= 0.0001f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        offsetAgainAgain -= 0.0001f;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        randIter = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        resetArray();
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        bubbleIter = 0;
        bubbleJter = 0;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    std::cout << "Mouse moved to (" << xpos << ", " << ypos << ")" << std::endl;
    posy = static_cast<float>(ypos);
    posx = static_cast<float>(xpos);
}