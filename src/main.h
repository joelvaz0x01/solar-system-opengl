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

void renderSphere();

void renderOrbit(float radius, unsigned int *VAO);

void bindTexture(unsigned int texture);

glm::mat4 planetCreator(float translation, float distance, float rotation, float scale, glm::vec3 centerModel);

/// @brief Struct to store the properties of a planet
struct planetProperties {
    std::string name; ///< name of the planet
    float translation; ///< translation around the sun
    float distance; ///< distance from the sun
    float rotation; ///< rotation around its own axis
    float scale; ///< scale of the planet
};
