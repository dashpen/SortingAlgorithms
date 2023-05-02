#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <openal/al.h>
#include <openal/alc.h>
#include <cmath>
#include "Shader.h"

void onFrameSizeChange(GLFWwindow* window, int width, int height);
void checkKeys(GLFWwindow* window);
void insertionSortStep(int* selectedValue);
void selectionSortStep(int* selectedValue);
void bubbleSortStep(int* selectedValue);
void merge(int arr[], const int left, const int mid, const int right);

// fragment shader code
const char *fragShaderCode = "#version 330 core\n"
"out vec4 FragColor; \n"
"in vec3 ourColor; \n"
"uniform float redShift; \n"
"void main() {\n"
"FragColor = vec4(ourColor.x, ourColor.y * redShift, ourColor.z * redShift, 1.0); \n"
"}\0";

// vertex shader code
const char* vertShaderCode = "#version 330 core\n"
"layout(location = 0) in vec3 aPos; \n"
"layout(location = 1) in vec3 aColor; \n"
"out vec3 ourColor; \n"
"uniform mat4 trans; \n"
"void main(){\n"
"    gl_Position = trans * vec4(aPos, 1.0f); \n"
"    ourColor = aColor; \n"
"};\0";


// array used in loop
const int ARRAY_LENGTH = 32;
int array[ARRAY_LENGTH];

// initializing iterators for while loop
int randIter = 0;

int initIter = ARRAY_LENGTH + 1;

int bubbleIter = initIter;
int bubbleJter = initIter;

int insertI = initIter;
int insertJ = initIter;

int selectionI = initIter;
int selectionJ = initIter;
int selMin = 0;

int sizeM = initIter;
int leftM = initIter;

// time difference between frames in milliseconds
double timeTargetBig = 500;
double timeTargetSmall = 0.5;
double timeTarget = timeTargetSmall;

int vheight = 600;
int vwidth = 600;

