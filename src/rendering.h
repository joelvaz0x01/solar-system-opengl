#ifndef SOLAR_SYSTEM_RENDERING_H
#define SOLAR_SYSTEM_RENDERING_H

#include <glad/gl.h>
#include <glm/glm.hpp>

#define PI 3.14159265359f
#define STEP 256

/**
 * @brief Function to render sphere
 */
void renderSphere();

/**
 * @brief Function to build orbit
 * @param radius: radius of the circle
 * @param VAO: vertex array object
 */
void renderOrbit(float radius, unsigned int* VAO);

/**
 * @brief Function to render skybox
 * @param skyboxCubeMap: skybox cube map
 */
void renderSkybox(unsigned int skyboxCubeMap);

/**
 * @brief Function to create planet model matrix
 * @param translation: translation around the sun/planet
 * @param distance: distance from the sun/planet
 * @param rotation: rotation around its own axis
 * @param scale: scale of the planet
 * @param centerModel: center of the model
 * @return model matrix
 */
glm::mat4 planetCreator(float translation, float distance, float rotation, float scale,
                        glm::vec3 centerModel);

#endif  // SOLAR_SYSTEM_RENDERING_H
