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
 * Camera modes:
 * - SPACE key: free camera mode (default)
 * - 0 key: top view camera mode
 * - 1 to 8 keys: focus on a planet (NUMPAD also works)
 *
 * Skybox modes:
 * - F1 key: purple nebula complex skybox (default)
 * - F2 key: green nebula skybox
 *
 * @author joelvaz0x01
 * @author BrunoFG1
 *
 */

#include <iostream>
#include <map>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
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
#define STEP 256 ///< increase to improve shape quality

#define WIDTH 1920 ///< width of the screen
#define HEIGHT 1080 ///< height of the screen

// values are adjusted if scale = 1.0f
#define CHAR_WIDTH_UP 27.0f ///< additional font space when x = WIDTH
#define CHAR_WIDTH_DOWN 25.0f ///< additional font start space when x = 0
#define CHAR_HEIGHT_UP 60.0f ///< additional font space when y = HEIGHT
#define CHAR_HEIGHT_DOWN 25.0f ///< additional font space when y = 0

/// planet information
/// see more at: https://science.nasa.gov/solar-system/planets/
/// and at: https://nssdc.gsfc.nasa.gov/planetary/factsheet/
planetInfo planetsData[] = {
        {"Mercury", "0.4 astronomical units",  "2,440 km",  "0 moons",   "59 Earth days",    "88 Earth days"},
        {"Venus",   "0.72 astronomical units", "6,051 km",  "0 moons",   "243 Earth days",   "225 Earth days"},
        {"Earth",   "1.0 astronomical unit",   "6,378 km",  "1 moon",    "1 Earth day",      "365 Earth days"},
        {"Mars",    "1.5 astronomical units",  "3,390 km",  "2 moons",   "23.9 Earth hours", "687 Earth days"},
        {"Jupiter", "5.2 astronomical units",  "69,911 km", "95 moons",  "10 Earth hours",   "4,333 Earth days"},
        {"Saturn",  "9.5 astronomical units",  "58,232 km", "146 moons", "10.7 Earth hours", "10,756 Earth days"},
        {"Uranus",  "19.8 astronomical units", "25,362 km", "27 moons",  "17 Earth hours",   "30,687 Earth days"},
        {"Neptune", "30 astronomical units",   "24,622 km", "14 moons",  "16 Earth hours",   "60,190 Earth days"}
};

/// planet properties
planetProperties planetProp[] = {
        {2.0f, 2.0f, 0.3f, 0.04f}, // mercury
        {1.5f, 3.0f, 0.4f, 0.1f}, // venus
        {1.0f, 4.0f, 0.5f, 0.1f}, // earth
        {0.8f, 5.0f, 0.6f, 0.09f}, // mars
        {0.6f, 6.0f, 0.7f, 0.3f}, // jupiter
        {0.3f, 7.0f, 0.8f, 0.4f}, // saturn
        {0.2f, 8.0f, 1.0f, 0.35f}, // uranus
        {0.1f, 9.0f, 0.9f, 0.35f}  // neptune
};

/// moon properties
planetProperties moonProp = {6.0f, 0.3f, 3.0f, 0.03f};

glm::mat4 view = glm::mat4(1.0f); ///< view matrix
glm::mat4 projection = glm::mat4(1.0f); ///< projection matrix

/// current camera position
Camera camera(
        glm::vec3(0.0f, 8.0f, 15.0f), // position
        glm::vec3(0.0f, 1.0f, 0.0f), // up - default
        -90.0f, // yaw - default
        -35.0f // pitch (look down)
);
Camera upViewCamera(
        glm::vec3(0.0f, 25.0f, 0.0f), // position
        glm::vec3(0.0f, 1.0f, 0.0f), // up - default
        -90.0f, // yaw - default
        -89.0f // pitch (look down)
);
Camera freeCamera = camera; ///< free camera mode position

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
unsigned int textVAO; ///< vertex array object for text
unsigned int textVBO; ///< vertex buffer object for text

unsigned int cameraMode = 8; ///< focus planet mode

unsigned int skyboxVAO = 0; ///< vertex array object for skybox

