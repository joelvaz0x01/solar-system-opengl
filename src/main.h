/**
 * @file main.h
 * @brief This file contains the function prototypes.
 * @details This file contains the function prototypes for the functions defined in main.c.
 *
 * @author joelvaz0x01
 * @author BrunoFG1
 *
 */

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

void processInput(GLFWwindow *window);

std::string getResourcePath(const std::string& relativePath);

unsigned int loadTexture(char const *path);

unsigned int loadCubeMap(char const **path);

void renderSphere();

void renderOrbit(float radius, unsigned int *VAO);

void renderText(Shader &shader, std::string text, float x, float y, float textScale, glm::vec3 color);

void renderSkybox(unsigned int skyboxCubeMap);

void bindTexture(unsigned int texture);

glm::mat4 planetCreator(float translation, float distance, float rotation, float scale, glm::vec3 centerModel);

/// Store the properties of a planet
struct planetProperties {
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

/// Struct for planet information
struct planetInfo {
    std::string name; ///< name of the planet
    std::string distance; ///< distance from the sun to the planet
    std::string radius; ///< radius of the planet
    std::string moons; ///< number of moons the planet has
    std::string rotationPeriod; ///< rotation period around its own axis
    std::string orbitalPeriod; ///< orbital period around the sun
};

float charHeightScaled(float scale, bool isMaxHeight);

float charWidthScaled(float scale, std::basic_string<char>::size_type textLength, bool isMaxWidth);

void showPlanetInfo(Shader &shader, unsigned int planetIndex, glm::vec3 textColor, float textScale);
