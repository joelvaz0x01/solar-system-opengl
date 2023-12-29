/**
 * @file main.cpp
 * @brief Solar System made with OpenGL
 *
 * Controls:
 * - W, A, S and D keys: move the camera
 * - Q key: move the camera down
 * - E key: move the camera up
 * - Mouse: look around
 * - Mouse scroll-wheel: zoom in and out
 * - ESC key: close the window
 *
 * @author joelvaz0x01
 * @author BrunoFG1
 * @date 2023-12-14
 *
 */

#include <iostream>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION ///< to avoid linker errors

#include <stb_image.h>
#include <shader_m.h>
#include <camera.h>

#include "main.h"

#define PI 3.14159265359f ///< pi number
#define STEP 64 ///< increase to improve shape quality

#define WIDTH 1920 ///< width of the screen
#define HEIGHT 1080 ///< height of the screen

#define CHAR_SPACE 27.0f ///< space occupied by each character
#define CHAR_HEIGHT 25.0f ///< max height of each character

/// planet properties
planetProperties planetProp[] = {
        {"mercury", 2.0f, 2.0f, 2.0f, 0.1f}, // mercury
        {"venus",   1.5f, 3.0f, 2.0f, 0.1f}, // venus
        {"earth",   1.0f, 4.0f, 2.0f, 0.1f}, // earth
        {"mars",    0.8f, 5.0f, 2.0f, 0.1f}, // mars
        {"jupiter", 0.6f, 6.0f, 2.0f, 0.3f}, // jupiter
        {"saturn",  0.3f, 7.0f, 2.0f, 0.3f}, // saturn
        {"uranus",  0.2f, 8.0f, 2.0f, 0.3f}, // uranus
        {"neptune", 0.1f, 9.0f, 2.0f, 0.3f}  // neptune
};

/// moon properties
planetProperties moonProp = {"moon", 5.0f, 0.3f, 2.0f, 0.05f};

glm::mat4 view = glm::mat4(1.0f); ///< view matrix
glm::mat4 projection = glm::mat4(1.0f); ///< projection matrix

Camera camera(glm::vec3(0.0f, 2.0f, 20.0f)); ///< camera position
double lastX = WIDTH / 2.0f; ///< last x position of the mouse
double lastY = HEIGHT / 2.0f; ///< last y position of the mouse
bool firstMouse = true; ///< check if it's the first time moving the mouse

double deltaTime = 0.0f; ///< time between current frame and last frame
double lastFrame = 0.0f; ///< time of last frame

unsigned int sphereVAO = 0; ///< vertex array object for sphere
GLsizei indexCount; ///< number of indices for sphere

unsigned int orbitVAO[] = {0, 0, 0, 0, 0, 0, 0, 0}; ///< vertex array object for orbit
unsigned int moonOrbitVAO = 0; ///< vertex array object for moon's orbit

std::map<GLchar, Character> Characters; ///< map for FreeType character
unsigned int textVAO, textVBO; ///< vertex array object and vertex buffer object for text

