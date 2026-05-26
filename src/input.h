#ifndef SOLAR_SYSTEM_INPUT_H
#define SOLAR_SYSTEM_INPUT_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

/**
 * @brief Function to process input
 * @param window: window to process input
 */
void process_input(GLFWwindow* window);

/**
 * @brief Function to resize window size if changed
 * @param window: window to resize
 * @param width: new width
 * @param height: new height
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * @brief Function to process mouse movement
 * @param window: window to process mouse movement
 * @param x_pos: x position of mouse
 * @param y_pos: y position of mouse
 */
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

/**
 * @brief Function to process mouse scroll-wheel
 * @param window: window to process mouse scroll
 * @param x_offset: x offset of mouse scroll
 * @param y_offset: y offset of mouse scroll
 */
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

#endif  // SOLAR_SYSTEM_INPUT_H
