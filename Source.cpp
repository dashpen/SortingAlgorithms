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
#include "Shader.h"

//void setArray(float* array[]);
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

// array used in loop
const int arrayLength = 300;
float array[arrayLength];

int randIter = 0;

int bubbleIter = arrayLength + 1;
int bubbleJter = arrayLength + 1;

int insertI = arrayLength + 1;
int insertJ = arrayLength + 1;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

float posx, posy;

float offsetAgain;
float offsetAgainAgain;

// time difference between frames in milliseconds
double timeTarget = 0.25;

int vheight = 600;
int vwidth = 600;

const int NUM_SOURCES = 10;

int main() {

    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    const int SAMPLE_RATE = 88200;
    const float DURATION = 0.5f; // seconds
    int NUM_SAMPLES = SAMPLE_RATE * DURATION;
    const float AMPLITUDE = 0.25f;
    float FREQUENCY = 880.0f; // Hz
    const float TAU = 6.28318530718f; // 2 * pi

    ALuint buffer;
    alGenBuffers(1, &buffer);

    short* data = new short[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float sine_wave = AMPLITUDE * sinf(TAU * FREQUENCY * t);
        data[i] = (short)(sine_wave * SHRT_MAX);
    }

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

    float inverseLen = 1 / len;
    float inverseMax = 1 / max;

    int speedMult = 1;

    int selectedValues[NUM_SOURCES];
    for (int i = 0; i < NUM_SOURCES; i++) {
        selectedValues[i] = -1;
    }


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
            float temp = array[randIter];
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
            else {
                //alSourceStopv(10, alSources);
            }
        //}

        int insI = insertI;
        int insJ = insertJ;

        if (insertI < len + 1) {
            if (array[insJ] < array[insJ - 1]) {
                selectedValue = insJ;
                float temp = array[insJ];
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

        insertI = insI;
        insertJ = insJ;

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

            if (i == selectedValue) {
                //newShader.setFloat("redShift", 0.0f);

                //FREQUENCY = (5 * inverseLen * array[i] + 1) * 440.0f;
                //const int SAMPLES = 2205;

                //for (int i2 = 0; i2 < NUM_SAMPLES; i2++)
                //{
                //    float t = (float)i2 / SAMPLE_RATE;
                //    float sine_wave = AMPLITUDE * sinf(TAU * FREQUENCY * t);
                //    data[i2] = (short)(sine_wave * SHRT_MAX);
                //}
                //double mult = 1 / (1000 * array[i]);
                //int period = SAMPLES * mult * 1000;
                //short waveform[SAMPLES];
                //for (int i3 = 0; i3 < SAMPLES; i3++) {
                //    if ((i3 % period) < period / 2) {
                //        waveform[i3] = 2500;
                //    }
                //    else {

                //        waveform[i3] = -2500;

                //    }
                //}


                //ALint numBuffersQueued = 0;
                //alGetSourcei(source, AL_BUFFERS_QUEUED, &numBuffersQueued);
                //while (numBuffersQueued--)
                //{
                //    alSourceUnqueueBuffers(source, 1, &buffer);
                //}

                //alBufferData(buffer, AL_FORMAT_MONO16, waveform, SAMPLES * sizeof(short), SAMPLE_RATE);
                //alBufferData(buffer, AL_FORMAT_MONO16, data, NUM_SAMPLES * sizeof(short), SAMPLE_RATE);

                //alSourceQueueBuffers(source, 1, &buffer);
                //alSourcePause(source);

                //alSourcef(source, AL_PITCH, array[i]/arrayLength);
                //alSourcei(source, AL_BUFFER, buffer);
                //alSourcePlay(source);
                //std::this_thread::sleep_for(std::chrono::microseconds(500));




                //int state;
                //do {
                //    alGetSourcei(source, AL_SOURCE_STATE, &state);
                //} while (state == AL_PLAYING);


            }
            int check = false;
            for (int k = 0; k < NUM_SOURCES; k++) {
                if (i == selectedValues[0]) {
                    alSourcePause(alSources[sourceIndex]);
                    alSourcef(alSources[sourceIndex], AL_PITCH, static_cast<float>((float)array[i] / (float)arrayLength) + 0.05f);
                    alSourcePlay(alSources[sourceIndex]);
                }
                if (i == selectedValues[k]) {

                    newShader.setFloat("redShift", 0.0f);
                    check = true;
                }
             }
            if (!check) {
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
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>((timeTarget - (diffTime * 1000)) * 1000)));
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
//    int arrayLength = sizeof(*array) / sizeof(*array[0]);
//    for (int i = 0; i < arrayLength; i++) {
//        *array[i] = static_cast<float>(i);
//    }
//}

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
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        insertI = 1;
        insertJ = 0;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    std::cout << "Mouse moved to (" << xpos << ", " << ypos << ")" << std::endl;
    posy = static_cast<float>(ypos);
    posx = static_cast<float>(xpos);
}