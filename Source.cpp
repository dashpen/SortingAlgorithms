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

//void setArray(float* array[]);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//void mergeSort(int arr1[], const int start, const int end);
void mergeSort(int arr1[], int n);
//void splitMerge(int arr1[], int arr2[], int start, int end);
void merge(int arr[], const int left, const int mid, const int right);

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

// array used in loop
const int ARRAY_LENGTH = 100;
int array[ARRAY_LENGTH];

int randIter = 0;

int initIter = ARRAY_LENGTH + 1;

int bubbleIter = initIter;
int bubbleJter = initIter;

int insertI = initIter;
int insertJ = initIter;

int selectionI = initIter;
int selectionJ = initIter;

int sizeM = initIter;
int leftM = initIter;


void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

float posx, posy;

float offsetAgain;
float offsetAgainAgain;

// time difference between frames in milliseconds
double timeTarget = 100;

int vheight = 600;
int vwidth = 600;

const int NUM_SOURCES = 10;

int main() {

    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    const int SAMPLE_RATE = 22050;
    const float DURATION = 0.5f; // seconds
    int NUM_SAMPLES = SAMPLE_RATE * DURATION;
    const float AMPLITUDE = 0.25f;
    float FREQUENCY = 988.0f; // Hz
    const float TAU = 6.28318530718f; // 2 * pi

    ALuint buffer;
    alGenBuffers(1, &buffer);

    short* data = new short[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float sine_wave = AMPLITUDE * sinf(TAU * FREQUENCY * t);
        data[i] = (short)(sine_wave * SHRT_MAX/2);
    }
    //for (int i = 0; i < NUM_SAMPLES; i++) {
    //    if (i % 220 < 110) {
    //        data[i] = 2500;
    //    }
    //    else {
    //        data[i] = -2500;
    //    }
    //}


    alBufferData(buffer, AL_FORMAT_MONO16, data, NUM_SAMPLES * sizeof(short), SAMPLE_RATE);



    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    //alSourcePlay(source);

    ALuint buffer2;
    alGenBuffers(1, &buffer2);
    int NUM_SAMPLES2 = SAMPLE_RATE * 2.0f;

    short* data2 = new short[NUM_SAMPLES2];
    for (int i = 0; i < NUM_SAMPLES2; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float sine_wave = AMPLITUDE * sinf(TAU * FREQUENCY * t);
        data2[i] = (short)(sine_wave * SHRT_MAX);
    }

    alBufferData(buffer2, AL_FORMAT_MONO16, data2, NUM_SAMPLES2 * sizeof(short), SAMPLE_RATE);

    ALuint source2;
    alGenSources(1, &source2);
    alSourcei(source2, AL_BUFFER, buffer2);
    //alSourcePlay(source2);

    //int state;
    //do {
    //    alGetSourcei(source2, AL_SOURCE_STATE, &state);
    //} while (state == AL_PLAYING);

    ALuint alSources[NUM_SOURCES];
    alGenSources(NUM_SOURCES, alSources);

    for (int i = 0; i < NUM_SOURCES; i++) {
        alSourcei(alSources[i], AL_BUFFER, buffer);
    }



    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(vwidth, vheight, "AMONG US!", NULL, NULL);
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

    glViewport(0, 0, vwidth, vheight);

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
    float maxHeight = 0.8f;

    //for (int i = 0; i < len; i++) {
    //    array[i] = static_cast<float>(array[i]);
    //}


    for (int i = ARRAY_LENGTH - 1; i > 0; i--) {
        int j = rand() % i + 1;
        float temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
     
    randIter = static_cast<int>(len);

    double prevTime = glfwGetTime();
    int numFrames = 0;

    float inverseLen = 1 / len;
    float inverseMax = 1 / max;

    int speedMult = 1;

    int selectedValues[NUM_SOURCES];
    for (int i = 0; i < NUM_SOURCES; i++) {
        selectedValues[i] = -1;
    }

    int tempArr[ARRAY_LENGTH];
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        tempArr[i] = static_cast<int>(array[i]);
        std::cout << tempArr[i] << ",";
    }

    std::cout << "\n";

    mergeSort(tempArr, ARRAY_LENGTH);
    //mergeSort(tempArr, 0 , ARRAY_LENGTH - 1);

    for (int i = 0; i < ARRAY_LENGTH; i++) {
        std::cout << tempArr[i] << ",";
    }
    std::cout << "\n";

    // rendering
    while (!glfwWindowShouldClose(window))
    {
        // fps calculator
        double curTime = glfwGetTime();
        double deltaTime = curTime - prevTime;
        numFrames++;
        if (deltaTime >= 0.5) {
            double milli = deltaTime * 1000 / static_cast<double>(numFrames);
            std::string ms = std::to_string(milli);
            std::string fps = std::to_string(numFrames/deltaTime);
            std::string newTitle = "AMONG US! || Milliseconds: " + ms + " FPS: " + fps;
            glfwSetWindowTitle(window, newTitle.c_str());
            numFrames = 0;
            prevTime = curTime;
        }
        double startTime = glfwGetTime();


        // input
        processInput(window);

        // rendering
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(shaderProgram);

        newShader.setFloat("redShift", 1.0f);
        newShader.setFloat("greenShift", 1.0f);

        glBindVertexArray(VAO);

        if (randIter <= (len - 1)) {
            int j = rand() % (randIter + 1);
            int temp = array[randIter];
            array[randIter] = array[j];
            array[j] = temp;
            randIter++;
        }

        int selectedValue = -1;

        //for (int i = 0; i < speedMult; i++) {
            if (bubbleIter < len) {
                if (bubbleJter < len - 1 - bubbleIter) {
                    if (array[bubbleJter] > array[bubbleJter + 1]) {
                        selectedValue = bubbleJter + 1;
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
            else {
                //alSourceStopv(10, alSources);
            }
        //}

        int insI = insertI;
        int insJ = insertJ;

        if (insertI < len + 1) {
            if (array[insJ] < array[insJ - 1]) {
                selectedValue = insJ;
                int temp = array[insJ];
                array[insJ] = array[insJ - 1];
                array[insJ - 1] = temp;
                insJ--;
            }
            else if (insJ == 0 || array[insJ] > array[insJ - 1]) {
                insJ = insI;
                insI++;
            }
        }
        else {
            //alSourceStopv(10, alSources);
        }

        //int selI = selectionI;
        //int selJ = selectionJ;

        //if (selI < len - 1) {
        //    int selMin = selI;
        //    //for (int j = selI + 1; j < len; j++) {

        //    //    if (array[j] < array[selMin]) {
        //    //        selMin = j;
        //    //    }
        //    //}

        //    if (selI != selMin) {
        //        selectedValue = selMin;
        //        int temp = array[selMin];
        //        array[selMin] = array[selI];
        //        array[selI] = temp;
        //    }

        //}
        //selI++;
        //selectionI = selI;
        //selectionJ = selJ;
        //std::cout << selI;
        int selI = selectionI;
        int selJ = selectionJ;

        if (selI < len - 1) {
            int selMin = selI;

            if (selJ < ARRAY_LENGTH) {

                if (array[selJ] < array[selMin]) {
                    selMin = selJ;
                }
                selectedValue = selJ;
                selJ++;
            }
            else {
                selI++;
                selJ = selI + 1;
                if (selI != selMin) {
                    selectedValue = selMin;
                    int temp = array[selMin];
                    array[selMin] = array[selI];
                    array[selI] = temp;
                }
            }
            //for (int j = selI + 1; j < len; j++) {

            //    if (array[j] < array[selMin]) {
            //        selMin = j;
            //    }
            //}



        }
        selectionI = selI;
        selectionJ = selJ;
        insertI = insI;
        insertJ = insJ;

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
        else {
            timeTarget = 1;
        }
        //for (curr_size = 1; curr_size <= n - 1; curr_size *= 2) {
        //    for (left = 0; left < n - 1; left += 2 * curr_size) {
        //        int mid = min(left + curr_size - 1, n - 1);
        //        int right = min(left + 2 * curr_size - 1, n - 1);

        //        merge(array, left, mid, right);
        //    }
        //}


        for (int i = NUM_SOURCES - 1; i > 0; i--) {
            selectedValues[i] = selectedValues[i - 1];
        }
        selectedValues[0] = selectedValue;

        //selectedValues[9] = selectedValues[8];
        //selectedValues[8] = selectedValues[7];
        //...
        //selectedValues[2] = selectedValues[1];
        //selectedValues[1] = selectedValues[0];


        //float maxh = static_cast<float>(vwidth)/static_cast<float>(vheight);
        float maxh = 1.0f;

        int sourceIndex = (numFrames % NUM_SOURCES);

        for (int i = 0; i < len; i++) {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(-1.0f + (1 * inverseLen) + (2 * i * inverseLen), -1.0f, 0));
            trans = glm::scale(trans, glm::vec3(inverseLen, (2 * (array[i] * inverseMax)) * maxh, 0));

            int redCheck = false;
            for (int k = 0; k < NUM_SOURCES; k++) {
                if (i == selectedValues[0]) {
                    float arrayScale = (float)array[i] / (float)ARRAY_LENGTH;
                    float pitch = static_cast<float>(0.8f * arrayScale);
                    alSourcePause(alSources[sourceIndex]);
                    alSourcef(alSources[sourceIndex], AL_PITCH, pitch);
                    alSourcePlay(alSources[sourceIndex]);
                }
                if (i == selectedValues[k]) {

                    newShader.setFloat("redShift", 0.0f);
                    redCheck = true;
                }
            }

            if (!redCheck) {
                newShader.setFloat("redShift", 1.0f);
            }

            //if (i == selectedValue) {
            //    newShader.setFloat("greenShift", 0.0f);
            //}

            unsigned int transformLocation = glGetUniformLocation(newShader.ID, "trans");
            glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

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

    alDeleteSources(1, &source);
    alDeleteSources(10, alSources);
    alDeleteBuffers(1, &buffer);
    delete[] data;

    alcDestroyContext(context);
    alcCloseDevice(device);

    glfwTerminate();
    return 0;
}

//void setArray(float* array[]) {
//    int ARRAY_LENGTH = sizeof(*array) / sizeof(*array[0]);
//    for (int i = 0; i < ARRAY_LENGTH; i++) {
//        *array[i] = static_cast<float>(i);
//    }
//}

void resetArray() {
    int ARRAY_LENGTH = sizeof(array) / sizeof(array[0]);
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        array[i] = i;
    }
}

//void mergeSort(int arr1[], int arr2[])
//{
//    int length = ARRAY_LENGTH;
//    splitMerge(arr1, arr2, 0, length);
//
//}
//void splitMerge(int arr1[], int arr2[], int start, int end)
//{
//    if (end - start <= 1) return;
//    int mid = (start + end) / 2;
//
//    splitMerge(arr1, arr2, start, mid);
//    splitMerge(arr1, arr2, mid+1, end);
//
//    merge(arr1, arr2, start, mid, end);
//}
//
//void merge(int arr1[], int arr2[], int start, int mid, int end)
//{
//    int i = start;
//    int j = mid;
//    for (int k = start; k < end; k++) {
//        if (i < mid && (j >= end || arr1[i] <= arr1[j])) {
//            arr2[k] = arr1[i];
//            i++;
//        }
//        else {
//            arr2[k] = arr1[j];
//            j++;
//        }
//    }
//}

void merge(int arr[], const int left, const int mid, const int right) {
    const int LLen = mid - left + 1;
    const int RLen = right - mid;

    int* L = new int[LLen];
    int* R = new int[RLen];

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

void mergeSort(int arr[], int n) {
    int curr_size;
    int left;

    for (curr_size = 1; curr_size <= n - 1; curr_size *= 2) {
        for (left = 0; left < n - 1; left += 2 * curr_size) {
            int mid = min(left + curr_size - 1, n - 1);
            int right = min(left + 2 * curr_size - 1, n - 1);

            merge(arr, left, mid, right);
        }
    }
}

//void mergeSort(int arr[], const int start, const int end) {
//    if (start >= end) return;
//    int mid = start + (end - start) / 2;
//    mergeSort(arr, start, mid);
//    mergeSort(arr, mid + 1, end);
//    merge(arr, start, mid, end);
//}

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
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        insertI = 1;
        insertJ = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        selectionI = 0;
        selectionJ = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        sizeM = 1;
        leftM = 0;
        timeTarget = 100;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    std::cout << "Mouse moved to (" << xpos << ", " << ypos << ")" << std::endl;
    posy = static_cast<float>(ypos);
    posx = static_cast<float>(xpos);
}