/** Main function that is responsible for the execution of the solar system
 *
 * @return 0 if successful, -1 otherwise
 *
 */
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Cube Texture", glfwGetPrimaryMonitor() /*nullptr*/, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // per-sample processing operation performed after the Fragment Shader
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // compile shaders
    Shader planet("shaders/planetVertex.glsl", "shaders/planetFragment.glsl");
    Shader sun("shaders/sunVertex.glsl", "shaders/sunFragment.glsl");
    Shader orbit("shaders/orbitVertex.glsl", "shaders/orbitFragment.glsl");
    Shader text("shaders/textVertex.glsl", "shaders/textFragment.glsl");

    //load freetype
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    //load font
    FT_Face face;
    if (FT_New_Face(ft, "resources/fonts/MPLUSRounded1c-Bold.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    } else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load the first 128 characters of ASCII set
        for (GLubyte c = 0; c < 128; c++) {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
                continue;
            }

            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    (GLsizei) face->glyph->bitmap.width,
                    (GLsizei) face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // store character for later use
            Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure textVAO/textVBO for texture quads
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *) nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load planet textures
    unsigned int sunTexture = loadTexture("resources/textures/sun.jpg");

    // load planet textures
    unsigned int planetTextures[] = {
            loadTexture("resources/textures/mercury.jpg"),
            loadTexture("resources/textures/venus.jpg"),
            loadTexture("resources/textures/earth.jpg"),
            loadTexture("resources/textures/mars.jpg"),
            loadTexture("resources/textures/jupiter.jpg"),
            loadTexture("resources/textures/saturn.jpg"),
            loadTexture("resources/textures/uranus.jpg"),
            loadTexture("resources/textures/neptune.jpg")
    };

    // load earth's moon texture
    unsigned int moonTexture = loadTexture("resources/textures/moon.jpg");

    // load saturn's ring texture
    unsigned int saturnRingTexture = loadTexture("resources/textures/saturn_ring.png");

    // number of planets
    unsigned int planetCount = sizeof(planetTextures) / sizeof(planetTextures[0]);

    // model matrix for each planet
    auto *planetModel = new glm::mat4[planetCount];

    // light configuration
    sun.use();
    sun.setInt("texture1", 0);

    // planet shader configuration
    planet.use();
    planet.setInt("material.diffuse", 0);
    planet.setInt("material.specular", 1);

    // phong lighting declaration
    glm::vec3 lightColor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;

    // light properties (sun)
    glm::vec3 sunPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sunLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 sunModel = glm::mat4(1.0f);

    // orbit properties
    glm::mat4 orbitModel = glm::mat4(1.0f);

    // text properties
    std::string startText = "Solar System";
    auto startTextLength = static_cast<float>(startText.length());
    float startTextScale = 0.8f;
    glm::vec3 textColor = glm::vec3(0.9f, 0.9f, 0.9f); // light-gray color

    // NOTE: to render fixed text, projection matrix must be orthographic (2D) instead of perspective (3D)
    // in this case: 0 <= x <= WIDTH && 0 <= y <= HEIGHT
    projection = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

    text.use();
    text.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        // light properties (phong shading)
        lightColor = sunLightColor;
        diffuseColor = lightColor * glm::vec3(0.8f);
        ambientColor = diffuseColor * glm::vec3(0.1f);

        // light properties (sun)
        sun.use();
        sun.setVec3("color", lightColor);
        sun.setMat4("projection", projection);
        sun.setMat4("view", view);
        sunModel = glm::translate(glm::mat4(1.0f), sunPosition);
        sunModel = glm::rotate(sunModel, (float) glfwGetTime() * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
        sun.setMat4("model", sunModel);
        bindTexture(sunTexture);
        renderSphere();

        // planet properties
        planet.use();
        planet.setVec3("light.position", sunPosition);
        planet.setMat4("projection", projection);
        planet.setMat4("view", view);
        planet.setVec3("light.ambient", ambientColor);
        planet.setVec3("light.diffuse", diffuseColor);
        planet.setVec3("light.specular", lightColor);

        // orbit properties
        orbit.use();
        orbit.setMat4("projection", projection);
        orbit.setMat4("view", view);
        orbit.setVec3("color", sunLightColor); // white color

        for (unsigned int i = 0; i < planetCount; i++) {
            // render planets
            planetModel[i] = planetCreator(
                    planetProp[i].translation, // translation around the sun (translation velocity)
                    planetProp[i].distance, // distance from the sun
                    planetProp[i].rotation, // rotation around its own axis (rotation velocity)
                    planetProp[i].scale, // scale of the planet
                    sunModel[3] // center of the model (contains the exact position of the sun)
            );
            planet.use();
            planet.setMat4("model", planetModel[i]);
            bindTexture(planetTextures[i]);
            renderSphere();

            // render planet's orbit
            orbit.use();
            orbitModel = glm::translate(glm::mat4(1.0f), glm::vec3(sunModel[3]));
            orbit.setMat4("model", orbitModel);
            renderOrbit(planetProp[i].distance, &orbitVAO[i]);

            if (planetProp[i].name == "earth") {
                // render moon
                glm::mat4 moonModel = planetCreator(
                        moonProp.translation, // translation around the earth (translation velocity)
                        moonProp.distance, // distance from the earth
                        moonProp.rotation, // rotation around its own axis (rotation velocity)
                        moonProp.scale, // scale of the planet
                        planetModel[i][3] // center of the model (contains the exact position of the earth)
                );
                planet.use();
                planet.setMat4("model", moonModel);
                bindTexture(moonTexture);
                renderSphere();

                // render moon's orbit
                orbit.use();
                orbitModel = glm::translate(glm::mat4(1.0f), glm::vec3(planetModel[i][3]));
                orbit.setMat4("model", orbitModel);
                renderOrbit(moonProp.distance, &moonOrbitVAO);
            }
        }

        // render project's name text
        renderText(
                text,
                startText,
                WIDTH - charSpaceScaled(startTextLength, startTextScale),
                charHeightScaled(startTextScale),
                startTextScale,
                textColor
        );

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVAO);

    glDeleteVertexArrays((GLsizei) planetCount, orbitVAO);
    glDeleteBuffers((GLsizei) planetCount, orbitVAO);

    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);

    glDeleteVertexArrays(1, &moonOrbitVAO);
    glDeleteBuffers(1, &moonOrbitVAO);

    glDeleteTextures(1, &sunTexture);
    glDeleteTextures(1, &moonTexture);
    glDeleteTextures(1, &saturnRingTexture);
    glDeleteTextures((GLsizei) planetCount, planetTextures);

    glfwTerminate(); // clear all previously allocated GLFW resources
    return 0;
}

