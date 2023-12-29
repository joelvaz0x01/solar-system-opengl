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

void renderText(Shader &shader, std::string text, float x, float y, float textScale, glm::vec3 color);

void bindTexture(unsigned int texture);

glm::mat4 planetCreator(float translation, float distance, float rotation, float scale, glm::vec3 centerModel);

/// Store the properties of a planet
struct planetProperties {
    std::string name; ///< name of the planet
    float translation; ///< translation around the sun
    float distance; ///< distance from the sun
    float rotation; ///< rotation around its own axis
    float scale; ///< scale of the planet
};

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int textureID; ///< ID handle of the glyph texture
    glm::ivec2 size; ///< Size of glyph
    glm::ivec2 bearing; ///< Offset from baseline to left/top of glyph
    unsigned int advance; ///< Horizontal offset to advance to next glyph
};

float charSpaceScaled(float textLength, float scale);

float charHeightScaled(float scale);
