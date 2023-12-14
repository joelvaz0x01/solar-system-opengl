/**
 * @file main.h
 * @brief This file contains the function prototypes.
 * @details This file contains the function prototypes for the functions defined in main.c.
 *
 * @author joelvaz0x01
 * @author BrunoFG1
 * @date 2023-12-14
 *
 */

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

void processInput(GLFWwindow *window);

unsigned int loadTexture(const char *path);
