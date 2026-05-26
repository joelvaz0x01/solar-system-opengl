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

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#ifdef __linux__
#include <fontconfig/fontconfig.h>
#endif

#include <camera.h>
#include <shader_m.h>

#include "constants.h"
#include "input.h"
#include "planet_data.h"
#include "rendering.h"
#include "text_renderer.h"
#include "utils.h"

// Global variables (used by input.cpp via extern)
Camera camera(glm::vec3(0.0f, 8.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -35.0f);
Camera upViewCamera(glm::vec3(0.0f, 25.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -89.0f);
Camera freeCamera = camera;

double lastX = WIDTH / 2.0f;
double lastY = HEIGHT / 2.0f;
bool firstMouse = true;

double deltaTime = 0.0f;
double lastFrame = 0.0f;

unsigned int cameraMode = 8;
unsigned int skyboxMode = 0;

int main() {
#ifdef __linux__
  if (!FcInit()) {
    std::cerr << "ERROR::FONTCONFIG: Could not init Fontconfig Library" << std::endl;
    return -1;
  }
#endif

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window =
      glfwCreateWindow(WIDTH, HEIGHT, "Solar System", glfwGetPrimaryMonitor(), nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Shader planetShader(getResourcePath("shaders/planetVertex.glsl").c_str(),
                      getResourcePath("shaders/planetFragment.glsl").c_str());
  Shader sunShader(getResourcePath("shaders/sunVertex.glsl").c_str(),
                   getResourcePath("shaders/sunFragment.glsl").c_str());
  Shader orbitShader(getResourcePath("shaders/orbitVertex.glsl").c_str(),
                     getResourcePath("shaders/orbitFragment.glsl").c_str());
  Shader textShader(getResourcePath("shaders/textVertex.glsl").c_str(),
                    getResourcePath("shaders/textFragment.glsl").c_str());
  Shader skyboxShader(getResourcePath("shaders/skyboxVertex.glsl").c_str(),
                      getResourcePath("shaders/skyboxFragment.glsl").c_str());

  if (initTextRenderer(getResourcePath("fonts/MPLUSRounded1c-Bold.ttf")) != 0) {
    return -1;
  }

  unsigned int sunTexture = loadTexture(getResourcePath("textures/planets/sun.jpg").c_str());
  unsigned int planetTextures[] = {
      loadTexture(getResourcePath("textures/planets/mercury.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/venus.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/earth.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/mars.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/jupiter.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/saturn.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/uranus.jpg").c_str()),
      loadTexture(getResourcePath("textures/planets/neptune.jpg").c_str())};
  unsigned int moonTexture = loadTexture(getResourcePath("textures/planets/moon.jpg").c_str());

  std::string pNebulaComplexPaths[] = {
      getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_right.png"),
      getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_left.png"),
      getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_top.png"),
      getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_bottom.png"),
      getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_front.png"),
      getResourcePath("textures/skybox/purple_nebula_complex/purple_nebula_complex_back.png"),
  };
  const char* pNebulaComplex[6];
  for (int i = 0; i < 6; ++i) pNebulaComplex[i] = pNebulaComplexPaths[i].c_str();
  unsigned int pNebulaComplexSkybox = loadCubeMap(pNebulaComplex);

  std::string gNebulaPaths[] = {
      getResourcePath("textures/skybox/green_nebula/green_nebula_right.png"),
      getResourcePath("textures/skybox/green_nebula/green_nebula_left.png"),
      getResourcePath("textures/skybox/green_nebula/green_nebula_top.png"),
      getResourcePath("textures/skybox/green_nebula/green_nebula_bottom.png"),
      getResourcePath("textures/skybox/green_nebula/green_nebula_front.png"),
      getResourcePath("textures/skybox/green_nebula/green_nebula_back.png"),
  };
  const char* gNebula[6];
  for (int i = 0; i < 6; ++i) gNebula[i] = gNebulaPaths[i].c_str();
  unsigned int gNebulaSkybox = loadCubeMap(gNebula);

  unsigned int planetCount = 8;
  unsigned int orbitVAO[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int moonOrbitVAO = 0;
  auto* planetModel = new glm::mat4[planetCount];

  sunShader.use();
  sunShader.setInt("texture1", 0);

  planetShader.use();
  planetShader.setInt("material.diffuse", 0);
  planetShader.setInt("material.specular", 1);

  glm::vec3 sunPosition = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 sunLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));

  textShader.use();
  textShader.setMat4("projection", projection);

  while (!glfwWindowShouldClose(window)) {
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    process_input(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection =
        glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    glm::vec3 lightColor = sunLightColor;
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);

    sunShader.use();
    sunShader.setVec3("color", lightColor);
    sunShader.setMat4("projection", projection);
    sunShader.setMat4("view", view);
    glm::mat4 sunModel = glm::translate(glm::mat4(1.0f), sunPosition);
    sunModel = glm::rotate(sunModel, (float)glfwGetTime() * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    sunShader.setMat4("model", sunModel);
    bindTexture(sunTexture);
    renderSphere();

    planetShader.use();
    planetShader.setVec3("light.position", sunPosition);
    planetShader.setMat4("projection", projection);
    planetShader.setMat4("view", view);
    planetShader.setVec3("light.ambient", ambientColor);
    planetShader.setVec3("light.diffuse", diffuseColor);
    planetShader.setVec3("light.specular", lightColor);

    orbitShader.use();
    orbitShader.setMat4("projection", projection);
    orbitShader.setMat4("view", view);
    orbitShader.setVec3("color", sunLightColor);

    for (unsigned int i = 0; i < planetCount; i++) {
      planetModel[i] = planetCreator(planetProp[i].translation, planetProp[i].distance,
                                     planetProp[i].rotation, planetProp[i].scale, sunModel[3]);
      planetShader.use();
      planetShader.setMat4("model", planetModel[i]);
      bindTexture(planetTextures[i]);
      renderSphere();

      orbitShader.use();
      glm::mat4 orbitModel = glm::translate(glm::mat4(1.0f), glm::vec3(sunModel[3]));
      orbitShader.setMat4("model", orbitModel);
      renderOrbit(planetProp[i].distance, &orbitVAO[i]);

      if (planetsData[i].name == "Earth") {
        glm::mat4 moonModel = planetCreator(moonProp.translation, moonProp.distance,
                                            moonProp.rotation, moonProp.scale, planetModel[i][3]);
        planetShader.use();
        planetShader.setMat4("model", moonModel);
        bindTexture(moonTexture);
        renderSphere();

        orbitShader.use();
        orbitModel = glm::translate(glm::mat4(1.0f), glm::vec3(planetModel[i][3]));
        orbitShader.setMat4("model", orbitModel);
        renderOrbit(moonProp.distance, &moonOrbitVAO);
      }
    }

    renderText(textShader, "Solar System", charWidthScaled(0.8f, 12, true),
               charHeightScaled(0.8f, false), 0.8f, sunLightColor);

    if (cameraMode == 9) {
      camera = upViewCamera;
      renderText(textShader, "Top View Camera Mode", charWidthScaled(1.0f, 21, false),
                 charHeightScaled(1.0f, true), 1.0f, sunLightColor);
    } else if (cameraMode != 8) {
      camera = Camera(glm::vec3(planetModel[cameraMode][3]) + glm::vec3(0.0f, 1.2f, 1.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -50.0f);
      showPlanetInfo(textShader, cameraMode, sunLightColor, 0.8f);
    } else {
      freeCamera = camera;
      renderText(textShader, "Free Camera Mode", charWidthScaled(1.0f, 17, false),
                 charHeightScaled(1.0f, true), 1.0f, sunLightColor);
    }

    skyboxShader.use();
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
    if (skyboxMode == 0)
      renderSkybox(pNebulaComplexSkybox);
    else
      renderSkybox(gNebulaSkybox);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  delete[] planetModel;
  cleanupTextRenderer();

  glDeleteTextures(1, &sunTexture);
  for (unsigned int& texture : planetTextures) glDeleteTextures(1, &texture);
  glDeleteTextures(1, &moonTexture);
  glDeleteTextures(1, &gNebulaSkybox);
  glDeleteTextures(1, &pNebulaComplexSkybox);

  for (unsigned int& vao : orbitVAO) glDeleteVertexArrays(1, &vao);
  glDeleteVertexArrays(1, &moonOrbitVAO);

#ifdef __linux__
  FcFini();
#endif

  glfwTerminate();
  return 0;
}