unsigned int skyboxMode = 0; ///< skybox mode

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

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Solar System", glfwGetPrimaryMonitor() /*nullptr*/, nullptr);
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
    Shader planet(getResourcePath("shaders/planetVertex.glsl").c_str(), getResourcePath("shaders/planetFragment.glsl").c_str());
    Shader sun(getResourcePath("shaders/sunVertex.glsl").c_str(), getResourcePath("shaders/sunFragment.glsl").c_str());
    Shader orbit(getResourcePath("shaders/orbitVertex.glsl").c_str(), getResourcePath("shaders/orbitFragment.glsl").c_str());
    Shader text(getResourcePath("shaders/textVertex.glsl").c_str(), getResourcePath("shaders/textFragment.glsl").c_str());
    Shader skybox(getResourcePath("shaders/skyboxVertex.glsl").c_str(), getResourcePath("shaders/skyboxFragment.glsl").c_str());

    //load freetype
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    //load font
    FT_Face face;
    std::string fontPath = getResourcePath("fonts/MPLUSRounded1c-Bold.ttf");
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
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
                std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
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
    unsigned int sunTexture = loadTexture(getResourcePath("textures/planets/sun.jpg").c_str());

    // load planet textures
    unsigned int planetTextures[] = {
            loadTexture(getResourcePath("textures/planets/mercury.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/venus.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/earth.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/mars.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/jupiter.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/saturn.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/uranus.jpg").c_str()),
            loadTexture(getResourcePath("textures/planets/neptune.jpg").c_str())
    };

    // load earth's moon texture
    unsigned int moonTexture = loadTexture(getResourcePath("textures/planets/moon.jpg").c_str());

    // load skybox textures
    // NOTE: skybox textures must be in square format (same width and height)
    // NOTE: must be in this order: right(+x), left(-x), top(+y), bottom(-y), front(+z), back(-z)
    // see more at: https://learnopengl.com/Advanced-OpenGL/Cubemaps

    // purple nebula complex skybox
    std::string pNebulaComplexPaths[6] = {
            getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_right.png"), // right side (+x)
            getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_left.png"), // left side (-x)
            getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_top.png"), // top side (+y)
            getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_bottom.png"), // bottom side (-y)
            getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_front.png"), // front side (+z)
            getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_back.png"), // back side (-z)
    };
    
    const char *pNebulaComplex[6];
    for (int i = 0; i < 6; i++) {
        pNebulaComplex[i] = pNebulaComplexPaths[i].c_str();
    }
    unsigned int pNebulaComplexSkybox = loadCubeMap(pNebulaComplex);

    // green nebula skybox
    std::string gNebulaPaths[6] = {
            getResourcePath("textures/skybox/green_nebula/green_nebula_right.png"), // right side (+x)
            getResourcePath("textures/skybox/green_nebula/green_nebula_left.png"), // left side (-x)
            getResourcePath("textures/skybox/green_nebula/green_nebula_top.png"), // top side (+y)
            getResourcePath("textures/skybox/green_nebula/green_nebula_bottom.png"), // bottom side (-y)
            getResourcePath("textures/skybox/green_nebula/green_nebula_front.png"), // front side (+z)
            getResourcePath("textures/skybox/green_nebula/green_nebula_back.png"), // back side (-z)
    };
    
    const char *gNebula[6];
    for (int i = 0; i < 6; i++) {
        gNebula[i] = gNebulaPaths[i].c_str();
    }
    unsigned int gNebulaSkybox = loadCubeMap(gNebula);

    // number of planets
    unsigned int planetCount = sizeof(planetTextures) / sizeof(planetTextures[0]);

    // model matrix for each planet
    auto *planetModel = new glm::mat4[planetCount];

    // sun shader configuration
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
    std::basic_string<char>::size_type startTextLength = startText.length();
    float startTextScale = 0.8f;

    std::string freeModeText = "Free Camera Mode";
    std::basic_string<char>::size_type freeModeTextLength = freeModeText.length();
    float freeModeTextScale = 1.0f;

    std::string upViewText = "Top View Camera Mode";
    std::basic_string<char>::size_type upViewTextLength = upViewText.length();
    float upViewTextScale = 1.0f;

    float planetInfoTextScale = 0.8f;

    glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f); // white color

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

        // sun properties (phong shading)
        lightColor = sunLightColor;
        diffuseColor = lightColor * glm::vec3(0.8f);
        ambientColor = diffuseColor * glm::vec3(0.1f);

        // sun properties
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

            if (planetsData[i].name == "Earth") {
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
                charWidthScaled(startTextScale, startTextLength, true),
                charHeightScaled(startTextScale, false),
                startTextScale,
                textColor
        );

        if (cameraMode == 9) { // render top view camera mode
            camera = upViewCamera;
            renderText(
                    text,
                    upViewText,
                    charWidthScaled(upViewTextScale, upViewTextLength, false),
                    charHeightScaled(upViewTextScale, true),
                    upViewTextScale,
                    textColor
            );
        } else if (cameraMode != 8) { // render planet's information camera mode
            camera = Camera(
                    glm::vec3(planetModel[cameraMode][3]) + glm::vec3(0.0f, 1.2f, 1.0f), // position
                    glm::vec3(0.0f, 1.0f, 0.0f), // up - default
                    -90.0f, // yaw - default
                    -50.0f // pitch (look down)
            );
            showPlanetInfo(text, cameraMode, textColor, planetInfoTextScale);
        } else { // render free camera mode
            freeCamera = camera; // save current camera position
            renderText(
                    text,
                    freeModeText,
                    charWidthScaled(freeModeTextScale, freeModeTextLength, false),
                    charHeightScaled(freeModeTextScale, true),
                    freeModeTextScale,
                    textColor
            );
        }

        // render skybox
        skybox.use();
        skybox.setMat4("projection", projection);
        skybox.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        if (skyboxMode == 0) renderSkybox(pNebulaComplexSkybox);
        else renderSkybox(gNebulaSkybox);

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &sphereVAO);
    for (unsigned int &i: orbitVAO) {
        glDeleteVertexArrays(1, &i);
    }
    glDeleteVertexArrays(1, &moonOrbitVAO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
    glDeleteVertexArrays(1, &skyboxVAO);

    glDeleteTextures(1, &sunTexture);
    for (unsigned int &planetTexture: planetTextures) {
        glDeleteTextures(1, &planetTexture);
    }
    glDeleteTextures(1, &moonTexture);
    glDeleteTextures(1, &gNebulaSkybox);
    glDeleteTextures(1, &pNebulaComplexSkybox);

    delete[] planetModel;

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

    // change camera mode
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { // reset camera position to free camera mode
        camera = freeCamera;
        cameraMode = 8;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
        cameraMode = 0; // mercury camera mode
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
        cameraMode = 1; // venus camera mode
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
        cameraMode = 2; // earth camera mode
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
        cameraMode = 3; // mars camera mode
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
        cameraMode = 4; // jupiter camera mode
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
        cameraMode = 5; // saturn camera mode
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
        cameraMode = 6; // uranus camera mode
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
        cameraMode = 7; // neptune camera mode
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
        cameraMode = 9; // top view camera mode

    // change skybox mode
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) skyboxMode = 0; // green nebula skybox
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) skyboxMode = 1; // purple nebula complex skybox
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
 *
 */
