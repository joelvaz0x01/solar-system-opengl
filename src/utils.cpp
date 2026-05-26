#include "utils.h"

#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include <glad/gl.h>
#include <stb_image.h>

std::string getResourcePath(const std::string& relativePath) {
#ifdef _WIN32
  /* Windows: use _dupenv_s for safety */
  char* resourcePath = nullptr;
  size_t len = 0;
  if (_dupenv_s(&resourcePath, &len, "SOLAR_SYSTEM_RESOURCE_PATH") == 0 &&
      resourcePath != nullptr) {
    std::string fullPath = std::string(resourcePath) + "/" + relativePath;
    free(resourcePath);
    return fullPath;
  }

  /* get executable path on Windows */
  char exePath[MAX_PATH];
  DWORD result = GetModuleFileNameA(NULL, exePath, MAX_PATH);
  if (result > 0) {
    std::string executablePath(exePath);
    size_t lastSlash = executablePath.find_last_of("\\");
    if (lastSlash != std::string::npos) {
      std::string executableDir = executablePath.substr(0, lastSlash);

      /* check if this is a shader path (special case for Windows build) */
      if (relativePath.find("shaders/") == 0) {
        return executableDir + "\\" + relativePath;
      } else {
        return executableDir + "\\resources\\" + relativePath;
      }
    }
  }
#else
  /* Unix/Linux: use getenv */
  const char* resourcePath = std::getenv("SOLAR_SYSTEM_RESOURCE_PATH");
  if (resourcePath != nullptr) {
    std::string fullPath = std::string(resourcePath) + "/" + relativePath;
    return fullPath;
  }

  /* get executable path on Unix/Linux */
  char exePath[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
  if (len != -1) {
    exePath[len] = '\0';
    std::string executablePath(exePath);
    size_t lastSlash = executablePath.find_last_of("/");
    if (lastSlash != std::string::npos) {
      std::string executableDir = executablePath.substr(0, lastSlash);

      /* check if this is a shader path (special case for Linux build) */
      if (relativePath.find("shaders/") == 0) {
        return executableDir + "/" + relativePath;
      } else {
        return executableDir + "/resources/" + relativePath;
      }
    }
  }
#endif
  /* fallback: use relative path from current directory */
  return "resources/" + relativePath;
}

unsigned int loadTexture(char const* path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLint format = GL_RGB;
    if (nrComponents == 1) format = GL_RED;
    else if (nrComponents == 3) format = GL_RGB;   /* JPG image requires GL_RGB */
    else if (nrComponents == 4) format = GL_RGBA;  /* PNG image requires GL_RGBA */

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

unsigned int loadCubeMap(char const** path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrComponents;
  stbi_set_flip_vertically_on_load(true);
  for (unsigned int i = 0; i < 6; i++) {
    unsigned char* data = stbi_load(path[i], &width, &height, &nrComponents, 0);
    if (data) {
      GLint format = GL_RGB;
      if (nrComponents == 1) format = GL_RED;
      else if (nrComponents == 3) format = GL_RGB;   /* JPG image requires GL_RGB */
      else if (nrComponents == 4) format = GL_RGBA;  /* PNG image requires GL_RGBA */

      /* see more at: https://learnopengl.com/Advanced-OpenGL/Cubemaps */
      glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
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

void bindTexture(unsigned int texture) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
}
