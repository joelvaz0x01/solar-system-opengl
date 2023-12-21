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
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION /// to avoid linker errors

#include <stb_image.h>
#include <shader_m.h>
#include <camera.h>

#include "main.h"


#define WIDTH 1920 /// width of the screen
#define HEIGHT 1080 /// height of the screen

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); /// camera position
double lastX = WIDTH / 2.0f; /// last x position of the mouse
double lastY = HEIGHT / 2.0f; /// last y position of the mouse

double deltaTime = 0.0f; /// time between current frame and last frame
double lastFrame = 0.0f; /// time of last frame

unsigned int sphereVAO = 0; /// vertex array object for sphere
GLsizei indexCount; /// number of indices for sphere

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Cube Texture", /*glfwGetPrimaryMonitor()*/ nullptr, nullptr);
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

    // compile shaders
    Shader shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");

    shader.use();
    shader.setInt("albedoMap", 0);
    shader.setInt("normalMap", 1);
    shader.setInt("metallicMap", 2);
    shader.setInt("roughnessMap", 3);
    shader.setInt("aoMap", 4);

    // load planet textures
    unsigned int sunTexture = loadTexture("resources/textures/sun.jpg");

    // light properties
    glm::vec3 lightPositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),};
    glm::vec3 lightColors[] = {glm::vec3(10.0f, 10.0f, 10.0f),};

    // projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    shader.use();
    shader.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        shader.setVec3("camPos", camera.Position);

        // bind textures
        bindTexture(sunTexture, 0);

        glm::mat4 model = glm::mat4(1.0f);

        // render the light source (sun)
        unsigned int lightPositionSize = sizeof(lightPositions) / sizeof(lightPositions[0]);
        for (unsigned int i = 0; i < lightPositionSize; ++i) {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            shader.setMat4("model", model);
            shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            renderSphere();
        }

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/** Function to process input
 *
 * @param window: window to process input
 * @return nullptr
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
 * @return nullptr
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

/** Function to process mouse movement
 *
 * @param window: window to process mouse movement
 * @param x_pos: x position of mouse
 * @param y_pos: y position of mouse
 * @return nullptr
 */
void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
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
 * @return nullptr
 */
void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    camera.ProcessMouseScroll(static_cast<float>(y_offset));
}

/** Function to render sphere
 *
 * @return nullptr
 */
void renderSphere() {
    if (sphereVAO == 0) { // first time initializing sphere
        glGenVertexArrays(1, &sphereVAO);

        // vertex buffer object, element buffer object
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int STEP = 64; // increase to improve shape quality
        const float PI = 3.14159265359f;
        const float distance = 1.0f; // distance from center (0,0)

        // create sphere
        for (unsigned int x = 0; x <= STEP; ++x) {
            for (unsigned int y = 0; y <= STEP; ++y) {
                // calculate the UV coordinates (two-dimensional texture coordinates)
                float xSegment = (float) x / (float) STEP; // u coordinate (horizontal)
                float ySegment = (float) y / (float) STEP; // v coordinate (vertical)

                // calculate the position of each vertex (same for normals)
                // see more at: https://mathinsight.org/spherical_coordinates
                float xPos = distance * std::sin(ySegment * PI) * std::cos(xSegment * 2.0f * PI);
                float yPos = distance * std::sin(ySegment * PI) * std::sin(xSegment * 2.0f * PI);
                float zPos = distance * std::cos(ySegment * PI);

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
    }

    glBindVertexArray(sphereVAO);

    // GL_TRIANGLE_STRIP is to ensure that the triangles are all drawn with the same orientation
    // see more at: https://www.khronos.org/opengl/wiki/Primitive#Triangle_primitives
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
}

/** Function to load 2D texture from file
 *
 * @param path: path to texture
 * @return textureID
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

void bindTexture(unsigned int texture, unsigned int textureUnit) {
    glActiveTexture(GL_TEXTURE + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}
