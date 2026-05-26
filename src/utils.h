#ifndef SOLAR_SYSTEM_UTILS_H
#define SOLAR_SYSTEM_UTILS_H

#include <string>

/**
 * @brief Get the full path to a resource file
 * @param relativePath: relative path to the resource
 * @return full path to the resource
 */
std::string getResourcePath(const std::string& relativePath);

/**
 * @brief Loads the texture and assign it to textureID
 * @param path: path to texture
 * @return textureID
 */
unsigned int loadTexture(char const* path);

/**
 * @brief Function to load cubeMap texture from file
 * @param path: array of 6 paths to texture (cubeMap)
 * @return textureID
 */
unsigned int loadCubeMap(char const** path);

/**
 * @brief Function to bind texture
 * @param texture: texture to bind
 */
void bindTexture(unsigned int texture);

#endif  // SOLAR_SYSTEM_UTILS_H
