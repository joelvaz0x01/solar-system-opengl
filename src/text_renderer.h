#ifndef SOLAR_SYSTEM_TEXT_RENDERER_H
#define SOLAR_SYSTEM_TEXT_RENDERER_H

#include <map>
#include <string>

#include <shader_m.h>

#include "types.h"

/**
 * @brief Initialize the text renderer
 * @param fontPath: path to the font file
 * @return 0 if successful, -1 otherwise
 */
int initTextRenderer(const std::string& fontPath);

/**
 * @brief Render text
 * @param shader: shader to render text
 * @param text: text to render
 * @param x: x position
 * @param y: y position
 * @param scale: text scale
 * @param color: text color
 */
void renderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

/**
 * @brief Function to scale char height
 * @param scale: scale of char height
 * @param isMaxHeight: check if the character is at the top of the screen
 * @return scaled character height
 */
float charHeightScaled(float scale, bool isMaxHeight);

/**
 * @brief Function to scale char width
 * @param scale: scale of char width
 * @param textLength: length of the text
 * @param isMaxWidth: check if the character is at the right of the screen
 * @return scaled character width
 */
float charWidthScaled(float scale, std::string::size_type textLength, bool isMaxWidth);

/**
 * @brief Function to show planet information
 * @param shader: shader to show planet information
 * @param planetIndex: index of the planet
 * @param textColor: color of the text
 * @param textScale: scale of the text
 */
void showPlanetInfo(Shader& shader, unsigned int planetIndex, glm::vec3 textColor, float textScale);

/**
 * @brief Cleanup text renderer resources
 */
void cleanupTextRenderer();

#endif  // SOLAR_SYSTEM_TEXT_RENDERER_H
