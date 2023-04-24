//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <stb_image/stb_image.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <iostream>
//#include <stdlib.h>
//#include <stdio.h>
//#include <windows.h>
//#include <time.h>
//#include <chrono>
//#include <thread>
//#include <openal/al.h>
//#include <openal/alc.h>
//#include "Shader.h"
//
////void setArray(float* array[]);
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//
////// Callback function for mouse button events
////void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
////{
////    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
////    {
////        double xpos, ypos;
////        glfwGetCursorPos(window, &xpos, &ypos);
////        std::cout << "Left mouse button pressed at (" << xpos << ", " << ypos << ")" << std::endl;
////    }
////}
////
////// Callback function for mouse movement events
//
//// array used in loop
//const int arrayLength = 200;
//float array[arrayLength];
//
//int randIter = 0;
//
//int bubbleIter = arrayLength + 1;
//int bubbleJter = arrayLength + 1;
//
//int insertI = arrayLength + 1;
//int insertJ = arrayLength + 1;
//
//void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
//
//float posx, posy;
//
//float offsetAgain;
//float offsetAgainAgain;
//
//// time difference between frames in milliseconds
//double timeTarget = 0.5;
//
//int vheight = 600;
//int vwidth = 600;
//
//int len = arrayLength;
//
//int main() {
//
//    ALCdevice* device = alcOpenDevice(NULL);
//    ALCcontext* context = alcCreateContext(device, NULL);
//    alcMakeContextCurrent(context);
//
//    const int SAMPLE_RATE = 44100;
//    const float DURATION = 0.5f; // seconds
//    int NUM_SAMPLES = SAMPLE_RATE * (timeTarget / 1000);
//    const float AMPLITUDE = 0.25f;
//    float FREQUENCY = 440.0f; // Hz
//    const float TAU = 6.28318530718f; // 2 * pi
//
//    ALuint buffer;
//    alGenBuffers(1, &buffer);
//
//    short* data = new short[NUM_SAMPLES];
//    //for (int i = 0; i < NUM_SAMPLES; i++)
//    //{
//    //    float t = (float)i / SAMPLE_RATE;
//    //    float sine_wave = AMPLITUDE * sinf(TAU * FREQUENCY * t);
//    //    data[i] = (short)(sine_wave * SHRT_MAX);
//    //}
//
//    alBufferData(buffer, AL_FORMAT_MONO16, data, NUM_SAMPLES * sizeof(short), SAMPLE_RATE);
//
//    ALuint source;
//    alGenSources(1, &source);
//    alSourcei(source, AL_BUFFER, buffer);
//    float maxh = 1.0f;
//    for (int i = 0; i < len; i++) {
//        glm::mat4 trans = glm::mat4(1.0f);
//        trans = glm::translate(trans, glm::vec3(-1.0f + (1 * inverseLen) + (2 * i * inverseLen), -1.0f, 0));
//        trans = glm::scale(trans, glm::vec3(inverseLen, (2 * (array[i] * inverseMax)) * maxh, 0));
//
//        if (i == selectedValue) {
//            newShader.setFloat("redShift", 0.0f);
//
//            FREQUENCY = (5 * inverseLen * array[i] + 1) * 440.0f;
//
//            //for (int i2 = 0; i2 < NUM_SAMPLES; i2++)
//            //{
//            //    float t = (float)i2 / SAMPLE_RATE;
//            //    float sine_wave = AMPLITUDE * sinf(TAU * FREQUENCY * t);
//            //    data[i2] = (short)(sine_wave * SHRT_MAX);
//            //}
//
//            int period = NUM_SAMPLES / array[i];
//            short waveform[2205];
//            for (int i3 = 0; i3 < NUM_SAMPLES; i3++) {
//                if (i3 % period < period / 2) {
//                    waveform[i3] = 2500;
//                }
//                else {
//
//                    waveform[i3] = -2500;
//
//                }
//            }
//
//
//            ALint numBuffersQueued = 0;
//            alGetSourcei(source, AL_BUFFERS_QUEUED, &numBuffersQueued);
//            while (numBuffersQueued--)
//            {
//                alSourceUnqueueBuffers(source, 1, &buffer);
//            }
//
//            alBufferData(buffer, AL_FORMAT_MONO16, waveform, NUM_SAMPLES * sizeof(short), SAMPLE_RATE);
//            //alBufferData(buffer, AL_FORMAT_MONO16, data, NUM_SAMPLES * sizeof(short), SAMPLE_RATE);
//
//            alSourceQueueBuffers(source, 1, &buffer);
//
//            alSourcei(source, AL_BUFFER, buffer);
//            alSourcePlay(source);
//
//
//
//            //int state;
//            //do {
//            //    alGetSourcei(source, AL_SOURCE_STATE, &state);
//            //} while (state == AL_PLAYING);
//
//
//        }
//        else {
//            newShader.setFloat("redShift", 1.0f);
//        }
//
//        unsigned int transformLocation = glGetUniformLocation(newShader.ID, "trans");
//        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    }
//}