#include "input.h"

#include <camera.h>

// Extern declarations for global variables used in callbacks
extern Camera camera;
extern Camera freeCamera;
extern unsigned int cameraMode;
extern unsigned int skyboxMode;
extern double deltaTime;
extern double lastX;
extern double lastY;
extern bool firstMouse;

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, (float)deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, (float)deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, (float)deltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UPWARD, (float)deltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWNWARD, (float)deltaTime);

  /* change camera mode */
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {  /* reset camera position to free camera mode */
    camera = freeCamera;
    cameraMode = 8;
  }
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
    cameraMode = 0;  /* mercury camera mode */
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
    cameraMode = 1;  /* venus camera mode */
  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
    cameraMode = 2;  /* earth camera mode */
  if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
    cameraMode = 3;  /* mars camera mode */
  if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
    cameraMode = 4;  /* jupiter camera mode */
  if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
    cameraMode = 5;  /* saturn camera mode */
  if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
    cameraMode = 6;  /* uranus camera mode */
  if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
    cameraMode = 7;  /* neptune camera mode */
  if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
    cameraMode = 9;  /* top view camera mode */

  if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) skyboxMode = 0;  /* green nebula skybox          */
  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) skyboxMode = 1;  /* purple nebula complex skybox */
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
  (void)window;
  if (firstMouse) {
    lastX = x_pos;
    lastY = y_pos;
    firstMouse = false;
  }

  double x_offset = x_pos - lastX;
  double y_offset = lastY - y_pos;  /* reversed since y-coordinates go from bottom to top */

  lastX = x_pos;
  lastY = y_pos;

  camera.ProcessMouseMovement((float)x_offset, (float)y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
  (void)window;
  (void)x_offset;
  camera.ProcessMouseScroll(static_cast<float>(y_offset));
}