/** Function to process input
 *
 * @param window: window to process input
 *
 */
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, (float) deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, (float) deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, (float) deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, (float) deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UPWARD, (float) deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWNWARD, (float) deltaTime);
}

/** Function to resize window size if changed (by OS or user resize)
 *
 * @param window: window to resize
 * @param width: new width
 * @param height: new height
 *
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

/** Function to process mouse movement
 *
 * @param window: window to process mouse movement
 * @param x_pos: x position of mouse
 * @param y_pos: y position of mouse
 *
 */
void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
    if (firstMouse) {
        lastX = x_pos;
        lastY = y_pos;
        firstMouse = false;
    }

    double x_offset = x_pos - lastX;
    double y_offset = lastY - y_pos; // reversed since y-coordinates go from bottom to top

    lastX = x_pos;
    lastY = y_pos;

    camera.ProcessMouseMovement((float) x_offset, (float) y_offset);
}

/**
 * Function to process mouse scroll-wheel
 *
 * @param window: window to process mouse scroll
 * @param x_offset: x offset of mouse scroll
 * @param y_offset: y offset of mouse scroll
 *
 */
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    camera.ProcessMouseScroll(static_cast<float>(y_offset));
}

/** Function to render sphere */
void renderSphere() {
    if (sphereVAO == 0) { // first time initializing the sphere
        glGenVertexArrays(1, &sphereVAO);

        // vertex buffer object, element buffer object
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions; // vertices
        std::vector<glm::vec2> uv; // texture coordinates
        std::vector<glm::vec3> normals; // normals
        std::vector<unsigned int> indices;

        const float radius = 1.0f; // radius from center (0,0)

        // create sphere
        for (unsigned int x = 0; x <= STEP; ++x) {
            for (unsigned int y = 0; y <= STEP; ++y) {
                // calculate the UV coordinates (two-dimensional texture coordinates)
                float xSegment = (float) x / (float) STEP; // u coordinate (horizontal)
                float ySegment = (float) y / (float) STEP; // v coordinate (vertical)

                // calculate the position of each vertex (same for normals)
                // see more at: https://mathinsight.org/spherical_coordinates
                float xPos = radius * std::sin(ySegment * PI) * std::cos(xSegment * 2.0f * PI);
                float yPos = radius * std::sin(ySegment * PI) * std::sin(xSegment * 2.0f * PI);
                float zPos = radius * std::cos(ySegment * PI);

                // add the elements to the end of each vector
                positions.emplace_back(xPos, yPos, zPos);
                uv.emplace_back(xSegment, ySegment);
                normals.emplace_back(xPos, yPos, zPos);
            }
        }

        // generate indices
        // see more at: https://opentk.net/learn/chapter1/3-element-buffer-objects.html
        bool oddRow = false;
        for (unsigned int y = 0; y < STEP; ++y) {
            if (!oddRow) {
                // even rows move left to right
                for (unsigned int x = 0; x <= STEP; ++x) {
                    indices.push_back(y * (STEP + 1) + x);
                    indices.push_back((y + 1) * (STEP + 1) + x);
                }
            } else {
                // odd rows move right to left
                for (int x = STEP; x >= 0; --x) {
                    indices.push_back((y + 1) * (STEP + 1) + x);
                    indices.push_back(y * (STEP + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        // calculate the number of indices (size of indices vector)
        indexCount = static_cast<GLsizei>(indices.size());

        // store all the data in one vector (positions, normals and uv)
        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i) {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (!normals.empty()) {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (!uv.empty()) {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }

        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        GLsizei stride = (3 + 2 + 3) * sizeof(float); // 3 for position, 2 for uv, 3 for normal

        // vertex attribute
        glVertexAttribPointer(
                0, // attribute
                3, // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                stride, // stride
                (void *) nullptr // array buffer offset
        );
        glEnableVertexAttribArray(0);

        // normal attribute
        glVertexAttribPointer(
                1, // attribute
                3, // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                stride, // stride
                (void *) (3 * sizeof(float)) // array buffer offset
        );
        glEnableVertexAttribArray(1);

        // texture attribute
        glVertexAttribPointer(
                2, // attribute
                2, // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                stride, // stride
                (void *) (6 * sizeof(float)) // array buffer offset
        );
        glEnableVertexAttribArray(2);

#ifdef _DEBUG
        std::cout << "New sphere created" << std::endl;
#endif

    }
    glBindVertexArray(sphereVAO);

    // GL_TRIANGLE_STRIP is to ensure that the triangles are all drawn with the same orientation
    // see more at: https://www.khronos.org/opengl/wiki/Primitive#Triangle_primitives
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
}

/** Function to build orbit
 *
 * @param radius: radius of the circle
 * @param VAO: vertex array object
 *
 */
void renderOrbit(float radius, unsigned int *VAO) {
    if (*VAO == 0) { // first time initializing the orbit
        glGenVertexArrays(1, VAO);

        // vertex buffer object, element buffer object
        unsigned int vbo;
        glGenBuffers(1, &vbo);

        std::vector<glm::vec3> vertices;

        float angle = 360.0f / STEP; // angle between each vertex

        // create circle
        for (unsigned int i = 0; i < STEP; i++) {
            float currentAngle = angle * (float) i;

            // calculate the position of each vertex
            // see more at: https://faun.pub/draw-circle-in-opengl-c-2da8d9c2c103
            float x = radius * std::cos(glm::radians(currentAngle));
            float y = 0.0f;
            float z = radius * std::sin(glm::radians(currentAngle));

            // infinite points in the circle correction
            if (currentAngle == 90.0f || currentAngle == 270.0f) x = 0.0f;
            else if (currentAngle == 0.0f || currentAngle == 180.0f) z = 0.0f;

            // add the elements to the end of each vector
            vertices.emplace_back(x, y, z);
        }

        glBindVertexArray(*VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        // vertex attribute
        glVertexAttribPointer(
                0, // attribute
                3, // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                3 * sizeof(float), // stride
                (void *) nullptr // array buffer offset
        );
        glEnableVertexAttribArray(0);

#ifdef _DEBUG
        std::cout << "New orbit created" << std::endl;
#endif

    }
    glBindVertexArray(*VAO);
    glDrawArrays(GL_LINE_LOOP, 0, STEP); // orbit mode
}

/** Function to render text
 *
 * @param shader: shader to render text
 * @param text: text to render
 * @param x: x position of text
 * @param y: y position of text
 * @param scale: scale of text
 * @param color: color of text
 * @param VAO: vertex array object
 *
 */
void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color) {
    shader.use();
    shader.setVec3("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float x_pos = x + (float) ch.bearing.x * scale;
        float y_pos = y - (float) (ch.size.y - ch.bearing.y) * scale;

        float w = (float) ch.size.x * scale;
        float h = (float) ch.size.y * scale;

        float vertices[6][4] = { // 2 for position, 2 for texture
                {x_pos,     y_pos + h, 0.0f, 0.0f}, // bottom left
                {x_pos,     y_pos,     0.0f, 1.0f}, // top left
                {x_pos + w, y_pos,     1.0f, 1.0f}, // top right

                {x_pos,     y_pos + h, 0.0f, 0.0f}, // bottom left
                {x_pos + w, y_pos,     1.0f, 1.0f}, // top right
                {x_pos + w, y_pos + h, 1.0f, 0.0f} // bottom right
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        // NOTE: use glBufferSubData and not glBufferData
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // advance cursors for the next glyph (NOTE: advance is number of 1/64 pixels)
        // 2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels)
        x += (float) (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/** Function to load 2D texture from file
 *
 * @param path: path to texture
 * @return textureID
 *
 */
unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLint format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

/** Function to bind texture
 *
 * @param texture: texture to bind
 *
 */
void bindTexture(unsigned int texture) {
    glActiveTexture(GL_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, texture);
}

/** Function to create planet
 *
 * @param translation: translation around the sun/planet
 * @param distance: distance from the sun/planet
 * @param rotation: rotation around its own axis
 * @param scale: scale of the planet
 * @param centerModel: center of the model
 * @return model matrix
 *
 */
glm::mat4 planetCreator(float translation, float distance, float rotation, float scale, glm::vec3 centerModel) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), centerModel); // move origin of rotation to the center of model
    model = glm::rotate(model, (float) glfwGetTime() * translation, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, distance));
    model = glm::rotate(model, (float) glfwGetTime() * rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale));
    return model; // center * translation * distance * rotation * scale
}

/** Function to scale char space
 *
 * @param charSpace: char space to scale
 * @param scale: scale of char space
 * @return scaled char space
 *
 */
float charSpaceScaled(float textLength, float scale) {
    return textLength * CHAR_SPACE * scale;
}

/** Function to scale char height
 *
 * @param scale: scale of char height
 * @return scaled char height
 *
 */
float charHeightScaled(float scale) {
    return CHAR_HEIGHT * scale;
}