// max amount of red lines
const int NUM_SOURCES = 2;

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(vwidth, vheight, "AMONG US!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onFrameSizeChange);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, vwidth, vheight);

    float vertices[] = {
        // positions          // colors           
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f, 1.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,  1.0f, 1.0f, 1.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,   // top left 
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

    // shader 

    // compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertShaderCode, NULL);
    glCompileShader(vertex);

    // errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Vertex Shader Compilation Failed\n" << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragShaderCode, NULL);
    glCompileShader(fragment);

    // errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Fragment Shader Compilation Failed\n" << infoLog << std::endl;
    }

    // shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader Linking Failed\n" << infoLog << std::endl;
    }

    // delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    srand(time(NULL));

    for (int i = 0; i < ARRAY_LENGTH; i++) {
        array[i] = i;
    }

    float len = sizeof(array) / sizeof(array[0]);
    float max = -1;

    for (int i = 0; i < len; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }

    // shuffle array
    for (int i = ARRAY_LENGTH - 1; i > 0; i--) {
        int j = rand() % i + 1;
        float temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
     
    randIter = static_cast<int>(len);

    int numFrames = 0;

    float inverseLen = 1 / len;
    float inverseMax = 1 / max;

    int selectedValues[NUM_SOURCES];
    for (int i = 0; i < NUM_SOURCES; i++) {
        selectedValues[i] = -1;
    }
    // rendering
    while (!glfwWindowShouldClose(window))
    {
        // for fps limiter (time target)
        double startTime = glfwGetTime();

        // input
        checkKeys(window);

        // rendering
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glUniform1f(glGetUniformLocation(shaderProgram, "redShift"), 1.0f);

        glBindVertexArray(VAO);

        // shuffling array
        if (randIter <= (len - 1)) {
            int j = rand() % (randIter + 1);
            int temp = array[randIter];
            array[randIter] = array[j];
            array[j] = temp;
            randIter++;
        }

        int selectedValue = -1;

        // bubble sort
        bubbleSortStep(&selectedValue);

        // insertion sort
        insertionSortStep(&selectedValue);

        // selection sort
        selectionSortStep(&selectedValue);

        // merge sort visualization
        if (sizeM <= ARRAY_LENGTH - 1) {
            if (leftM < ARRAY_LENGTH - 1) {
                int mid = min(leftM + sizeM - 1, ARRAY_LENGTH - 1);
                int right = min(leftM + 2 * sizeM - 1, ARRAY_LENGTH - 1);
                const int LLen = mid - leftM + 1;
                const int RLen = right - mid;

                int* L = new int[LLen];
                int* R = new int[RLen];

                for (int i = 0; i < LLen; i++) {
                    L[i] = array[leftM + i];
                }
                for (int j = 0; j < RLen; j++) {
                    R[j] = array[mid + 1 + j];
                }

                int LI = 0;
                int RI = 0;
                int arrI = leftM;

                while (LI < LLen && RI < RLen) {
                    if (L[LI] <= R[RI]) {
                        array[arrI] = L[LI];
                        LI++;
                    }
                    else {
                        array[arrI] = R[RI];
                        RI++;
                    }
                    arrI++;
                }

                while (LI < LLen) {
                    array[arrI] = L[LI];
                    LI++;
                    arrI++;
                }

                while (RI < RLen) {
                    array[arrI] = R[RI];
                    RI++;
                    arrI++;
                }
                delete[] L;
                delete[] R;
                leftM += 2 * sizeM;
            }
            else {
                sizeM *= 2;
                leftM = 0;
            }
        }

        // shift selectedValues list 1 element over
        for (int i = NUM_SOURCES - 1; i > 0; i--) {
            selectedValues[i] = selectedValues[i - 1];
        }
        selectedValues[0] = selectedValue;

        //selectedValues[9] = selectedValues[8];
        //selectedValues[8] = selectedValues[7];
        //...
        //selectedValues[2] = selectedValues[1];
        //selectedValues[1] = selectedValues[0];

        // max height for 'pillars' as a percent of viewport (1.0f is the view height)
        float maxh = 1.0f;

        for (int i = 0; i < len; i++) {
            // identity matrix
            glm::mat4 trans = glm::mat4(1.0f);
            // move block to correct position based on index
            trans = glm::translate(trans, glm::vec3(-1.0f + (1 * inverseLen) + (2 * i * inverseLen), -1.0f, 0));
            // scale block to correct width and height based on value
            trans = glm::scale(trans, glm::vec3(inverseLen, (2 * (array[i] * inverseMax)) * maxh, 0));

            // set values under selectedValues to red and white otherwise
            int redCheck = false;
            for (int k = 0; k < NUM_SOURCES; k++) {
                if (i == selectedValues[k]) {

                    glUniform1f(glGetUniformLocation(shaderProgram, "redShift"), 0.0f);
                    redCheck = true;
                }
            }

            if (!redCheck) {
                glUniform1f(glGetUniformLocation(shaderProgram, "redShift"), 1.0f);
            }

            unsigned int transformLocation = glGetUniformLocation(shaderProgram, "trans");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // framerate setting based on timeTarget
        double endTime = glfwGetTime();
        double diffTime = endTime - startTime;
        if (diffTime * 1000 < timeTarget) {
            int microsecs = static_cast<int>((timeTarget - (diffTime * 1000)) * 1000);
            std::this_thread::sleep_for(std::chrono::microseconds(microsecs));
        }

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

void resetArray() {
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        array[i] = i;
    }
}



void merge(int arr[], const int left, const int mid, const int right) {
    const int LLen = mid - left + 1;
    const int RLen = right - mid;

    int* L = new int[LLen];
    int* R = new int[RLen];

    // make left and right arrays
    for (int i = 0; i < LLen; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < RLen; j++) {
        R[j] = arr[mid + 1 + j];
    }

    int LI = 0;
    int RI = 0;
    int arrI = left;

    while (LI < LLen && RI < RLen) {
        if (L[LI] <= R[RI]) {
            arr[arrI] = L[LI];
            LI++;
        }
        else {
            arr[arrI] = R[RI];
            RI++;
        }
        arrI++;
    }

    while (LI < LLen) {
        arr[arrI] = L[LI];
        LI++;
        arrI++;
    }

    while (RI < RLen) {
        arr[arrI] = R[RI];
        RI++;
        arrI++;
    }
    delete[] L;
    delete[] R;
}

void bubbleSortStep(int* selectedValue)
{
    // bubble sort visualization
    if (bubbleIter < ARRAY_LENGTH) {
        if (bubbleJter < ARRAY_LENGTH - 1 - bubbleIter) {
            if (array[bubbleJter] > array[bubbleJter + 1]) {
                *selectedValue = bubbleJter + 1;
                int temp = array[bubbleJter];
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
}

void selectionSortStep(int* selectedValue)
{
    int selI = selectionI;
    int selJ = selectionJ;

    // selection sort visualization
    if (selI < ARRAY_LENGTH - 1) {
        int selMin = selI;
        for (int j = selI + 1; j < ARRAY_LENGTH; j++) {
            if (array[j] < array[selMin]) {
                selMin = j;
            }
        }

        if (selI != selMin) {
            *selectedValue = selMin;
            int temp = array[selMin];
            array[selMin] = array[selI];
            array[selI] = temp;

        }
    }
    selI++;
    selectionI = selI;
    selectionJ = selJ;
}

void insertionSortStep(int* selectedValue) {
    // insertion sort visualization
    if (insertI < ARRAY_LENGTH + 1) {
        if (array[insertJ] < array[insertJ - 1]) {
            *selectedValue = insertJ;
            int temp = array[insertJ];
            array[insertJ] = array[insertJ - 1];
            array[insertJ - 1] = temp;
            insertJ--;
        }
        else if (insertJ == 0 || array[insertJ] > array[insertJ - 1]) {
            insertJ = insertI;
            insertI++;
        }
    }
}

void onFrameSizeChange(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// checks which keys are pressed each frame
void checkKeys(GLFWwindow* window)
{
    // closes window when escape is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // fill mode
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    // wireframe mode
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // shuffle array
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        randIter = 0;
        timeTarget = timeTargetSmall;
    }
    // set array to sorted
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        resetArray();
    }
    // start bubble sort
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        bubbleIter = 0;
        bubbleJter = 0;
        timeTarget = timeTargetSmall;
    }
    // start insertion sort
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        insertI = 1;
        insertJ = 0;
        timeTarget = timeTargetSmall;
    }
    // start selection sort
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        selectionI = 0;
        selectionJ = 0;
        timeTarget = timeTargetBig;
    }
    // start merge sort
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        sizeM = 1;
        leftM = 0;
        timeTarget = timeTargetBig;
    }
}