void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color) {
    shader.use();
    shader.setVec3("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE);
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

/** Function to render skybox
 *
 * @param skyboxCubeMap: skybox cube map
 *
 */
void renderSkybox(unsigned int skyboxCubeMap) {
    if (skyboxVAO == 0) { // first time initializing the skybox
        float skyboxVertices[] = { // cube to render skybox
                // bottom side
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,
                -0.5f, 0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,

                // upper side
                -0.5f, -0.5f, 0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, 0.5f,
                -0.5f, -0.5f, 0.5f,

                // back side
                -0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, 0.5f,
                -0.5f, 0.5f, 0.5f,

                // front side
                0.5f, 0.5f, 0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,

                // left side
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, -0.5f, 0.5f,
                -0.5f, -0.5f, 0.5f,
                -0.5f, -0.5f, -0.5f,

                // right side
                -0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, -0.5f
        };

        // vertex buffer object
        unsigned int vbo;
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &vbo);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(
                0, // attribute
                3, // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                3 * sizeof(float), // stride
                (void *) nullptr // array buffer offset
        );
        glEnableVertexAttribArray(0);
    }
    glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // reset depth function to default
}

/**
 * @brief Get the full path to a resource file, supporting AppImage deployment
 * @details This function checks for the SOLAR_SYSTEM_RESOURCE_PATH environment variable
 * which is set by the AppImage wrapper script. If not found, it looks for resources 
 * relative to the executable's location.
 *
 * @param relativePath: relative path to the resource (e.g., "textures/sun.jpg")
 * @return full path to the resource
 */
std::string getResourcePath(const std::string& relativePath) {
#ifdef _WIN32
    // Windows: use _dupenv_s for safety
    char* resourcePath = nullptr;
    size_t len = 0;
    if (_dupenv_s(&resourcePath, &len, "SOLAR_SYSTEM_RESOURCE_PATH") == 0 && resourcePath != nullptr) {
        std::string fullPath = std::string(resourcePath) + "/" + relativePath;
        free(resourcePath);
        return fullPath;
    }
    
    // Get executable path on Windows
    char exePath[MAX_PATH];
    DWORD result = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (result > 0) {
        std::string executablePath(exePath);
        size_t lastSlash = executablePath.find_last_of("\\");
        if (lastSlash != std::string::npos) {
            std::string executableDir = executablePath.substr(0, lastSlash);
            // Check if this is a shader path (special case for Windows build)
            if (relativePath.find("shaders/") == 0) {
                return executableDir + "\\" + relativePath;
            } else {
                return executableDir + "\\resources\\" + relativePath;
            }
        }
    }
#else
    // Unix/Linux: use getenv
    const char* resourcePath = std::getenv("SOLAR_SYSTEM_RESOURCE_PATH");
    if (resourcePath != nullptr) {
        std::string fullPath = std::string(resourcePath) + "/" + relativePath;
        return fullPath;
    }
    
    // Get executable path on Unix/Linux
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        std::string executablePath(exePath);
        size_t lastSlash = executablePath.find_last_of("/");
        if (lastSlash != std::string::npos) {
            std::string executableDir = executablePath.substr(0, lastSlash);
            return executableDir + "/resources/" + relativePath;
        }
    }
#endif
    
    // Fallback: use relative path from current directory
    return "resources/" + relativePath;
}

/**
 * @brief Loads the texture and assign it to textureID
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
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB; // JPG image requires GL_RGB
        else if (nrComponents == 4) format = GL_RGBA; // PNG image requires GL_RGBA

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

/** Function to load cubeMap texture from file
 *
 * @param path: path to texture (cubeMap)
 * @return textureID
 *
 */
unsigned int loadCubeMap(char const **path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(path[i], &width, &height, &nrComponents, 0);
        if (data) {
            GLint format = GL_RGB;
            if (nrComponents == 1) format = GL_RED;
            else if (nrComponents == 3) format = GL_RGB; // JPG image requires GL_RGB
            else if (nrComponents == 4) format = GL_RGBA; // PNG image requires GL_RGBA

            // see more at: https://learnopengl.com/Advanced-OpenGL/Cubemaps
            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // target
                    0, // level
                    format, // internal format
                    width, // width
                    height, // height
                    0, // border
                    format, // format
                    GL_UNSIGNED_BYTE, // type
                    data // pixels
            );
            stbi_image_free(data);

#ifdef _DEBUG
            std::cout << "CubeMap texture loaded successfully at path: " << path[i] << std::endl;
#endif

        } else {
            std::cerr << "CubeMap texture failed to load at path: " << path[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

/** Function to bind texture
 *
 * @param texture: texture to bind
 *
 */
void bindTexture(unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);
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

/** Function to scale char height
 *
 * @param scale: scale of char height
 * @param isMaxHeight: check if the character is at the top of the screen
 * @return scaled character height
 *
 */
float charHeightScaled(float scale, bool isMaxHeight) {
    float result; // correction to apply when the character is not at the top of the screen

    if (isMaxHeight) result = HEIGHT - CHAR_HEIGHT_UP * scale;
    else result = CHAR_HEIGHT_DOWN * scale;

    return result;
}


/** Function to scale char width
 *
 * @param scale: scale of char width
 * @param textLength: length of the text
 * @param isMaxWidth: check if the character is at the right of the screen
 * @return scaled character width
 *
 */
float charWidthScaled(float scale, std::basic_string<char>::size_type textLength, bool isMaxWidth) {
    float result; // correction to apply when the character is not at the top of the screen

    if (isMaxWidth) result = WIDTH - static_cast<float>(textLength) * CHAR_WIDTH_UP * scale;
    else result = CHAR_WIDTH_DOWN * scale;

    return result;
}

/** Function to show planet information
 *
 * @param shader: shader to show planet information
 * @param planetIndex: index of the planet to use in planetInfo
 * @param textColor: color of the text
 * @param textScale: scale of the text
 *
 */
void showPlanetInfo(Shader &shader, unsigned int planetIndex, glm::vec3 textColor, float textScale) {
    std::string planetInfoText[] = {
            "Name: " + planetsData[planetIndex].name,
            "Distance: " + planetsData[planetIndex].distance,
            "Radius: " + planetsData[planetIndex].radius,
            "Moons number: " + planetsData[planetIndex].moons,
            "Rotation duration: " + planetsData[planetIndex].rotationPeriod,
            "Translation duration: " + planetsData[planetIndex].orbitalPeriod,
    };

    int planetInfoTextSize = sizeof(planetInfoText) / sizeof(planetInfoText[0]);

    for (int i = 0; i < planetInfoTextSize; i++) {
        renderText(
                shader,
                planetInfoText[i],
                charWidthScaled(textScale, planetInfoText[i].length(), false),
                charHeightScaled(textScale, true) - ((float) i * 50.0f),
                textScale,
                textColor
        );
    }
}
